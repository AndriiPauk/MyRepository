#ifndef SERVER_COMMUNICATOR_H
#define SERVER_COMMUNICATOR_H

#include "osdependent.h"
//This class only for sending messages from server to client and receiving messages.
namespace client {
class ServerCommunicator
{
 public:
  explicit ServerCommunicator(standards::Socket new_socket); //receive socket for connection
  ~ServerCommunicator();  //release socket

  bool SendMessage(standards::Packet);
  bool ReceiveMessage(standards::Packet);
  int ReceiveData(standards::Packet); //because for receiving file I need bytes control
 private:

  standards::Socket local_socket_; //socket for TCP communication with server
};
}
#endif // SERVER_COMMUNICATOR_H
