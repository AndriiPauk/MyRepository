#include "server.h"
#include "client.h"

int main() {
  int choice = 0;
  std::cout << "Start as a server --- '1'" << std::endl;
  std::cout << "Start as a client --- '2'" << std::endl;
  std::cin >> choice;
  if (choice == 1){ //SERVER
    server::Server start_client;
    start_client.StartSession();
  }else if (choice == 2){  //CLIENT
    client::Client start_client;
    start_client.StartSession();
  } else {
    std::cout << "There were only 2 options." << std::endl;
  }
  return 0;
}
