#include "client_commands_handler.h"

namespace server {
bool ClientCommandsHandler::GetCurrentWorkingDir() {
  standards::Packet answer;
  copy(current_path_.begin(), current_path_.end() + 1, answer); // +1 to write '\0'
  return client_->SendMessage(answer);
}

bool ClientCommandsHandler::GetFilesList() {
  std::string list_of_files = FilesList(current_path_);
  standards::Packet answer;
  copy(list_of_files.begin(), list_of_files.end() + 1, answer);
  return client_->SendMessage(answer);
}

bool ClientCommandsHandler::ChangeDir(standards::Packet request) {
  std::string change_path(request + standards::kMarkerSize);
  standards::Packet answer;
  if(change_path == standards::kUpLevelDir) {
    std::string temporary_path = current_path_;
    temporary_path.pop_back(); //erase last symbol (splitter)
    decltype(temporary_path.rbegin()) reverse_path_iter; //delete lower directory name
    while((*(reverse_path_iter = temporary_path.rbegin()) != osdependent::kDirectorySplitter)
            && (temporary_path.size() - 1) ) {
      temporary_path.pop_back();
    }
    if(IsPathExist(temporary_path)) {
      current_path_ = temporary_path;
      sprintf (answer, "%d", standards::OK);
      return client_->SendMessage(answer);
    } else {
      sprintf (answer, "%d", standards::ERR);
      return client_->SendMessage(answer);
    }
  } else {
    std::string temporary_path = current_path_ + change_path;
    if(IsPathExist(temporary_path)) {
      current_path_ += change_path + osdependent::kDirectorySplitter;
      sprintf (answer, "%d", standards::OK);
      return client_->SendMessage(answer);
    } else {
      sprintf (answer, "%d", standards::ERR);
      return client_->SendMessage(answer);;
    }
  }
}

bool ClientCommandsHandler::GetFile(standards::Packet request) {
  std::string file_name(request + standards::kMarkerSize);
  std::string file_path = current_path_ + osdependent::kDirectorySplitter + file_name;
  standards::Packet file_info;
  std::ifstream file(file_path, std::ios::binary); //getting size of file
  if(!file.good()) { //if file doesn't exist
    sprintf (file_info, "%d", standards::ERR);
    file.close();
    return client_->SendMessage(file_info);
  }
  file.seekg(0, std::ios::end);
  std::size_t file_size = file.tellg();
  file.close();
  sprintf (file_info + standards::kMarkerSize, "%d", file_size);
  std::ifstream file_stream (file_path, std::ios::in | std::ios::binary);
  if(file_stream.is_open()) {
    sprintf (file_info, "%d", standards::OK);
    if(!(client_->SendMessage(file_info))) {
        return false;
    }
  } else {
    sprintf (file_info, "%d", standards::ERR);
    return client_->SendMessage(file_info);
  }
  standards::Packet is_client_ready;
  if(!client_->ReceiveMessage(is_client_ready)) {
    return false;
  }
  int marker = atoi(is_client_ready);
  if(marker != standards::OK) {
    return true;
  }
  std::size_t already_sent = 0, bytes_to_read = 0;
  standards::Packet buffer;
  while(already_sent < file_size) {
    if(file_size - already_sent >= standards::kPacketSize) {
      bytes_to_read = standards::kPacketSize;
    } else {
      bytes_to_read = file_size - already_sent;
    }
    file_stream.read(buffer, bytes_to_read);
    if(! client_->SendMessage(buffer)) {
      file_stream.close();
      return false;
    }
    already_sent += bytes_to_read;
  }
  file.close();
  return true;
}

#ifdef _WIN32 //these two functions are absolutely different, so I used ifdef outside
std::string ClientCommandsHandler::FilesList(std::string directory) {
  directory += "*.*";
  HANDLE files_search = INVALID_HANDLE_VALUE;
  WIN32_FIND_DATA data;
  files_search = FindFirstFile((LPSTR)directory.c_str(), &data); // create HANDLE to manage files searching
  std::string list_of_files;
  if(files_search == INVALID_HANDLE_VALUE) {
    return list_of_files;
  }
  do {
    list_of_files += static_cast<std::string>(data.cFileName) + '\n';
  } while (FindNextFile(files_search, &data));
  FindClose(files_search);
  return list_of_files;
}

bool ClientCommandsHandler::IsPathExist(std::string directory) {
  DWORD result = GetFileAttributes((LPSTR)directory.c_str());
  return (result != INVALID_FILE_ATTRIBUTES && (result & FILE_ATTRIBUTE_DIRECTORY));
}

#else    //for Linux
std::string ClientCommandsHandler::FilesList(std::string directory) {
  directory += ".";
  DIR *directory_object;
  dirent *directory_manager;
  std::string list_of_files;
  if((directory_object = opendir(directory.c_str())) == nullptr) {
    return list_of_files;
  }
  while ((directory_manager = readdir (directory_object)) != nullptr) {
    list_of_files += static_cast<std::string>(directory_manager->d_name) + '\n';
  }
  closedir (directory_object);
  return list_of_files;
}

bool ClientCommandsHandler::IsPathExist(std::string directory) {
  DIR *directory_object = opendir(directory.c_str());
  if(directory_object) {
    closedir (directory_object);
    return true;
  } else {
    closedir (directory_object);
    return false;
  }
}
#endif
}
