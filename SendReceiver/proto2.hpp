#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <queue>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
#include <map>
#include <functional>

using namespace std::placeholders;

class Message 
{
public:
  Message() {}
protected:
  std::string target;
  std::string source;
  std::vector<char> data;
};

class Transport2 {
public:
  void queueBufferToSend(void* buffer, int size) {}
  void getNextReceivedBuffer(void* buffer, int& size) {}

  void run() 
  {
    // get message from transport and queue them
    // if there is buffer to send, send them
  }
protected:
 
};


class MessageCoderDecoder2 {
public:
  MessageCoderDecoder2() {}

  void BufferToMessage(void* buffer, int size, Message& message) {}
  void MessageToBuffer(Message message, void* buffer, int size) {}

protected:

};

class Service2;

class Bus2 {
public:
  Bus2() {}
  Bus2(std::shared_ptr<Transport2> transport, std::shared_ptr<MessageCoderDecoder2>) {}

  void RegisterService(Service2& service) {
    // register service name with is receiveMessage methods
    service.sendMessageMethod(std::bind( &Bus2::queueMessageToSend, this, _1));
    mServicesDictionair[service.name()] = std::bind(&Service2::receiveMessage, &service, _1);
  }

  void queueMessageToSend(Message message) {}

  void run() 
  {
    // get message from transport
    // decode it and put decoded message in messages queue
    // dispatcher look in messages queue and dispatch base on message target (node or services)
    // if there is message to send, pick them encode them for sending and send then to transport
  }

protected:
  std::map<std::string, std::function<void(Message& Message)>> mServicesDictionair;
};

class Service2 {
public:
  Service2() {}
  std::string name() { return mName; }

  void sendMessageMethod(std::function<void(Message message)> callback) { mBusSendMessage = callback; }

  // callback function to get message
  void receiveMessage(Message message) {}

  void sendMessage(Message message) 
  {
    //mBus.
    mBusSendMessage(message);
  }

protected:
  std::string mName;
  std::function<void(Message message)> mBusSendMessage;
};