// send.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using Poco::Net::Socket;
using Poco::Net::MulticastSocket;
using Poco::Net::SocketAddress;
using Poco::Net::IPAddress;
using Poco::Timespan;
using Poco::Stopwatch;
using Poco::TimeoutException;
using Poco::InvalidArgumentException;
using Poco::IOException;
using namespace std;

MulticastSocketTest::MulticastSocketTest()
{
}


MulticastSocketTest::~MulticastSocketTest()
{
}


void MulticastSocketTest::testMulticast()
{
  //MulticastEchoServer echoServer;
  SocketAddress group("239.255.1.2", 12345);
  //MulticastSocket ms( SocketAddress::IPv4 );
  MulticastSocket ms;

  cout << "send to echo server\n";
  int n = ms.sendTo("hello\0", 6, group);

  char buffer[256];
  n = ms.receiveBytes(buffer, sizeof(buffer));
  cout << " Receive " << n << " bytes  [" << buffer << "]\n";
  ms.close();
}


int main()
{

  cout << "Send to echo server and wait reply\n";

  MulticastSocketTest mct;
  mct.testMulticast();

  return 0;
}

