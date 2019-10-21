//
// Created by Lukas Bos on 28/08/2019.
//

#ifndef RTT_SUBSCRIBER_H
#define RTT_SUBSCRIBER_H

#include <string>
#include <mutex>
#include <zmqpp/zmqpp.hpp>
#include <zmqpp/reactor.hpp>
#include <google/protobuf/message.h>
#include <functional>

namespace roboteam_proto {

class Subscriber {

 private:
  zmqpp::context context;
  zmqpp::socket * socket;
  zmqpp::reactor reactor;
  std::thread t1;
  bool running;

  void init(const std::string & tcpPort, const std::string &topic);

 public:
  // create a subscriber with a callback function that gets called when new data is available
  // the new data will be available in the function.
  // this constructor can be used for method calls
  template <class T_Instance, class T_Response>
  Subscriber(const std::string & tcpPort, const std::string & topic, void(T_Instance::*subscriberCallbackMethod)(T_Response & resp), T_Instance * instance) {
    init(tcpPort, topic);

    zmqpp::poller * poller = &reactor.get_poller();
    auto callback = [=](){
      zmqpp::message response;
      if(poller->has_input(* socket) ){
        socket->receive(response);

        google::protobuf::Message * obj = new T_Response;
        //= static_cast<google::protobuf::Message*>((T_Response*)0);
        std::string responseStr = response.get(0);
        std::string messageStr = responseStr.substr(topic.length(), responseStr.length() - topic.length());
        bool parseSuccess = obj->ParseFromString(messageStr);


        if (parseSuccess) {
            auto output = dynamic_cast<T_Response*>(obj);
          (instance->*subscriberCallbackMethod)(*output); // call the subscriberCallback function
        } else {
          std::cerr << "received faulty packet" << std::endl;
        }
      }
    };
    reactor.add(* socket, callback);
    t1 = std::thread(&Subscriber::poll, this, &reactor);
  }

  // create a subscriber with a callback function that gets called when new data is available
  // the new data will be available in the function.
  // this constructor can be used for free function calls
  template <class T_Response>

  Subscriber(const std::string & tcpPort, const std::string & topic, void (*func)(T_Response & resp)) {
    init(tcpPort, topic);

    zmqpp::poller * poller = &reactor.get_poller();
    auto callback = [=]() {
      zmqpp::message response;
      if(poller->has_input(* socket) ){
        socket->receive(response);

        google::protobuf::Message * obj = new T_Response;
        //= static_cast<google::protobuf::Message*>((T_Response*)0);
        std::string responseStr = response.get(0);
        std::string messageStr = responseStr.substr(topic.length(), responseStr.length() - topic.length());
        bool parseSuccess = obj->ParseFromString(messageStr);

        if (parseSuccess) {
          auto output = dynamic_cast<T_Response*>(obj);
          func(*output); // call the subscriberCallback function
        } else {
          std::cerr << "Received faulty packet" << std::endl;
        }
      }
    };
    reactor.add(* socket, callback);
    t1 = std::thread(&Subscriber::poll, this, &reactor);
  }

  ~Subscriber();
  void poll(zmqpp::reactor * reactor);
};
}

#endif //RTT_SUBSCRIBER_H
