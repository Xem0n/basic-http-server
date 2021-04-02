#ifndef CONNECTION_H
#define CONNECTION_H

#include "libs/Socket.h"
#include <vector>
#include <map>
#include <string>
#include <chrono>

class Connection {
  public:
    Connection(Socket* conn);

    void run();

  private:
    Socket* conn;

    std::vector<std::string> data;
    std::map<std::string, std::string> header;

    std::chrono::time_point<std::chrono::system_clock> start_time;

    bool check_request();
    void prepare_response();

    bool get_data();
    bool get_header();

    bool is_http();
    bool is_path_correct();

    std::string read_file();
    std::string get_mime();

    void redirect(std::string path_to);
    void send_response(std::string data = "",
                       int status = 200,
                       std::string status_text = "OK",
                       std::string mime = "text/plain;charset=utf-8");
    void print_logs();
};

#endif
