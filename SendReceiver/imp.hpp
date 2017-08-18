#pragma once

#include <atomic>
#include <chrono>

#include "Poco/Net/MulticastSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/NetException.h"
#include "Poco/Net/IPAddress.h"
#include "Poco/Net/IPAddressImpl.h"
#include "Poco/Timespan.h"
#include "Poco/Stopwatch.h"
#include "Poco/Net/Net.h"

#include "proto.hpp"

class MulticastBusTransport : public BusTransport {
public:

  // initialize socket layer
  MulticastBusTransport() :
    mMulticastGroup("239.255.1.2", 12345),
    mServerNetworkInterface(findInterface())
  {
    mMulticastServerSocket.bind(Poco::Net::SocketAddress(Poco::Net::IPAddress(), mMulticastGroup.port()), true);
    mMulticastServerSocket.joinGroup(mMulticastGroup.host(), mServerNetworkInterface);
  }


  MulticastBusTransport(std::string IPv4ToBinTo) :
    mMulticastGroup("239.255.1.2", 12345),
    mServerNetworkInterface(findInterface(IPv4ToBinTo))
  {
    mMulticastServerSocket.bind(Poco::Net::SocketAddress(Poco::Net::IPAddress(), mMulticastGroup.port()), true);
    mMulticastServerSocket.joinGroup(mMulticastGroup.host(), mServerNetworkInterface);
  }

  // close socket layer
  ~MulticastBusTransport()
  {
    mMulticastServerSocket.leaveGroup(mMulticastGroup.host(), mServerNetworkInterface);
    mMulticastServerSocket.close();
  }

  // start receiving tread
  void startThread()
  {
    mIsStartedThread.store(true);
    mThread = std::thread(&MulticastBusTransport::run, this);
  }

  // stop receiving thread 
  void stopThread()
  {
    mIsStartedThread.store(false);
    mThread.join();
  }

  void send(const void* buffer, int bufferSize)
  {
    std::cout << "Sending data\n";
    mMulticastSendSocket.sendTo(buffer, bufferSize, mMulticastGroup);
  }

  // find an netwotk interface to join multicast to
  Poco::Net::NetworkInterface findInterface()
  {
    Poco::Net::NetworkInterface ret;

    Poco::Net::NetworkInterface::Map m = Poco::Net::NetworkInterface::map();
    for (Poco::Net::NetworkInterface::Map::const_iterator it = m.begin(); it != m.end(); ++it)
    {
      if (it->second.supportsIPv4() &&
        it->second.firstAddress(Poco::Net::IPAddress::IPv4).isUnicast() &&
        !it->second.isLoopback() &&
        !it->second.isPointToPoint())
      {
        //return it->second;
        ret = it->second;
      }
    }
    //return NetworkInterface();
    mServerIPAdresse = ret.firstAddress(Poco::Net::IPAddress::Family::IPv4);
    std::cout << "Selected interface is " << ret.displayName() << " with ip " << mServerIPAdresse.toString() << "\n";
    return ret;
  }

  Poco::Net::NetworkInterface findInterface(std::string ipToMatch)
  {
    Poco::Net::NetworkInterface ret = Poco::Net::NetworkInterface::forAddress(Poco::Net::IPAddress(ipToMatch, Poco::Net::IPAddress::IPv4));
    mServerIPAdresse = ret.firstAddress(Poco::Net::IPAddress::Family::IPv4);
    std::cout << "Selected interface is " << ret.displayName() << " with ip " << mServerIPAdresse.toString() << "\n";
    return ret;
  }

  void run() override {

    Poco::Timespan span(250000);

    while (mIsStartedThread.load())
    {
     
      if (mMulticastServerSocket.poll(span, Poco::Net::Socket::SELECT_READ))
      {
        try
        {
          char buffer[1024];
          Poco::Net::SocketAddress sender;
          int n = mMulticastServerSocket.receiveFrom(buffer, sizeof(buffer), sender);
          std::cout << " Receive " << n << " bytes from " << sender.toString() << " [" << buffer << "]\n";
          if (sender.host().toString() == mServerIPAdresse.toString())
          {
            std::cout << "Data Received from ourself discard it!\n";
          }
          else 
          {
            std::lock_guard<std::mutex> guard(mWriteMutex);
            // add the received message in the queue
            std::vector<char> buf(buffer, buffer+n);
            mBufferReceived.push(buf);
          }
        }
        catch (Poco::Exception& exc)
        {
          std::cerr << "threadRunner::multicastServer exception : " << exc.displayText() << " " << exc.message() << std::endl;
        }// try read
      } // if ready to read

      if (mMulticastServerSocket.poll(span, Poco::Net::Socket::SELECT_WRITE))
      {
        try
        {
          // TODO : if there is buffer in the queue send it
        }
        catch (Poco::Exception& exc)
        {
          std::cerr << "threadRunner::multicastServer exception : " << exc.displayText() << " " << exc.message() << std::endl;
        }// try write
      } // if ready to write

      std::this_thread::sleep_for(std::chrono::milliseconds(10));

    } // while 

  } // Run

protected:
  std::atomic<bool> mIsStartedThread{ false };
  std::thread mThread;

  Poco::Net::MulticastSocket  mMulticastServerSocket;
  Poco::Net::IPAddress		  mServerIPAdresse;
  Poco::Net::NetworkInterface mServerNetworkInterface;

  Poco::Net::MulticastSocket  mMulticastSendSocket;
  Poco::Net::SocketAddress    mMulticastGroup;

};
