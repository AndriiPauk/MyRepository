#ifndef SERVER_H
#define SERVER_H

#include "client_communicator.h"
#include "client_commands_handler.h"

namespace server {
class Server{
 public:
  Server();        //create and bind socket, listen to clients
  ~Server();       //close main server socket

  void StartSession();
 private:
  standards::Socket AcceptNewClient();  //accept new client and returm socket for connection
  void ReleaseResources();
  static void ClientHandler (standards::Socket client_socket); //static to get the ability to call it in thread
#ifdef _WIN32
  static unsigned int __stdcall NewClientSession(void *socket);
#else //Linux
  static void *NewClientSession(void *socket);
#endif

  sockaddr_in  server_address_;
  standards::Socket listen_socket_;         //I use it only to establish connection, not for messaging
};
}
#endif // SERVER_H
