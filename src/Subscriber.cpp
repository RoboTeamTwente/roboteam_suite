#include "Subscriber.h"
#include <iostream>

/*
 * Set up a connection.
 */
void roboteam_proto::Subscriber::init(const Channel & channel) {
  std::cout << "[Roboteam_proto] Starting subscriber for " << channel.name << std::endl;
  this->reactor = new zmqpp::reactor();
  this->socket = new zmqpp::socket(this->context, zmqpp::socket_type::sub);
  this->socket->subscribe("");
  this->socket->connect(channel.port);
  running = true;
}

/*
 * Poll for messages
 */
void roboteam_proto::Subscriber::poll() {
  while (running) {
     reactor->poll();
  }
}

/*
 * Graceful shutdown. Stop the polling thread, close the socket and delete the pointers.
 */
roboteam_proto::Subscriber::~Subscriber() {
  std::cout << "[Roboteam_proto] Stopping subscriber for " << channel.name << std::endl;

  running = false;
  reactor->remove(*socket);
  t1.join();
  socket->close();
  delete socket;
  delete reactor;
}
