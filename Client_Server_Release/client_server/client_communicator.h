#ifndef CLIENT_COMMUNICATOR_H
#define CLIENT_COMMUNICATOR_H

#include "osdependent.h"
//This class only for sending messages from server to client and receiving messages.
namespace server {
class ClientCommunicator
{
 public:
  explicit ClientCommunicator(standards::Socket new_socket); //receive socket for connection
  ~ClientCommunicator(); //release socket

  bool SendMessage(standards::Packet);
  bool ReceiveMessage(standards::Packet);
  static void NewClientAccepted() {++current_num_clients_;} //I need this function because threads
  static std::size_t GetCurrentNumClients() {return current_num_clients_;}

 private:
  standards::Socket current_socket_; //socket that is being used for sending/receiving messages/data
  static std::size_t current_num_clients_; //
};
}
#endif // CLIENT_COMMUNICATOR_H
