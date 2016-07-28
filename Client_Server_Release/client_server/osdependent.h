#ifndef OSDEPENDENT_H
#define OSDEPENDENT_H

#include "standards.h"
// This header contains all OS dependent code.
#ifdef _WIN32   // for Windows OS
#include <winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <process.h>
#include <direct.h> // for file system navigation

#define chdir _chdir
#define getcwd _getcwd

namespace osdependent {
const char kDirectorySplitter = '\\';
const std::string start_path = static_cast<std::string> (getcwd(nullptr, 0)) + kDirectorySplitter;
}


#else   // for Linux OS
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h> // for multithreading
#include <dirent.h>  //for directories navigation
#include <unistd.h>

#define closesocket close // for unification of socket closing
#define INVALID_SOCKET -1
#define SOCKET_ERROR   -1

namespace osdependent {
const char kDirectorySplitter = '/';
const std::string start_path = static_cast<std::string> (getcwd(nullptr, 0)) + kDirectorySplitter;
}
#endif
#endif // OSDEPENDENT_H
