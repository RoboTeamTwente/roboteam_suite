//
// Created by Lukas Bos on 28/08/2019.
//

#include "Subscriber.h"
#include <iostream>


roboteam_proto::Subscriber::Subscriber(std::string topic, subscriberCallback func) {
  socket = new zmqpp::socket(context, zmqpp::socket_type::sub);
  socket->subscribe(topic);
  socket->connect(PUBLISH_ENDPOINT);

  setCallBack(func);
  t1 = std::thread(&Subscriber::poll, this, &reactor);
}

void roboteam_proto::Subscriber::setCallBack(subscriberCallback func) {
  zmqpp::poller * poller = &reactor.get_poller();

  auto callback = [=](){
    zmqpp::message response;
    if(poller->has_input(* socket) ){
      socket->receive(response);
      func(response.get(0));
    }
  };

  reactor.add(* socket, callback);
}

void roboteam_proto::Subscriber::poll(zmqpp::reactor * reactor) {
  while (reactor->poll()) { }
}

void roboteam_proto::Subscriber::destroy() {
  t1.join();
}
