#ifndef CONNECTION_H
#define CONNECTION_H

#include "libs/Socket.h"
#include "response.h"
#include <vector>
#include <map>
#include <string>
#include <chrono>

class Connection
{
  public:
    Connection(Socket* conn);

    void run();

  private:
    Socket* conn;

    std::vector<std::string> message;
    std::map<std::string, std::string> header;

    std::chrono::time_point<std::chrono::system_clock> start_time;

    bool check_request();
    void prepare_response();
    void fallback();

    std::vector<std::string> get_message();
    std::map<std::string, std::string> get_header();

    bool is_http();
    bool is_path_correct();

    void print_logs();
};

#endif
