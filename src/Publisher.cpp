//
// Created by Lukas Bos on 28/08/2019.
//

#include "Publisher.h"
#include <string>
#include <iostream>

// create a publisher which transmits messages over a given topic
roboteam_proto::Publisher::Publisher(const std::string & tcpPort) : tcpPort(tcpPort) {
  socket = new zmqpp::socket(context, zmqpp::socket_type::pub);
  std::cout << "[Roboteam_proto] Publisher binding to address: " << tcpPort << std::endl;
  socket->bind(tcpPort);
}

// send a string over the topic
bool roboteam_proto::Publisher::send(std::string const& topic, std::string message) {
  std::string data = topic + message;
  zmqpp::message transmission;
  transmission << data;
  return socket->send(transmission, true);
}

roboteam_proto::Publisher::~Publisher() {
  socket->close();
  std::cout << "[Roboteam_proto] Publisher shut down for address: " << tcpPort << std::endl;
  delete socket;
}
