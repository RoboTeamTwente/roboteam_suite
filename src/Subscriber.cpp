//
// Created by Lukas Bos on 28/08/2019.
//

#include "Subscriber.h"
#include <iostream>
#include <Subscriber.h>

// create a subscriber with a callback function that gets called when new data is available
// the new data will be available in the function.
// this constructor can be used for free function calls
roboteam_proto::Subscriber::Subscriber(std::string topic, void (*subscriberCallback)(std::string)) {
  init(topic);

  zmqpp::poller * poller = &reactor.get_poller();

  auto callback = [=](){
    zmqpp::message response;
    if(poller->has_input(* socket) ){
      socket->receive(response);
      subscriberCallback(response.get(0)); // call the subscriberCallback function
    }
  };

  reactor.add(* socket, callback);

  t1 = std::thread(&Subscriber::poll, this, &reactor);
}

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
