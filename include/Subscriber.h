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
  void init(const std::string &topic);

 public:
  template <class T>
  Subscriber(std::string topic, void(T::* subscriberCallbackMethod)(std::string response), T * instance){
      init(topic);

      zmqpp::poller * poller = &reactor.get_poller();
      auto callback = [=](){
        zmqpp::message response;
        if(poller->has_input(* socket) ){
          socket->receive(response);
          (instance->*subscriberCallbackMethod)(response.get(0)); // call the subscriberCallback function
        }
      };
      reactor.add(* socket, callback);

      t1 = std::thread(&Subscriber::poll, this, &reactor);
  }

  Subscriber(std::string topic, void(* subscriberCallbackMethod)(std::string response));

  void poll(zmqpp::reactor * reactor);
  void destroy();
};
}

#endif //RTT_SUBSCRIBER_H
