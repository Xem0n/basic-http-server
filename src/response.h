#ifndef RESPONSE_H
#define RESPONSE_H

#include "libs/Socket.h"
#include <string>
#include <vector>
#include <map>

class Response
{
  public:
    int status;
    std::string status_text;
    std::string message;
    std::string mime;

    Response(Socket* conn,
             std::vector<std::string> message,
             std::map<std::string, std::string> header);

    void valid();
    void send();
    void redirect(std::string path_to);

  private:
    Socket* conn;

    std::vector<std::string> request;
    std::map<std::string, std::string> header;

    std::string get_file();
    std::string get_mime();
};

#endif
