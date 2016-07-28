#include "client_communicator.h"

namespace server{
std::size_t ClientCommunicator::current_num_clients_ = 0;

ClientCommunicator::ClientCommunicator(standards::Socket new_socket) {
#ifdef _WIN32
  WSADATA wsa_data;
  if (WSAStartup(MAKEWORD(2,2), &wsa_data) != 0) {
    std::cerr << "WSAStartup failed " << std::endl;
    exit(1);
  }
#endif
  current_socket_ = new_socket;
}

ClientCommunicator::~ClientCommunicator(){
  --current_num_clients_;
  std::cout << "There are " << GetCurrentNumClients()
              <<" clients connected to server" << std::endl;
  closesocket(current_socket_); //release socket
#ifdef _WIN32
  WSACleanup();
#endif
}

bool ClientCommunicator::SendMessage(standards::Packet data){
  int is_succed = send(current_socket_, data, standards::kPacketSize,0);
  return !(is_succed == SOCKET_ERROR || is_succed == 0);
}

bool ClientCommunicator::ReceiveMessage(standards::Packet data){
  int is_succed = recv(current_socket_, data, standards::kPacketSize,0);
  return !(is_succed == SOCKET_ERROR || is_succed == 0);  //return false when there was an issue while
}                                                         //receiving data, or no data received (0 byte)
}
