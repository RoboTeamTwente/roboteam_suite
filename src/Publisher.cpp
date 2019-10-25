#include "Publisher.h"
#include <string>
#include <iostream>

namespace roboteam_proto {
//
//// create a publisher which transmits messages over a given topic
//template<class T>
//Publisher<T>::Publisher(const Channel & channel) : channel(channel) {
//  std::cout << "[Roboteam_proto] Starting publisher for channel " << channel.name << std::endl;
//  socket = new zmqpp::socket(context, zmqpp::socket_type::pub);
//  socket->bind(channel.port);
//}
//
//// send a message over the topic
//template<class T>
//bool Publisher<T>::send(const T & message) {
//  zmqpp::message transmission;
//  transmission << message.SerializeAsString();
//  return socket->send(transmission, true);
//}
//
//template<class T>
//Publisher<T>::~Publisher() {
//  std::cout << "[Roboteam_proto] Stopping publisher for channel " << channel.name << std::endl;
//  socket->close();
//  delete socket;
//}

}