#include "libs/Socket.h"
#include "connection.h"
#include <iostream>
#include <cstdlib>
#include <thread>

int main(int argc, char** argv)
{
  if (argc < 2) {
    std::cout << "First argument: Port";

    return 1;
  }

  int port = atoi(argv[1]);

  SocketServer server(port, 5);
  std::cout << "Server started...\n";

  while (true) {
    Socket* connection_socket = server.Accept();
    std::cout << "New connection...\n";

    // Make new object and thread for every connection, so server can handle many clients at a time
    Connection connection(connection_socket);
    std::thread connection_thread(&Connection::run, connection);
    connection_thread.detach();
  }

  return 0;
}
