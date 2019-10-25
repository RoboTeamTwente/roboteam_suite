//
// Created by Lukas Bos on 28/08/2019.
//

#include "Subscriber.h"
#include <iostream>

void roboteam_proto::Subscriber::init(const std::string & tcpPort, const std::string &topic) {
  this->reactor = new zmqpp::reactor();
  this->socket = new zmqpp::socket(this->context, zmqpp::socket_type::sub);
  this->socket->subscribe(topic);
  this->socket->connect(tcpPort);
  std::cout << "[Roboteam_proto] Subscriber binding to address: " << tcpPort <<  ", with topic: " << topic << std::endl;
  running = true;
}

// keep polling for new messages
void roboteam_proto::Subscriber::poll() {
  while (running) {
     reactor->poll();
  }
}

roboteam_proto::Subscriber::~Subscriber() {
  running = false;
  reactor->get_poller().remove(*socket);
  reactor->remove(*socket);
  t1.join();
  socket->close();
  std::cout << "[Roboteam_proto] Subscriber shut down for address: " << tcpPort <<  ", with topic: " << topic << std::endl;
  delete socket;
  delete reactor;
}
