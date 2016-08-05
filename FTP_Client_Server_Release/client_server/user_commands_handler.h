#ifndef USER_COMMANDS_HANDLER_H
#define USER_COMMANDS_HANDLER_H
#include "server_communicator.h"

namespace client {
class UserCommandsHandler
{
 public:
  UserCommandsHandler (ServerCommunicator *server): server_(server) {}
  ~UserCommandsHandler() {}

  bool GetCurrentWorkingDir();
  bool GetFilesList(); //of current directory
  bool ChangeDir();
  bool GetFile();
  void Quit();
 private:
  bool ReceiveFile(std::size_t file_size, std::string file_name);

  ServerCommunicator *server_;
};
}

#endif // USER_COMMANDS_HANDLER_H
