#ifndef CLIENT_H
#define CLIENT_H

#include "server_communicator.h"
#include "user_commands_handler.h"

namespace client {
class Client
{
 public:
  Client();
  ~Client();

  void StartSession();
 private:
  void ReleaseSocket();

  sockaddr_in  peer_;
  standards::Socket local_socket_;
};
}
#endif // CLIENT_H
