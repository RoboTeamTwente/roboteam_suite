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

class Subscriber {
  const std::string PUBLISH_ENDPOINT = "tcp://127.0.0.1:5555";

 private:
  zmqpp::context context;
  zmqpp::socket * socket;
  zmqpp::reactor reactor;
  std::thread t1;
  void createCallbackFunction(void (*subscriberCallback)(std::string));
  void init(const std::string &topic);

 public:
  template <class T>
  Subscriber(std::string topic, void(T::* subscriberCallbackMethod)(std::string response), T& instance);

  Subscriber(std::string topic, void(* subscriberCallbackMethod)(std::string response));

  void poll(zmqpp::reactor * reactor);
  void destroy();
};
}

#endif //RTT_SUBSCRIBER_H
