//
// Created by Lukas Bos on 28/08/2019.
//

#ifndef RTT_SUBSCRIBER_H
#define RTT_SUBSCRIBER_H

#include <string>
#include <mutex>
#include <zmqpp/zmqpp.hpp>
#include <zmqpp/reactor.hpp>

namespace roboteam_proto {

typedef void (* subscriberCallback)(std::string response);

class Subscriber {
  const std::string PUBLISH_ENDPOINT = "tcp://127.0.0.1:5555";

 private:
  zmqpp::context context;
  zmqpp::socket * socket;
  zmqpp::reactor reactor;
  void setCallBack(subscriberCallback func);
  std::thread t1;

 public:
  Subscriber(std::string topic, subscriberCallback func);
  void poll(zmqpp::reactor * reactor);
  void destroy();
};
}

#endif //RTT_SUBSCRIBER_H
