#include "user_commands_handler.h"

namespace client{
bool UserCommandsHandler::GetCurrentWorkingDir() {
  standards::Packet command;
  sprintf (command, "%d", standards::PWD); //write PWD in command.
  if(!server_->SendMessage (command)) {
    std::cout << "Unfortunately connection was lost" << std::endl;
    return false;
  }
  standards::Packet current_dir;
  if(server_->ReceiveMessage(current_dir)) {
    std::cout << current_dir << std::endl;
    return true;
  } else {
    std::cout << "Unfortunately connection was lost" << std::endl;
    return false;
  }
}

bool UserCommandsHandler::GetFilesList() {
  standards::Packet command;
  sprintf (command, "%d", standards::LS); //write LS in command
  if(!server_->SendMessage (command)) {
    std::cout << "Unfortunately connection was lost" << std::endl;
    return false;
  }
  standards::Packet files_list;
  if(server_->ReceiveMessage(files_list)) {
    std::cout << files_list << std::endl;
    return true;
  } else {
    std::cout << "Unfortunately connection was lost" << std::endl;
    return false;
  }
}

bool UserCommandsHandler::ChangeDir() {
  std::cout << "To open directory (only from list of current) type directory name" << std::endl;
  std::cout << "To close current directory (go one level up) type --- .." << std::endl;
  std::string  new_path;
  std::cin >> new_path;
  standards::Packet command;
  sprintf (command, "%d", standards::CD);
  std::copy(new_path.begin(), new_path.end() + 1, command + standards::kMarkerSize); //shift to next number
  if(!server_->SendMessage (command)) {
    std::cout << "Unfortunately connection was lost" << std::endl;
    return false;
  }
  standards::Packet answer;
  if(!server_->ReceiveMessage(answer)) {
    std::cout << "Unfortunately connection was lost" << std::endl;
    return false;
  }
  int marker = atoi(answer);
  if(marker != standards::OK) {
    std::cout << "No such directory" << std::endl;
  }
  return true;
}

bool UserCommandsHandler::GetFile() {
  std::cout << "Please type file name from current directiry that you want to receive" << std::endl;
  std::string file_name;
  std::cin >> file_name;
  standards::Packet command;
  sprintf (command, "%d", standards::GET);
  std::copy(file_name.begin(), file_name.end() + 1, command + standards::kMarkerSize);
  if(!server_->SendMessage (command)) {
    std::cout << "Unfortunately connection was lost" << std::endl;
    return false;
  }
  standards::Packet answer;
  if(!server_->ReceiveMessage(answer)) {
    std::cout << "Unfortunately connection was lost" << std::endl;
    return false;
  }
  int marker = atoi(answer);
  if(marker != standards::OK) {
    std::cout << "No such file" << std::endl;
    return true;
  }
  int file_size = atoi(answer + standards::kMarkerSize);
  return ReceiveFile(file_size, file_name);
}

void UserCommandsHandler::Quit() {
  standards::Packet command;
  sprintf (command, "%d", standards::QUIT); //write LS in command
  server_->SendMessage(command);
}

bool UserCommandsHandler::ReceiveFile(std::size_t file_size, std::string file_name) {
  std::cout << "Please type full path to directory where you want to save that file" << std::endl;
  std::string local_path;
  std::cin >> local_path;
  standards::Packet marker;
  if(chdir(local_path.c_str()) != 0) {
    std::cout << "No such directory" << std::endl;
    sprintf (marker, "%d", standards::ERR);
    return server_->SendMessage(marker);
  }
  std::ofstream file (file_name, std::ios::out | std::ios::binary);
  if(!file.is_open()) {
    std::cout << "Cannot create the file" << std::endl;
    sprintf (marker, "%d", standards::ERR);
    return server_->SendMessage(marker);
  }
  sprintf (marker, "%d", standards::OK);
  if(!server_->SendMessage(marker)) {
    return false;
  }
  std::size_t already_received = 0, percent_complete = 0, last_printed = 0;
  standards::Packet buffer;
  while(already_received != file_size) {
    int num_received_bytes = server_->ReceiveData(buffer);
    if(num_received_bytes == 0) {
      std::cout << "Unfortunately connection was lost" << std::endl;
      file.close();
      remove(file_name.c_str()); //if connection problem, remove file
      return false;
    }
    if(already_received + num_received_bytes > file_size) {
      num_received_bytes = file_size - already_received;
    }
    file.write(buffer, num_received_bytes);
    already_received += num_received_bytes;
    percent_complete = (double)already_received/file_size * 100; //next 4 strings for printing state
    if(percent_complete >= last_printed) {
    std::cout << percent_complete  << "% received" << std::endl;
    last_printed += standards::kMinPrintedPercent;
    }
  }
  file.close();
  return true;
}
}
