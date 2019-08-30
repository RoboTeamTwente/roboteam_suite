//
// Created by Lukas Bos on 28/08/2019.
//

#include "Publisher.h"
#include <string>

// create a publisher which transmits messages over a given topic
roboteam_proto::Publisher::Publisher() {
  socket = new zmqpp::socket(context, zmqpp::socket_type::pub);
  socket->bind(PUBLISH_ENDPOINT);
}

// send a string over the topic
void roboteam_proto::Publisher::send(std::string const& topic, std::string message) {
  std::string data = topic + message;
  zmqpp::message transmission;
  transmission << data;
  socket->send(transmission);
}