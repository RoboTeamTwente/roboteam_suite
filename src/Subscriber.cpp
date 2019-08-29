//
// Created by Lukas Bos on 28/08/2019.
//

#include "Subscriber.h"
#include <iostream>
#include <Subscriber.h>

void roboteam_proto::Subscriber::init(const std::string &topic) {
  this->socket = new zmqpp::socket(this->context, zmqpp::socket_type::sub);
  this->socket->subscribe(topic);
  this->socket->connect(this->PUBLISH_ENDPOINT);
}

// keep polling for new messages
void roboteam_proto::Subscriber::poll(zmqpp::reactor * reactor) {
  while (reactor->poll()) { }
}

// when finished this function should be called
void roboteam_proto::Subscriber::destroy() {
  t1.join();
}
