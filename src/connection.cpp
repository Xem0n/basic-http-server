#include "connection.h"
#include <iostream>
#include <fstream>

const std::string ENDLINE = "\r\n";
const std::string HOME_DIR = "public_html";

// Usage:
// MIME[extension]
std::map<std::string, std::string> MIME = {
  {"txt", "text/plain;charset=utf-8"},
  {"html", "text/html;charset=utf-8"},
  {"css", "text/css;charset=utf-8"},
  {"js", "test/javascript;charset=utf-8"},
  {"ico", "image/x-icon"}
};

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

  if (!check_request()) return;

  prepare_response();

  print_logs();
}

// Checks whether request can be handled by the server
// If not, send a proper fallback response
bool Connection::check_request()
{
  return get_data() &&
         get_header() &&
         is_http() &&
         is_path_correct();
}

void Connection::prepare_response()
{
  std::string data = read_file();
  std::string mime = get_mime();

  // Both data and mime return '\0' when request is invalid
  // Or can't be handled by server
  if (data == "\0" || mime == "\0") return;

  send_response(data, 200, "OK", mime);
}

bool Connection::get_data()
{
  std::vector<std::string> data;
  std::string line = conn->ReceiveLine();

  // HTTP request always ends with an empty line
  while (line != ENDLINE) {
    data.push_back(line);
    line = conn->ReceiveLine();
  }

  if (data.size() < 2) {
    send_response("400 Bad Request", 400, "Bad Request");

    return false;
  }

  this->data = data;

  return true;
}

bool Connection::get_header()
{
  std::vector<std::string> split_data = split_string(data[0], " ");

  if (split_data.size() != 3) {
    send_response("400 Bad Request", 400, "Bad Request");

    return false;
  }
  
  header = {
    {"method", split_data[0]},
    {"path", split_data[1]},
    {"version", split_data[2]}
  };

  return true;
}

bool Connection::is_http()
{
  if (header["version"] == (std::string)"HTTP/1.1" + ENDLINE) {
    return true;
  } else {
    send_response("400 Bad Request", 400, "Bad Request");

    return false;
  }
}

bool Connection::is_path_correct()
{
  // Check for vulnerabilities
  if (header["path"].find("..") != std::string::npos ||
      header["path"].find("%252E%252E%252F") != std::string::npos ||
      header["path"][0] != '/') {
    this->send_response("403 Forbidden", 403, "Forbidden");

    return false;
  }

  // Add index.html to path if file is not specified
  if (header["path"].back() == '/') {
    header["path"] += "index.html";
  }

  return true;
}

// Read file (from header)
std::string Connection::read_file()
{
  std::string contents = "";
  std::string path_to = HOME_DIR + header["path"];

  std::ifstream file(path_to, std::ios::binary);

  if (!file.is_open()) {
    send_response("404 Not Found", 404, "Not Found");

    return "\0";
  }

  while (file) {
    contents += file.get();
  }

  file.close();

  // Reading data from file this way leaves unwanted character at the end
  // So it has to be removed
  // Otherwise it would crash some data
  // For example javascript
  if (contents.length() > 0) {
    contents.pop_back();
  }

  return contents;
}

std::string Connection::get_mime()
{
  size_t pos = header["path"].find_last_of('.');
  std::string extension = header["path"].substr(pos + 1, header["path"].length() - pos);

  if (MIME.find(extension) == MIME.end()) {
    send_response("403 Forbidden", 403, "Forbidden");

    return "\0";
  } else {
    return MIME[extension];
  }
}

void Connection::redirect(std::string path_to)
{
  conn->SendLine("HTTP/1.1 301 Moved Permanently");
  conn->SendLine("Location: " + path_to);
  conn->SendLine("");

  conn->Close();
}

void Connection::send_response(std::string data,
                               int status,
                               std::string status_text,
                               std::string mime)
{
  conn->SendLine("HTTP/1.1 " + std::to_string(status) + " " + status_text);
  conn->SendLine("Content-Type: " + mime);
  conn->SendLine("Content-Length: " + std::to_string(data.size()));
  conn->SendLine("");
  conn->SendLine(data);

  conn->Close();
}

void Connection::print_logs()
{
  auto cur_time = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = cur_time - start_time;

  std::cout << "time: " << diff.count() << "s" << std::endl;
  std::cout << "path: " << header["path"] << std::endl;
  std::cout << "method: " << header["method"] << std::endl;
}
