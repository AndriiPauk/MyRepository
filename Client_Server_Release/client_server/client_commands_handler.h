#ifndef CLIENT_COMMANDS_HANDLER_H
#define CLIENT_COMMANDS_HANDLER_H

#include "client_communicator.h"

namespace server {
class ClientCommandsHandler
{
 public:
  ClientCommandsHandler(ClientCommunicator *client) : client_(client) {
    current_path_ = osdependent::start_path;
  }
  ~ClientCommandsHandler() {}

  bool GetCurrentWorkingDir();
  bool GetFilesList(); //of current directory files
  bool ChangeDir(standards::Packet request);
  bool GetFile(standards::Packet request); // send file to client(GetFile is a command)
 private:
  std::string FilesList(std::string directory); // return list of files in directory
  bool IsPathExist(std::string directory);

  ClientCommunicator *client_;
  std::string current_path_; //save current path to avoid problems with threads
};
}
#endif // CLIENT_COMMANDS_HANDLER_H
