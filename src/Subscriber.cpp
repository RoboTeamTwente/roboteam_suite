//
// Created by Lukas Bos on 28/08/2019.
//

#include "Subscriber.h"
#include <iostream>

void roboteam_proto::Subscriber::init(const std::string & tcpPort, const std::string &topic) {
  this->socket = new zmqpp::socket(this->context, zmqpp::socket_type::sub);
  this->socket->subscribe(topic);
  this->socket->connect(tcpPort);
  running = true;
}

// keep polling for new messages
void roboteam_proto::Subscriber::poll(zmqpp::reactor * reactor) {
  while (running) {
    reactor->poll();
  }
}

roboteam_proto::Subscriber::~Subscriber() {
  running = false;
  reactor.remove(*socket);
  t1.join();
  socket->close();
  delete socket;
}
