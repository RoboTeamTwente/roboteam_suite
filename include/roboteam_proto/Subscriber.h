#ifndef RTT_SUBSCRIBER_H
#define RTT_SUBSCRIBER_H

#include <string>
#include <mutex>
#include <zmqpp/zmqpp.hpp>
#include <zmqpp/reactor.hpp>
#include <google/protobuf/message.h>
#include <functional>
#include "Channel.h"

namespace roboteam_proto {

template <class T_Response>
class Subscriber {
 private:
  Channel channel;
  zmqpp::context context;
  zmqpp::socket * socket;
  zmqpp::reactor * reactor;
  std::thread t1;
  bool running;

  void init(const Channel & channel) {
    std::cout << "[Roboteam_proto] Starting subscriber for " << channel.name << std::endl;
    this->reactor = new zmqpp::reactor();
    this->socket = new zmqpp::socket(this->context, zmqpp::socket_type::sub);
    this->socket->subscribe("");
    this->socket->connect(channel.port);
    running = true;
  }

 public:
  // create a subscriber with a callback function that gets called when new data is available
  // the new data will be available in the function.
  // this constructor can be used for method calls
  template <class T_Instance>
  Subscriber(const Channel & channel, void(T_Instance::*subscriberCallbackMethod)(T_Response & resp), T_Instance * instance)
      : channel(channel) {
    init(channel);

    zmqpp::poller * poller = &reactor->get_poller();
    auto callback = [=](){
      zmqpp::message response;
      if(poller->has_input(* socket) ){
        socket->receive(response);

        google::protobuf::Message * obj = new T_Response;
        bool parseSuccess = obj->ParseFromString(response.get(0));

        if (parseSuccess) {
            auto output = dynamic_cast<T_Response*>(obj);
          (instance->*subscriberCallbackMethod)(*output); // call the subscriberCallback function
        } else {
          std::cerr << "received faulty packet" << std::endl;
        }
      }
    };
    reactor->add(* socket, callback);
    t1 = std::thread(&Subscriber::poll, this);
  }

  // create a subscriber with a callback function that gets called when new data is available
  // the new data will be available in the function.
  // this constructor can be used for free function calls
  Subscriber(const Channel & channel, void (*func)(T_Response & resp))
      : channel(channel) {
    init(channel);

    zmqpp::poller * poller = &reactor->get_poller();
    auto callback = [=]() {
      zmqpp::message response;
      if(poller->has_input(* socket) ){
        socket->receive(response);

        google::protobuf::Message * obj = new T_Response;
        bool parseSuccess = obj->ParseFromString(response.get(0));

        if (parseSuccess) {
          auto output = dynamic_cast<T_Response*>(obj);
          func(*output); // call the subscriberCallback function
        } else {
          std::cerr << "Received faulty packet" << std::endl;
        }
      }
    };
    reactor->add(* socket, callback);
    t1 = std::thread(&Subscriber::poll, this);
  }

  ~Subscriber() {
    std::cout << "[Roboteam_proto] Stopping subscriber for " << channel.name << std::endl;
    running = false;
    t1.join();
    reactor->remove(*socket);
    socket->close();
    delete socket;
    delete reactor;
  }

  void poll() {
    while (running) {
      reactor->poll(500);
    }
  }
};
}

#endif //RTT_SUBSCRIBER_H
