#include "server_communicator.h"

namespace client{
ServerCommunicator::ServerCommunicator(standards::Socket new_socket) {
#ifdef _WIN32
  WSADATA wsa_data;
  if (WSAStartup(MAKEWORD(2,2), &wsa_data) != 0) {
    std::cerr << "WSAStartup failed " << std::endl;
    exit(1);
  }
#endif
  local_socket_ = new_socket;
}

ServerCommunicator::~ServerCommunicator(){
  closesocket(local_socket_); //release socket
#ifdef _WIN32
  WSACleanup();
#endif
}

bool ServerCommunicator::SendMessage(standards::Packet data){
  int is_succed = send(local_socket_, data, standards::kPacketSize, 0);
  return !(is_succed == SOCKET_ERROR || is_succed == 0);
}

bool ServerCommunicator::ReceiveMessage(standards::Packet data){
  int is_succed = recv(local_socket_, data, standards::kPacketSize, 0);
  return !(is_succed == SOCKET_ERROR || is_succed == 0);  //return false when there was an issue while
}                                                       //receiving data, or no data received (0 byte)

int ServerCommunicator::ReceiveData(standards::Packet data) {
  int num_bytes = recv(local_socket_, data, standards::kPacketSize, 0);
  return (num_bytes == SOCKET_ERROR) ? 0 : num_bytes;
}
}
