#include "server.h"

namespace server {
Server::Server():listen_socket_(INVALID_SOCKET) {
#ifdef _WIN32
  WSADATA wsa_data;
  if (WSAStartup(MAKEWORD(2,2), &wsa_data) != 0) {
    std::cerr << "WSAStartup failed " << std::endl;
    exit(1);
  }
#endif
  listen_socket_ = socket(AF_INET,SOCK_STREAM,0); //create socket for TCP connection
  if (listen_socket_ == INVALID_SOCKET) {
    ReleaseResources();
    std::cerr << "Server socket creation failed" << std::endl;
    exit(1);
  }
  server_address_.sin_family = AF_INET;  // Internet protocol family
  server_address_.sin_addr.s_addr = inet_addr(standards::kServerIP.c_str()); // IP adress
  server_address_.sin_port = standards::kPort;
  int is_succed = SOCKET_ERROR;
  is_succed = bind(listen_socket_,(sockaddr *) &server_address_, sizeof(server_address_));
  if(is_succed != 0){                     //associate socket with a port on our local machine
    std::cerr << "Binding was failed" << std::endl;
    ReleaseResources();
    exit(1);
  }
  is_succed = SOCKET_ERROR;
  is_succed = listen(listen_socket_,standards::kMaxNumClientQueue); //kMaxNumClient --- max num of clients in queue
  if(is_succed != 0){
    std::cerr << "Mistake listen call" << std::endl;
    ReleaseResources();
    exit(1);
  }
  std::cout << "Server is waiting for clients..." << std::endl;
}

Server::~Server() {
   ReleaseResources();
}

void Server::StartSession() {
  volatile std::size_t loop = 1;
  while (loop) {
    if (ClientCommunicator::GetCurrentNumClients() < standards::kMaxNumClient) {
      standards::Socket new_client = AcceptNewClient();
      if (new_client == INVALID_SOCKET) {
        std::cout << "Fail in accepting new client" <<std::endl;
        continue;
      }
      ClientCommunicator::NewClientAccepted(); // +1 to number of clients, I have to do it here because
                             //ClientCommunicator object  will be created in function that
                             //thread function start(2 functions).
                             //If I don't, I can get 1 extra client, num of clients lates adding 1 and
                             //new client can be accepted (1 extra)
 //create thread for already accepted client and answer client requests
#ifdef _WIN32
      HANDLE current_thread = (HANDLE)_beginthreadex(0, 0, &NewClientSession, &new_client, 0, 0);
      CloseHandle(current_thread);  //close HANDLE, we don't need HANDLE anymore to manage thread
#else //Linux
      pthread_t current_thread;
      pthread_create(&current_thread, NULL, &NewClientSession, &new_client);
      pthread_detach(current_thread);
#endif
    } else {
//wait 1 second and continue for "old" client disconnection
#ifdef _WIN32
      Sleep(1000);
#else //Linux
      sleep(1);
#endif
    }
  }
}

standards::Socket Server::AcceptNewClient() {
  standards::Socket new_socket = accept(listen_socket_,NULL,0); // we don't need information about client,
                                                        // so we put 0 instead 2 and 3-rd parameters
  if(new_socket == INVALID_SOCKET){
    std::cerr << "Fail in connection new client." << std::endl;
    return INVALID_SOCKET;
  }else{
    standards::Packet greetings;
    sprintf (greetings, "%d", standards::OK); //write OK(10) in greetings
    int is_succed =  send(new_socket, greetings, standards::kPacketSize, 0);
    if((is_succed != SOCKET_ERROR) && (is_succed != 0)) {
      return new_socket;
    } else {
      return INVALID_SOCKET;
    }
  }
}

void Server::ClientHandler (standards::Socket client_socket) {
  ClientCommunicator client(client_socket);
  std::cout << "There are " << ClientCommunicator::GetCurrentNumClients()
              <<" clients connected to server" << std::endl;
  ClientCommandsHandler command_handler(&client);
  bool is_continue = true; //to check whether client has quit, or connection was lost
  standards::Packet request;
  while(is_continue && client.ReceiveMessage(request)) {
    int command = atoi(request);
    switch (command) {
      case standards::QUIT: {
        is_continue = false;
        break;
      }
      case standards::PWD: {
        is_continue = command_handler.GetCurrentWorkingDir();
        break;
      }
      case standards::LS: {
        is_continue = command_handler.GetFilesList();
        break;
      }
      case standards::CD: {
        is_continue = command_handler.ChangeDir(request);
        break;
      }
      case standards::GET: {
        is_continue = command_handler.GetFile(request);
        break;
      }
      default : {
        is_continue = false;
      }
    }
  }
}

void Server::ReleaseResources() {
if(listen_socket_ != INVALID_SOCKET) { //to get the ability to use this function also when socket creation failed
    closesocket(listen_socket_);       //release socket
}
#ifdef _WIN32
  WSACleanup();
#endif
}

#ifdef _WIN32
unsigned int __stdcall Server::NewClientSession(void *socket) {
  standards::Socket current_client = *(static_cast<standards::Socket*> (socket));
  ClientHandler (current_client);   //handle client requests
  _endthreadex(0);   //ends thread and release all thread resources
}
#else //Linux
void *Server::NewClientSession(void *socket) {
  standards::Socket current_client = *(static_cast<standards::Socket*> (socket));
  ClientHandler (current_client);
}
#endif
}
