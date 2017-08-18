// SendReceive.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


class MulticastBusTransport {
public:

  // initialize socket layer
  MulticastBusTransport() :
    mMulticastGroup("239.255.1.2", 12345), 
    mServerNetworkInterface(findInterface()) 
  {
    mMulticastServerSocket.bind(Poco::Net::SocketAddress(Poco::Net::IPAddress(), mMulticastGroup.port()), true);
    mMulticastServerSocket.joinGroup(mMulticastGroup.host(), mServerNetworkInterface);
  }

  MulticastBusTransport( std::string IPv4ToBinTo) :
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
    mThread = std::thread(&MulticastBusTransport::threadRunner, this);
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
	  Poco::Net::NetworkInterface ret = Poco::Net::NetworkInterface::forAddress(Poco::Net::IPAddress( ipToMatch, Poco::Net::IPAddress::IPv4));
	  mServerIPAdresse = ret.firstAddress(Poco::Net::IPAddress::Family::IPv4);
	  std::cout << "Selected interface is " << ret.displayName() << " with ip " << mServerIPAdresse.toString() << "\n";
	  return ret;
  }

protected:
  std::atomic<bool> mIsStartedThread{ false };
  std::thread mThread;

  Poco::Net::MulticastSocket  mMulticastServerSocket;
  Poco::Net::IPAddress		  mServerIPAdresse;
  Poco::Net::NetworkInterface mServerNetworkInterface;

  Poco::Net::MulticastSocket  mMulticastSendSocket;
  Poco::Net::SocketAddress    mMulticastGroup;
  

  void threadRunner() {

    Poco::Timespan span(250000);

    while (mIsStartedThread.load())
    {
      // put here thread code
      if (mMulticastServerSocket.poll(span, Poco::Net::Socket::SELECT_READ))
      {
        try
        {
          char buffer[256];
          Poco::Net::SocketAddress sender;
          int n = mMulticastServerSocket.receiveFrom(buffer, sizeof(buffer), sender);
          std::cout << " Receive " << n << " bytes from " << sender.toString() << " [" << buffer << "]\n";
		  if (sender.host().toString() == mServerIPAdresse.toString())
		  {
			  std::cout << "Data Received from ourself discard it!\n";
		  }
          /*buffer[n - 1] = 'E';
          buffer[n] = 'C';
          buffer[n + 1] = 0;
          mMulticastServerSocket.sendTo(buffer, n + 3, sender);*/
        }
        catch (Poco::Exception& exc)
        {
          std::cerr << "threadRunner::multicastServer exception : " << exc.displayText() << " " << exc.message() << std::endl;
        }
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

  }

};

int no_more_main( int argc, char* argv[])
{

  bool goOn = true;
  char key;

  std::cout << "argc is " << argc << "\n";
  for (int i = 0; i < argc; i++)
  {
	  std::cout << "arg " << i << " = " << argv[i] << "\n";
  }

  std::cout << "\nSendReceive Multicast\n";
  std::cout << "\ncall parameters : " << argv[0] << " ip_to_bind_to\n";
  std::cout << "\nType key then enter to run commande.\n";
  std::cout << "Commandes :\n\tq : quit\n\ts : send\n\n";

  if (argc < 2)
  {
	  std::cout << "Error : ip to bind to is missing in the call arguments\n";
	  exit(-1);
  }

  try
  {
    MulticastBusTransport SndRcv(argv[1]);

    SndRcv.startThread();

    // loop until user want to leave
    while(goOn)
    {
      std::cin >> key;
      switch (key)
      {
      case 'q': 
        goOn = false;
        break;
      case 's' :
        SndRcv.send("Sent", 5);
        break;
      }
    }
    
    SndRcv.stopThread();

  }
  catch (Poco::Exception& exc)
  {
    std::cerr << "threadRunner::multicastServer exception : " << exc.displayText() << " " << exc.message() << std::endl;
  }
    
  return 0;
}

