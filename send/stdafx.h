// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
#include <iostream>

#include "Poco/Net/MulticastSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/NetException.h"
#include "Poco/Net/IPAddress.h"
#include "Poco/Net/IPAddressImpl.h"
#include "Poco/Timespan.h"
#include "Poco/Stopwatch.h"
#include "Poco/Net/Net.h"

class MulticastSocketTest
{
public:
  MulticastSocketTest();
  ~MulticastSocketTest();

  void testMulticast();

private:
};