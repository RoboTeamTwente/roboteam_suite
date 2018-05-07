#include <QtNetwork>
#include <boost/asio.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/algorithm/hex.hpp>

#include <fstream>
#include <iostream>
#include <ros/ros.h>
#include <chrono>
#include <ios>
#include <string>

namespace b = boost;

#include "roboteam_msgs/RobotCommand.h"
#include "roboteam_msgs/RobotSerialStatus.h"
#include "roboteam_msgs/World.h"
#include "roboteam_msgs/RobotFeedback.h"

#include "roboteam_utils/LastWorld.h"
#include "roboteam_utils/grSim_Commands.pb.h"
#include "roboteam_utils/normalize.h"
#include "roboteam_utils/SlowParam.h"
#include "std_msgs/Bool.h"
#include "std_msgs/Float64MultiArray.h"

#include "roboteam_robothub/packing.h"
#include "roboteam_robothub/GRSim.h"

#define VERIFY_COMMANDS
#define PACKET_SIZE 24

namespace {

bool halt = false;

void processHalt(const std_msgs::Bool::ConstPtr &msg) {
    ROS_INFO_STREAM("Received a halt: " << (int) msg->data );
    halt = msg->data;
}

ros::Publisher pub;

rtt::GRSimCommander grsimCmd(true);
SlowParam<bool> grSimBatch("grsim/batching", true);

// SlowParam<std::string> colorParam("our_color");
// SlowParam<std::string> grsim_ip("grsim/ip", "127.0.0.1");
// SlowParam<int>         grsim_port("grsim/port", 20011);

// Algorithm for efficient GRSim'ing:
// - Receive msg
// - Put in buffer
//
// When is the buffer flushed:
// - After every ros::spinOnce(), flush buffer
//      - Probably the cleanest, simplest. Not sure about effectiveness.
//      - If in one spinonce you receive one robot instruction twice
//        you drop a packet
//      - If it takes multiple cycles to get a packet from everyone then this
//        is a bad solution since it doesn't compress much.
//      - If we run at the same speed as a rolenode however it seems appropriate
//      - Maybe with an escape as soon as it drops one packet?
// - Every 6 messages
//      - Might drop a packet every once in a while
//      - Like this the most atm.
//      - What if there are 2 robots? Need to detect how many there are.
//      - Reset that every 0.25 seconds?
// - Every a certain amount of time
//      - Might drop a packet every once in a while as well
// - When a packet from every robot is received
//      - Needs to be reset every 0.25 seconds or smth to account for robots leaving e.d.
//      - If one rolenode crashes all robots have no instructions for 0.25 secs
//
// - Did some tests. With 4 testx's running it seems quite nicely separated, i.e.
//   0 1 2 3 4 0 1 2 3 4, for some permutation of 0 1 2 3 4.
//   So the following seems a nice approach. Also, the safety thing will make
//   sure that for bad permutations or nodes that run faster than others it will
//   still send packets at a better than worst-case-scenario rate. Did some more
//   tests with randomly started testx's, still works like a charm.
//
// - Every time the buffer has reached the threshold, it will flush.
// - Every 0.25 (or some other amount) of seconds robothub will check
//   how many robots it has seen it that time period
// - That will be its new threshold
// - As long as all rolenodes run at the same Hz it should work fine
// - If one crashes the system recovers in 0.25 secs.
// - Safety: if two (or more/less?) packets are dropped from a specific robot
//   robothub immediately evaluates how many robots there are, changes its
//   threshold, and flushes the buffer.

enum class SerialResultStatus {
    ACK,
    NACK,
    STRANGE_RESPONSE,
    CANT_OPEN_PORT,
    COMMAND_TO_PACKET_FAILED,
    UNKNOWN_WRITE_ERROR,
    UNKNOWN_READ_ERROR,
    CONNECTION_CLOSED_BY_PEER,
	COMMAND_SANITY_CHECK_FAILED
} ;

struct SerialSendResult {
    SerialResultStatus status;
};

std::map<int, roboteam_msgs::RobotSerialStatus> serialStatusMap;
std::map<int, roboteam_msgs::RobotSerialStatus> grsimStatusMap;

std::string serial_file_path = "/dev/serial/by-id/usb-STMicroelectronics_STM32_Virtual_ComPort_00000000001A-if00";
//std::string serial_file_path = "/dev/ttyACM2";
//std::string serial_file_path = "/home/emiel/myfifo";

bool serialPortOpen = false;
boost::asio::io_service io;
boost::asio::serial_port serialPort(io);
SerialResultStatus prevResultStatus = SerialResultStatus::ACK;

int acks = 0;
int nacks = 0;
int networkMsgs = 0;






//between 11 and 23 Bytes, ideally
typedef struct roboAckData{
	//regular fields: 11 Bytes
	uint8_t roboID:5;
	uint8_t wheelLeftFront:1;
	uint8_t wheelRightFront:1;
	uint8_t wheelLeftBack:1;
	uint8_t wheelRightBack:1;
	uint8_t genevaDriveState:1;
	uint8_t batteryState:1;
	int16_t xPosRobot:13;
	int16_t yPosRobot:13;
	int16_t rho:11;
	int16_t theta:11;
	int16_t orientation:11;
	int16_t angularVelocity:11;
	uint8_t ballSensor:7;

	//extra fields (add 12 Bytes)
	uint32_t xAcceleration;
	uint32_t yAcceleration;
	uint32_t angularRate;

} roboAckData;





int char2int(char input)
{
	if(input >= '0' && input <= '9') return input - '0';
	if(input >= 'A' && input <= 'F') return input - 'A' + 10;
	if(input >= 'a' && input <= 'f') return input - 'a' + 10;
	throw std::invalid_argument("char2int : Invalid input string");
}

// This function assumes src to be a zero terminated sanitized string with
// an even number of [0-9a-f] characters, and target to be sufficiently large
void hex2bin(const char* src, std::array<uint8_t, 23>& target)
{
	int i;
	while(*src && src[1])
	{
		target.at(i) = char2int(*src)*16 + char2int(src[1]);
//		printf("%i : Converted %c%c to %u -> %u\n", i, *src, src[1], char2int(*src)*16 + char2int(src[1]), target.at(i));
		src += 2;
		i++;
	}
}


std::string string_to_hex(const std::string& input)
{
	static const char* const lut = "0123456789ABCDEF";
	size_t len = input.length();

	std::string output;
	output.reserve(2 * len);
	for (size_t i = 0; i < len; ++i)
	{
		const unsigned char c = input[i];
		output.push_back(lut[c >> 4]);
		output.push_back(lut[c & 15]);
	}
	return output;
}





void ackPacketToRoboAckData(uint8_t input[23], uint8_t packetlength, roboAckData *output) {
	//input is now specified as an array of size 23. Note that there are also ACK packets of the length 11.
	//You need to use packetlength to know which range of the array contains useful information.
	//The attempt of accessing input[11] to input[22] for a short ACK packet will yield garbage data.

	output->roboID = input[0]>>3; //a
	output->wheelLeftFront = (input[0]>>2)&1; //b
	output->wheelRightFront = (input[0]>>1)&1; //c
	output->wheelLeftBack = (input[0])&1; //d

	output->wheelRightBack = (input[1]>>7)&1; //e
	output->genevaDriveState = (input[1]>>6)&1; //f
	output->batteryState = (input[1]>>5)&1;  //g
	output->xPosRobot = ((input[1]&0b11111)<<8 | input[2]); //h

	output->yPosRobot = ((input[3]<<5) | (input[4]>>3)); //k

	output->rho = (input[4]&0b111)<<8; //m

	output->rho |= input[5]; //m

	output->theta = (input[6]<<3) | (input[7]>>5); //o

	output->orientation = ((input[7]&0b11111)<<6) | (input[8]>>2); //p

	output->angularVelocity = ((input[8]&0b11)<<9) | (input[9]<<1) | ((input[10]>>7)&1); //q

	output->ballSensor = input[10]&0x7f; //s

	if(packetlength < 23)
		return;

	//extra data
	output->xAcceleration = (input[11]<<24) | (input[12]<<16) | (input[13]<<8) | input[14]; //t
	output->yAcceleration = (input[15]<<24) | (input[16]<<16) | (input[17]<<8) | input[18]; //u
	output->angularRate = (input[19]<<24) | (input[20]<<16) | (input[21]<<8) | input[22]; //v

}



















void readStuff(){
	if (!serialPortOpen) {
		ROS_INFO_STREAM("Opening serial port " << serial_file_path << "...");
		boost::system::error_code errorCode;
		serialPort.open(serial_file_path, errorCode);
		switch (errorCode.value()) {
			case boost::system::errc::success:
				serialPortOpen = true;
				ROS_INFO_STREAM("Port " << serial_file_path << " opened");
				break;
			default:
				ROS_FATAL_STREAM("ERROR! Could not open serial port: " << errorCode.message());
				return;
		}
	}
	ROS_DEBUG("Port open");

	// Create a buffer to receive the hex in
	boost::asio::streambuf hexBuffer;
	// Create an error_code object
	b::system::error_code ec;
	// Read bytes into hexBuffer until '\n' appears
	size_t bytesReceived = boost::asio::read_until(serialPort, hexBuffer, '\n', ec);

	// If an error occured, return
	if(ec){
		ROS_WARN_STREAM("An error occured! " << ec.message());
		return;
	}
	// If there is an incorrect number of bytes
	if(bytesReceived != 49){
		ROS_WARN_STREAM("Dropping message with incorrect size " << bytesReceived);
		return;
	}
	ROS_DEBUG("Packet correct");

	// Commit the buffer. (emptying?)
	hexBuffer.commit(bytesReceived);
	// No idea
	std::istream is(&hexBuffer);
	// Create a string to put the hex in
	std::string hexString;
	// Put the hex in the string
	std::getline(is, hexString);
	// replace \n with \0. Not used atm, but can't hurt
	hexString[bytesReceived-1] = '\0';
	ROS_DEBUG("String null-terminated");
	// Create an array to hold the 23 bytes
	std::array<uint8_t, 23> byteArray {{}};
	// Convert the hex string to an uint8_t array. +2 to drop the ACK byte
	hex2bin(hexString.c_str()+2, byteArray);

	ROS_DEBUG("Packet converted to byte array");

	// === Print the values === //
	std::cout << "received " << bytesReceived << " bytes " << hexString << std::endl;
	for(int i = 0; i < 48; i += 2){
		printf("0x%c%c\t", hexString[i], hexString[i+1]);
	}
	printf("\n\t");
	for(int i = 0; i < 23; i++){
		printf("%i\t", byteArray.at(i));
	}
	printf("\n");
	// ======================== //

	roboAckData rad;
	// Convert the uint8_t array to roboAckData
	ackPacketToRoboAckData(byteArray.data(), 23, &rad);
	// Convert the uint8_t array to LowLevelRobotFeedback
	rtt::LowLevelRobotFeedback llrc = rtt::createRobotFeedback(byteArray);

	roboteam_msgs::RobotFeedback msg = rtt::toRobotFeedback(llrc);

	// Print the roboAckData, LowLevelRobotFeedback, and RobotFeedback
	std::cout << "roboID           : " << +rad.roboID  			<< " " << llrc.id 				<< " " << msg.id 				<< std::endl;
	std::cout << "wheelLeftFront   : " << +rad.wheelLeftFront  	<< " " << llrc.wheelLeftFront   << " " <<(msg.wheelLeftFront  	? "1" : "0") << std::endl;
	std::cout << "wheelRightFront  : " << +rad.wheelRightFront  << " " << llrc.wheelRightFront  << " " <<(msg.wheelRightFront 	? "1" : "0") << std::endl;
	std::cout << "wheelLeftBack    : " << +rad.wheelLeftBack  	<< " " << llrc.wheelLeftBack    << " " <<(msg.wheelLeftBack   	? "1" : "0") << std::endl;
	std::cout << "wheelRightBack   : " << +rad.wheelRightBack  	<< " " << llrc.wheelRightBack   << " " <<(msg.wheelRightBack  	? "1" : "0") << std::endl;
	std::cout << "genevaDriveState : " << +rad.genevaDriveState << " " << llrc.genevaDriveState << " " <<(msg.genevaDriveState	? "1" : "0") << std::endl;
	std::cout << "batteryState     : " << +rad.batteryState  	<< " " << llrc.batteryState  	<< " " <<(msg.batteryState  	? "1" : "0") << std::endl;
	std::cout << "xPosRobot        : " << +rad.xPosRobot  		<< " " << llrc.position_x       << " " << msg.position_x      	<< std::endl;
	std::cout << "yPosRobot        : " << +rad.yPosRobot  		<< " " << llrc.position_y       << " " << msg.position_y      	<< std::endl;
	std::cout << "rho              : " << +rad.rho  			<< " " << llrc.rho  			<< " " << msg.rho  				<< std::endl;
	std::cout << "theta            : " << +rad.theta  			<< " " << llrc.theta  			<< " " << msg.theta  			<< std::endl;
	std::cout << "orientation      : " << +rad.orientation  	<< " " << llrc.rotation  	    << " " << msg.rotation  	   	<< std::endl;
	std::cout << "angularVelocity  : " << +rad.angularVelocity  << " " << llrc.angularVelocity  << " " << msg.angularVelocity 	<< std::endl;
	std::cout << "ballSensor       : " << +rad.ballSensor  		<< " " << llrc.ballSensor  		<< " " << msg.ballSensor  		<< std::endl;
	std::cout << "xAcceleration    : " << +rad.xAcceleration  	<< " " << llrc.acceleration_x   << " " << msg.acceleration_x  	<< std::endl;
	std::cout << "yAcceleration    : " << +rad.yAcceleration  	<< " " << llrc.acceleration_y   << " " << msg.acceleration_y  	<< std::endl;
	std::cout << "angularRate      : " << +rad.angularRate 		<< " " << llrc.velocity_angular << " " << msg.velocity_angular	<< std::endl;

	std::cout << "\n\n" << std::endl;

}


// Returns true if ack, false if nack.
SerialSendResult sendSerialCommands(const roboteam_msgs::RobotCommand& _msg) {

    SerialSendResult result;
    result.status = SerialResultStatus::ACK;

    // ==============
    // Check port
    // ==============
    // If there is something with the serial port
    if (!serialPortOpen) {
        ROS_INFO_STREAM("Opening serial port " << serial_file_path << "...");
        boost::system::error_code errorCode;
        serialPort.open(serial_file_path, errorCode);
        switch (errorCode.value()) {
            case boost::system::errc::success:
                serialPortOpen = true;
                ROS_INFO_STREAM("Port " << serial_file_path << " opened");
                break;
            default:
                ROS_ERROR_STREAM_THROTTLE(1, "ERROR! Could not open serial port: " << errorCode.message());

                result.status = SerialResultStatus::CANT_OPEN_PORT;
                return result;
        }
    }
	ROS_DEBUG("Port is still open...");

    b::optional<roboteam_msgs::World> worldOpt;
    if (rtt::LastWorld::have_received_first_world()) {
        worldOpt = rtt::LastWorld::get();
    }

    // ================
    // Create message
    // ================
	ROS_DEBUG("Creating message...");
	if (auto bytesOpt = rtt::createRobotPacket(_msg, worldOpt)) {
        // Success!

        auto bytes = *bytesOpt;

        // Write message to it
        // TODO: read/write can throw!
        b::system::error_code ec;
		ROS_DEBUG("Writing message...");
		b::asio::write(serialPort, boost::asio::buffer(bytes.data(), bytes.size()), ec);
		ROS_DEBUG("Message written");
        switch (ec.value()) {
            case boost::asio::error::eof:
                ROS_ERROR_STREAM("Connection closed by peer while writing! Closing port and trying to reopen...");
                result.status = SerialResultStatus::CONNECTION_CLOSED_BY_PEER;
                serialPort.close();
                serialPortOpen = false;
                return result;
                break;
            case boost::system::errc::success:
				ROS_DEBUG("Success!");
                // Everything's just peachy!
                break;
            default:
                // Unknown read error!
                ROS_ERROR_STREAM("Unknown write error! Closing the port and trying to reopen...");
                serialPort.close();
                serialPortOpen = false;
                result.status = SerialResultStatus::UNKNOWN_WRITE_ERROR;
                return result;
                break;
        }

        // Listen for ack
        // CAREFUL! The first ascii character is the robot ID
        // _________________________________________
        // _____________                ____________
        // _____________ IN HEXADECIMAL ____________
        // _________________________________________

        int const returnSize = PACKET_SIZE;	// Will become 24 bytes with the new protocol. 1st byte = ACK/NACK
        std::array<uint8_t, returnSize> ackCode;
        ackCode.fill('!');
		ROS_DEBUG("Reading message...");
        b::asio::read(serialPort, boost::asio::buffer(ackCode.data(), returnSize), ec);
		ROS_DEBUG("Message read");

        switch (ec.value()) {
            case boost::asio::error::eof:
                ROS_ERROR_STREAM("Connection closed by peer while reading! Closing port and trying to reopen...");
                result.status = SerialResultStatus::CONNECTION_CLOSED_BY_PEER;
                serialPort.close();
                serialPortOpen = false;
                return result;
                break;
            case boost::system::errc::success:
				ROS_DEBUG("Success!");
                // Peachy!
                break;
            default:
                ROS_ERROR_STREAM("Unknown read error! Closing the port and trying to reopen...");
                serialPort.close();
                serialPortOpen = false;
                result.status = SerialResultStatus::UNKNOWN_READ_ERROR;
                return result;
                break;
        }

//         Uncomment for debug info
         ROS_INFO_STREAM("AckCode size: " << ackCode.size() );
         ROS_INFO_STREAM("ackCodes: ");

         for (int i = 0; i < returnSize; i += 2) {
             uint8_t b = ackCode[i];
             printf("%c%c\n", ackCode[i], ackCode[i + 1]);
         }

        ROS_WARN_STREAM_THROTTLE(1, "Messages not checked for ACK or NACK!");

        return result;

//        auto ack = rtt::decodeOldACK(ackCode);
//
////        ROS_INFO_STREAM("Robot ID : " << ack.robotID );
//        // ROS_INFO_STREAM("Random value : " << ack.randomValue );
//
//        if (ack.robotACK) {
//            result.status = SerialResultStatus::ACK;
//            return result;
//        } else {
//            result.status = SerialResultStatus::NACK;
//            return result;
//        }

        // TODO: @Performance this should probably done in such a way that it doesn't
        // block ros::spin()
        // Altough it is pretty fast now at 1 packet per ms.
        // TODO: @Incomplete we do not handle the ACK here. Should probably influence the order or something (round robin?)
    } else {
        // Oh shit.
        result.status = SerialResultStatus::COMMAND_TO_PACKET_FAILED;
        return result;
    }
}

enum class RobotType {
    SERIAL,
    GRSIM,
};

b::optional<RobotType> stringToRobotType(std::string const & t) {
    if (t == "serial" || t == "proto") {
        return RobotType::SERIAL;
    } else if (t == "grsim") {
        return RobotType::GRSIM;
    } else {
        return b::none;
    }
}

std::string robotTypeToString(RobotType const t) {
    switch(t) {
        case RobotType::SERIAL:
            return "serial";
        case RobotType::GRSIM:
            return "grsim";
        default:
            return "undefined";
    }
}

std::map<int, RobotType> robotTypes;

void updateRobotTypes() {
    for (int i = 0; i < 16; ++i) {
        std::string paramName = "robot" + std::to_string(i) + "/robotType";
        if (ros::param::has(paramName)) {
            std::string paramRobotType = "";
            ros::param::get(paramName, paramRobotType);

            if (auto robotTypeOpt = stringToRobotType(paramRobotType)) {
                robotTypes[i] = *robotTypeOpt;
            } else {
                auto robotTypeIt = robotTypes.find(i);
                if (robotTypeIt != robotTypes.end()) {
                    ROS_WARN_STREAM("Unexpected value set for the robot type of robot "
                            << i
                            << ": \""
                            << paramRobotType
                            << "\". Leaving setting at: "
                            << robotTypeToString(robotTypes[i])
                            );
                } else {
                    ROS_WARN_STREAM("Unexpected value set for the robot type of robot "
                            << i
                            << ": \""
                            << paramRobotType
                            << "\". Leaving setting at unset."
                            );
                }
            }
        } else {
            // No type detected for robot. That's okay.
            // But delete the param from the map if it's there
            auto typeIt = robotTypes.find(i);
            if (typeIt != robotTypes.end()) {
                robotTypes.erase(typeIt);
            }
        }
    }
}

b::optional<RobotType> getMode(int id) {
    auto const typeIt = robotTypes.find(id);

    if (typeIt != robotTypes.end()) {
        return typeIt->second;
    } else {
        return b::none;
    }
}

std::set<RobotType> getDistinctTypes() {
    std::set<RobotType> types;

    for (auto const & pair : robotTypes) {
        types.insert(pair.second);
    }

    return types;
}

void sendCommand(roboteam_msgs::RobotCommand command) {
    // ROS_INFO_STREAM("[sendCommand] " << command.id << std::endl;
    auto mode = getMode(command.id);

    if (mode == b::none) {
        // We skip this command, it's not meant for us
        // No mode set!
    } else if (mode == RobotType::GRSIM) {
        grsimCmd.queueGRSimCommand(command);
        grsimStatusMap[command.id].acks++;
        networkMsgs++;
    } else if (mode == RobotType::SERIAL) {
        auto result = sendSerialCommands(command);

        switch (result.status) {
            case SerialResultStatus::ACK:
                acks++;
                serialStatusMap[command.id].acks++;
                break;
            case SerialResultStatus::NACK:
                nacks++;
                serialStatusMap[command.id].nacks++;
                break;
            default:
                break;
        }
        prevResultStatus = result.status;
    }
}

void processRobotCommandWithIDCheck(const ros::MessageEvent<roboteam_msgs::RobotCommand> & msgEvent) {
    //ROS_INFO_STREAM("[processRobotCommandWithIDCheck]" << std::endl;

    auto const & msg = *msgEvent.getMessage();

    auto pubName = msgEvent.getPublisherName();

    int robotLoc = pubName.find("robot");
    if (robotLoc != std::string::npos) {
        b::optional<int> robotID;

        try {
            auto robotIDLength2 = std::stoi(pubName.substr(robotLoc + 5, 2));
            robotID = robotIDLength2;
        } catch (...) {
            try {
                auto robotIDLength1 = std::stoi(pubName.substr(robotLoc + 5, 1));
                robotID = robotIDLength1;
            } catch (...) {
                // Neither matched; do nothing.
                ROS_ERROR("Neither matched!");
            }
        }

        if (robotID) {
            if (*robotID == msg.id) {
                sendCommand(msg);
            } else {
                ROS_ERROR_STREAM("Message sent by robot " << *robotID << " is not the same as ID in message which is " << msg.id << "!");
            }
        } else {
            // Parsing failed; let the message through, probably some test node.
            ROS_ERROR("Parsing failed!");
        }
    } else {
        // In this case the msg was sent by something else
        sendCommand(msg);
    }
}

bool hasArg(std::vector<std::string> const & args, std::string const & arg) {
    return std::find(args.begin(), args.end(), arg) != args.end();
}

} // anonymous namespace

