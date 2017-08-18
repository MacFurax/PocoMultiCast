#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <queue>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>


// bass class to implement bus transport
class BusTransport {
public:
  BusTransport() {}
  ~BusTransport() {}

  void queueBufferToSend( std::vector<char> data ) 
  {
    std::lock_guard<std::mutex> guard(mWriteMutex);
    mBufferToSend.push(data);
  }

  int availableReceivedBufferCount() 
  {
    std::lock_guard<std::mutex> guard(mReadMutex);
    return mBufferReceived.size();
  }
  
  void getNextReceivedBuffer( std::vector<char>& data ) 
  {
    //std::vector<char> buf;
    std::lock_guard<std::mutex> guard(mReadMutex);
    data = mBufferReceived.front();
    mBufferReceived.pop();
  }

  void virtual run() {}

protected:
  std::mutex mReadMutex;
  std::mutex mWriteMutex;
  std::queue<std::vector<char>> mBufferToSend;
  std::queue<std::vector<char>> mBufferReceived;
};

class Bus
{
public:
  Bus() {}
  //Bus(std::shared_ptr<BusTransport> transport): mTransport(transport){}
  Bus(std::shared_ptr<BusTransport> transport) : mTransport(transport) {}

  ~Bus() {}

  void sendMessage(std::string message) 
  {
    messagesToSend.push(message);
  }
  
  bool receivedMessage(std::string &message) 
  {
    if (messagesReceived.size() == 0) return false;
    message = messagesReceived.front();
    messagesReceived.pop();
    return true;
  }

  void Run() 
  {
    if (messagesToSend.size() > 0)
    {
      std::vector<char> buf;
      std::string message = messagesToSend.front();
      messagesToSend.pop();
      std::copy( message.begin(), message.end(), buf.begin());
      mTransport->queueBufferToSend( buf );
    }

    if (mTransport->availableReceivedBufferCount() > 0)
    {
      std::vector<char> buf;
      mTransport->getNextReceivedBuffer(buf);
      std::string message;
      std::copy(buf.begin(), buf.end(), message.begin());
      messagesReceived.push(message);
    }

  }

private:
 
  std::shared_ptr<BusTransport> mTransport;
  std::queue<std::string> messagesToSend;
  std::queue<std::string> messagesReceived;
};


class Node {
public:
  Node() {}
  Node( std::string name ) {}
  ~Node() {}

  int sendMessage(std::string message) 
  {
    mBus.sendMessage( message );
  }

  int receivedMessage(std::string &message) 
  {
    mBus.receivedMessage(message);
  }

  void BindToBus(Bus& bus) { mBus = bus; }

  void Run() {}

private:
  Bus mBus;
};

class Service {
public :
  Service() {}
  Service(std::string name) {}

  void BindToNode(std::shared_ptr<Node> node) { mNode = node; }

  void virtual Run() {}

private:
  std::shared_ptr<Node> mNode;
};


class System {
public:
  System() {}

  void Run() {
    while (mRun)
    {
      mBus->Run();
      mNode->Run();
      for (auto s : mServices)
        s->Run();
      callback();
    }
  }

  void Start()
  {
    mRun = true;
    Run();
  }

  void stop() 
  {
    mRun = false;
  }

  void SetBus(std::shared_ptr<Bus> bus) { mBus = bus; }
  void SetNode(std::shared_ptr<Node> node) { mNode = node; }
  void AddService(std::shared_ptr<Service> service) { mServices.push_back(service); }

  void virtual callback() {}

private:
  bool mRun = false;
  std::shared_ptr<Bus> mBus;
  std::shared_ptr<Node> mNode;
  std::vector<std::shared_ptr<Service>> mServices;
};