#include "client.h"

namespace client {
Client::Client() {
#ifdef _WIN32
  WSADATA wsa_data;
  if (WSAStartup(MAKEWORD(2,2), &wsa_data) != 0) {
    std::cerr << "WSAStartup failed " << std::endl;
    exit(1);
  }
#endif
  local_socket_ = socket(AF_INET, SOCK_STREAM, 0); //create socket for TCP connection
  if(local_socket_ == INVALID_SOCKET){
    std::cerr << "Client socket creation failed" << std::endl;
    ReleaseSocket();
    exit(1);
  }
  peer_.sin_family = AF_INET;
  peer_.sin_addr.s_addr = inet_addr(standards::kServerIP.c_str());
  peer_.sin_port = standards::kPort;

  int is_connect = connect(local_socket_,(sockaddr*) &peer_, sizeof(peer_)); //connect to server
  if(is_connect != 0){
    std::cerr << "The server is down... did not connect" << std::endl;
    ReleaseSocket();
    exit(1);
  }else{
    std::cout << "Server is working. Waiting for a session..." << std::endl;
    standards::Packet greetings;
    bool is_succeed =  recv(local_socket_, greetings, standards::kPacketSize,0);
    int marker = atoi(greetings); //convert char[] to int
    if(is_succeed && (marker == standards::OK)) {
      std::cout << "Connection was successfully established." << std::endl;
    } else {
      std::cerr << "Unfortunately connection was lost" << std::endl;
      ReleaseSocket();
      exit(1);
    }
  }
}

Client::~Client() {
  ReleaseSocket();
}

void Client::StartSession() {
  ServerCommunicator server(local_socket_);
  std::cout << "Please choose one of the following commands:" << std::endl;
  std::cout << "quit --- 0" << std::endl;
  std::cout << "pwd  --- 1" << std::endl;
  std::cout << "ls   --- 2" << std::endl;
  std::cout << "cd   --- 3" << std::endl;
  std::cout << "get  --- 4" << std::endl;
  UserCommandsHandler command_handler(&server);
  std::size_t command;
  bool is_continue = true; //to check whether it's the end of session
  while(is_continue && std::cin >> command) {
    switch (command) {
      case standards::QUIT: {
        command_handler.Quit();
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
        is_continue = command_handler.ChangeDir();
        break;
      }
      case standards::GET: {
        is_continue = command_handler.GetFile();
        break;
      }
      default : {
        std::cout << "No such option. Please choose from the list:" << std::endl;
        std::cout << "quit --- 0" << std::endl;
        std::cout << "pwd  --- 1" << std::endl;
        std::cout << "ls   --- 2" << std::endl;
        std::cout << "cd   --- 3" << std::endl;
        std::cout << "get  --- 4" << std::endl;
      }
    }
  }
}

void Client::ReleaseSocket() {
  if(local_socket_ != INVALID_SOCKET) { //to get the ability to use this function also when socket creation failed
    closesocket(local_socket_);       //release socket
  }
#ifdef _WIN32
  WSACleanup();
#endif
}
}