int main(int argc, char *argv[]) {
    std::vector<std::string> args(argv, argv + argc);


    // ┌──────────────────┐
    // │  Initialization  │
    // └──────────────────┘


    // Create ROS node called robothub and subscribe to topic 'robotcommands'
    ros::init(argc, argv, "robothub");
    ros::NodeHandle n;

    // Get the number of iterations per second
    int roleHz = 120;
    ros::param::get("role_iterations_per_second", roleHz);
    ros::Rate loop_rate(30);

	std::chrono::high_resolution_clock::time_point lastStatistics = std::chrono::high_resolution_clock::now();

	while (ros::ok()) {
		loop_rate.sleep();
		ros::spinOnce();

		auto timeNow = std::chrono::high_resolution_clock::now();
		auto timeDiff = timeNow - lastStatistics;

		// ┌──────────────────┐
		// │   Every second   │
		// └──────────────────┘
//		if (std::chrono::duration_cast<std::chrono::milliseconds>(timeDiff).count() > 100) {
			ROS_DEBUG("\nTick");
			lastStatistics = timeNow;

			readStuff();

//		}
	}

	return 0;







	/*


    ROS_INFO_STREAM("Refresh rate: " << roleHz << "hz");

    // Publisher and subscriber to get robotcommands
    ros::Subscriber subRobotCommands = n.subscribe("robotcommands", 1000, processRobotCommandWithIDCheck);

    // Halt subscriber
    ros::Subscriber subHalt = n.subscribe("halt", 1, processHalt);

    // Publisher for the serial status
    ros::Publisher pubSerialStatus = n.advertise<roboteam_msgs::RobotSerialStatus>("robot_serial_status", 100);

    using namespace std;
    using namespace std::chrono;

    high_resolution_clock::time_point lastStatistics = high_resolution_clock::now();

    // Get the initial robot types from rosparam
    updateRobotTypes();


    // ┌──────────────────┐
    // │    Main loop     │
    // └──────────────────┘

    int currIteration = 0;

    while (ros::ok()) {
        loop_rate.sleep();
        ros::spinOnce();

        // Stop all robots if needed!
        if (halt) {
            roboteam_msgs::RobotCommand command;
            for (int i = 0; i < 16; ++i) {
                command.id = i;

                sendCommand(command);
            }
        }

        grsimCmd.setBatch(grSimBatch());

        auto timeNow = high_resolution_clock::now();
        auto timeDiff = timeNow - lastStatistics;

        // ┌──────────────────┐
        // │   Every second   │
        // └──────────────────┘
        if (duration_cast<milliseconds>(timeDiff).count() > 1000) {
            // Get the robot types from rosparam
            updateRobotTypes();

            lastStatistics = timeNow;

            ROS_INFO_STREAM("==========| " << currIteration++ << " |==========");

            if (halt) {
                ROS_INFO_STREAM("Halting status : !!!HALTING!!!");
            } else {
                ROS_INFO_STREAM("Halting status : Not halting");
            }

            auto modes = getDistinctTypes();

            // ┌──────────────────┐
            // │      Serial      │
            // └──────────────────┘
            if (modes.find(RobotType::SERIAL) != modes.end()) {
                ROS_INFO_STREAM("---- Serial ----");

                // Get the percentage of acks and nacks
                auto total = acks + nacks;
                auto ackPercent = static_cast<int>((acks / (double) total) * 100);
                auto nackPercent = static_cast<int>((nacks / (double) total) * 100);

                if (total == 0) {
                    ackPercent = 0;
                    nackPercent = 0;
                }

                // If the port failed to open, print a message that we're trying to open it
                if (prevResultStatus == SerialResultStatus::CANT_OPEN_PORT) {
                    ROS_INFO_STREAM("Trying to open port " << serial_file_path << "...");
                } else {
                    // Otherwise everything was fine and dandy.
                    // (Any other errors are handled by the sendSerial function itself by printing)
                    // Print the status.
                    ROS_INFO_STREAM("Current port: " << serial_file_path );
                    ROS_INFO_STREAM("Sent messages the past second: " << total );
                    ROS_INFO_STREAM("Capacity: " << std::floor(total / 360.0) << "%");
                    ROS_INFO_STREAM("Acks    : " << acks << " (" << ackPercent << ")");
                    ROS_INFO_STREAM("Nacks   : " << nacks << " (" << nackPercent << ")");
                    acks=0;
                    nacks=0;

                    // Publish any status info on the robots
                    for (auto &pair : serialStatusMap) {
                        int id = pair.first;
                        roboteam_msgs::RobotSerialStatus &status = pair.second;

                        if (status.start_timeframe == ros::Time(0)) {
                            // This is a new status message.
                            // It has been initialized with an id of 0.
                            // So we need to set it correctly.
                            status.id = id;
                        } else {
                            // This status message is ready to be sent.
                            status.end_timeframe = ros::Time::now();

                            pubSerialStatus.publish(status);
                        }

                        // Set the new start time.
                        status.start_timeframe = ros::Time::now();
                        // Reset the ack and nack counters.
                        status.acks = 0;
                        status.nacks = 0;
                    }
                }
            }
            // ┌──────────────────┐
            // │      grSim       │
            // └──────────────────┘
            if (modes.find(RobotType::GRSIM) != modes.end()) {

                // If there's a robot set to GRSim print the amount of network messages sent
                ROS_INFO_STREAM("---- GRSim ----");
                ROS_INFO_STREAM("Network messages sent: " << networkMsgs );
                networkMsgs = 0;


                // Publish any status info on the robots
                for (auto &pair : grsimStatusMap) {
                    int id = pair.first;
                    roboteam_msgs::RobotSerialStatus &status = pair.second;

                    if (status.start_timeframe == ros::Time(0)) {
                        // This is a new status message.
                        // It has been initialized with an id of 0.
                        // So we need to set it correctly.
                        status.id = id;
                    } else {
                        // This status message is ready to be sent.
                        status.end_timeframe = ros::Time::now();

                        pubSerialStatus.publish(status);
                    }

                    // Set the new start time.
                    status.start_timeframe = ros::Time::now();
                    // Reset the ack and nack counters.
                    status.acks = 0;
                    status.nacks = 0;
                }

                if (grsimCmd.isBatch()) {
                    ROS_INFO_STREAM("Batching : yes");
                    rtt::GRSimCommander::Stats stats = grsimCmd.consumeStatistics();

                    ROS_INFO_STREAM("Average efficiency:       " << std::floor(stats.averageEfficiency * 100) << "%");
                    ROS_INFO_STREAM("Number of forced flushes: " << stats.numForcedFlushes);
                    ROS_INFO_STREAM("Current threshold:        " << stats.threshold);
                } else {
                    ROS_INFO_STREAM("Batching : no");
                }
            }
        }
    }

    return 0;
    */
}
