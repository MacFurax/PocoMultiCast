// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
#include "Poco/Net/Net.h"

#include "Poco/Net/MulticastSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/NetworkInterface.h"
#include "Poco/Thread.h"
#include "Poco/Event.h"


class MulticastEchoServer : public Poco::Runnable
  /// A simple sequential Multicast echo server.
{
public:
  MulticastEchoServer();
  /// Creates the MulticastEchoServer.

  ~MulticastEchoServer();
  /// Destroys the MulticastEchoServer.

  Poco::UInt16 port() const;
  /// Returns the port the echo server is
  /// listening on.

  void run();
  /// Does the work.

  void join();

  const Poco::Net::SocketAddress& group() const;
  /// Returns the group address where the server listens.

  const Poco::Net::NetworkInterface& interfc() const;
  /// Returns the network interface for multicasting.

protected:
  static Poco::Net::NetworkInterface findInterface();
  /// Finds an appropriate network interface for
  /// multicasting.

private:
  Poco::Net::MulticastSocket  _socket;
  Poco::Net::SocketAddress    _group;
  Poco::Net::NetworkInterface _if;
  Poco::Thread _thread;
  Poco::Event  _ready;
  bool         _stop;
};