#ifndef WORLDHANDLER_H
#define WORLDHANDLER_H

#include <roboteam_proto/RobotData.pb.h>
#include <roboteam_proto/State.pb.h>
#include <net/robocup_ssl_client.h>
#include <Publisher.h> //TODO: ugly include, fix in utils
#include <Subscriber.h>
#include <observer/Observer.h>
#include <utility>



class Handler {
   private:
    std::unique_ptr<proto::Publisher<proto::State>> pub_state = nullptr;
    std::unique_ptr<RoboCupSSLClient> vision_client = nullptr;
    std::unique_ptr<RoboCupSSLClient> refbox_client = nullptr;
    std::unique_ptr<proto::Subscriber<proto::RobotData>> sub_feedback = nullptr;
    Observer observer;
    std::vector<proto::RobotData> receivedRobotData;
   public:
    Handler() = default;

    /*
     * Setup a world with a kalmanfilter, and initialize the publishers for publishing data.
     */
    void init();
    void start();
    std::vector<proto::SSL_WrapperPacket> receiveVisionPackets();
    std::vector<proto::SSL_Referee> receiveRefereePackets();
    void setupSSLClients();
    void robotDataCallBack(proto::RobotData& data);
};


#endif