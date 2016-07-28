#ifndef STANDARDS_H
#define STANDARDS_H

#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <stdlib.h>

//this file contain all constants
namespace standards{
  const std::size_t kPacketSize = 1024;
  const std::size_t kMarkerSize = 2*sizeof(int); // 2 because itoa takes 33 bytes in 32 bytes system
  const std::string kServerIP = "127.0.0.1"; //default server IP
  const std::size_t kPort = 7500;      //default port
  const std::size_t kMaxNumClientQueue = 10; //max number of client in queue waiting for server connection
  const std::size_t kMaxNumClient = 20;    //max number of clients that can be served at the same time
  const std::string kUpLevelDir = "..";
  const std::size_t kMinPrintedPercent = 5; //to print state of file receiving

enum {  //commands id
  QUIT = 0,
  PWD = 1,
  LS = 2,
  CD = 3,
  GET = 4,
  OK = 10,  // marker of success
  ERR = -1
};

typedef int Socket;
typedef char Packet[kPacketSize];
}

#endif // STANDARDS_H
