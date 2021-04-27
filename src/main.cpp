#include "libs/Socket.h"
#include "connection.h"
#include <iostream>
#include <cstdlib>
#include <thread>

const int CONNECTIONS = 5;

void accept_connection(SocketServer server)
{
  Socket* connection_socket = server.Accept();
  std::cout << "New connection...\n";

  // Make new object and thread for every connection, so server can handle many clients at a time
  Connection connection(connection_socket);
  std::thread connection_thread(&Connection::run, connection);
  connection_thread.detach();
}

void run_server(int port)
{
  SocketServer server(port, CONNECTIONS);
  std::cout << "Server started...\n";

  while (true) {
    accept_connection(server);
  }
}

int main(int argc, char** argv)
{
  if (argc < 2) {
    std::cout << "Too few arguments. No port passed.\n";

    return 1;
  }

  int port = atoi(argv[1]);

  run_server(port);
  
  return 0;
}
