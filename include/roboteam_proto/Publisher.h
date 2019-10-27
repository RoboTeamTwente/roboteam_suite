#ifndef RTT_PUBLISHER_H
#define RTT_PUBLISHER_H

#include <string>
#include <zmqpp/zmqpp.hpp>
#include <google/protobuf/message.h>
#include <roboteam_utils/constants.h>
#include "Channel.h"

namespace roboteam_proto {

/*
 * Defines a publisher that publishes to a TCP channel
 * The type of the messages passed has to be specified beforehand in the template.
 *
 * This class is designed according to RAII: creating the object immediately binds to the channel,
 * and deleting the object immediately unbinds to the channel and cleans up memory.
 *
 * @author Lukas Bos
 * @date 26-10-2019
 */
template<class T>
class Publisher {
 private:
  zmqpp::context context;
  zmqpp::socket * socket;
  Channel channel;

 public:
  // Apply rule of 5: in all cases we cannot copy/move this object
  Publisher(const Publisher &copy) = delete;
  Publisher &operator=(const Publisher &other) = delete;
  Publisher(const Publisher &&copy) = delete;
  Publisher &operator=(Publisher &&data) = delete;

  /*
   * Open a publisher and bind to the channel.
   *
   * @param channel: The channel to publish to.
   */
  explicit Publisher(const roboteam_utils::ChannelType & channelType)
        : channel (roboteam_utils::CHANNELS.at(channelType)) {
      std::cout << "[Roboteam_proto] Starting publisher for channel " << channel.name << std::endl;
      socket = new zmqpp::socket(context, zmqpp::socket_type::pub);
      socket->bind(channel.port);
  }

  /*
  * closes the socket before deleting the publisher
  */
  ~Publisher() {
      std::cout << "[Roboteam_proto] Stopping publisher for channel " << channel.name << std::endl;
      socket->close();
      delete socket;
  }

  /*
   * Send a message of the specified type
   * @param message: message to send. Must consist of base google::protobuf::Message to compile!
   */
  bool send(const T &message) {
      zmqpp::message transmission;
      transmission << message.SerializeAsString();
      return socket->send(transmission, true);
  }
};
}

#endif //RTT_PUBLISHER_H
