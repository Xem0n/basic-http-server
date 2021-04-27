#include "connection.h"
#include <iostream>

const std::string ENDLINE = "\r\n";

std::vector<std::string> split_string(std::string split_string, std::string separator)
{
  std::vector<std::string> strings;

  while (true) {
    std::string::size_type pos = split_string.find(separator);

    if (pos == std::string::npos) {
      // In case there's no more seperators in string, escape the whole string 
      if (split_string.length() > 0) {
        strings.push_back(split_string);
      }

      break;
    }

    strings.push_back(split_string.substr(0, pos));
    split_string.erase(0, pos + separator.length());
  }

  return strings;
}

Connection::Connection(Socket* conn)
{
  this->conn = conn;
}

// Main wrapper function
void Connection::run()
{
  start_time = std::chrono::system_clock::now();

  message = get_message();
  header = get_header();

  if (check_request()) {
    prepare_response();

    print_logs();
  } else {
    fallback();
  }
}

// Checks whether request can be handled by the server
// If not, send a proper fallback response
bool Connection::check_request()
{
  return is_http() &&
         is_path_correct();
}

void Connection::prepare_response()
{
  Response response(conn, message, header);
  response.valid();
  response.send();
}

void Connection::fallback()
{
  Response response(conn, message, header);
  response.status = 403;
  response.status_text = "Forbidden";
  response.message = "403 Forbidden";
  response.send();
}

std::vector<std::string> Connection::get_message()
{
  std::vector<std::string> message;
  std::string line = conn->ReceiveLine();

  // HTTP request always ends with an empty line
  while (line != ENDLINE) {
    message.push_back(line);
    line = conn->ReceiveLine();
  }

  return message;
}

std::map<std::string, std::string> Connection::get_header()
{
  std::vector<std::string> split_message = split_string(message[0], " ");
  
  std::map<std::string, std::string> header = {
    {"method", split_message[0]},
    {"path", split_message[1]},
    {"version", split_message[2]}
  };

  return header;
}

bool Connection::is_http()
{
  if (header["version"] == (std::string)"HTTP/1.1" + ENDLINE) {
    return true;
  } else {
    return false;
  }
}

bool Connection::is_path_correct()
{
  // Check for vulnerabilities
  if (header["path"].find("..") != std::string::npos ||
      header["path"].find("%252E%252E%252F") != std::string::npos ||
      header["path"][0] != '/') {

    return false;
  }

  // Add index.html to path if file is not specified
  if (header["path"].back() == '/') {
    header["path"] += "index.html";
  }

  return true;
}

void Connection::print_logs()
{
  auto cur_time = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = cur_time - start_time;

  std::cout << "time: " << diff.count() << "s" << std::endl;
  std::cout << "path: " << header["path"] << std::endl;
  std::cout << "method: " << header["method"] << std::endl;
}
