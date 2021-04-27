#include "response.h"
#include <fstream>

const std::string NONE = "\0";
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

Response::Response(Socket* conn,
                   std::vector<std::string> request,
                   std::map<std::string, std::string> header)
{
  this->conn = conn;
  this->request = request;
  this->header = header;

  message = get_file();
  mime = get_mime();
}

void Response::valid()
{
  if (message == NONE) {
    status_text = "Not Found";
    status = 404;
    message = "404 Not Found";
  } else if (mime == NONE) {
    status_text = "Unsupported Media Type";
    status = 415;
    message = "415 Unsupported Media Type";
  } else {
    status_text = "OK";
    status = 200;
  }
}

void Response::send()
{
  conn->SendLine("HTTP/1.1 " + std::to_string(status) + " " + status_text);
  conn->SendLine("Content-Type: " + mime);
  conn->SendLine("Content-Length: " + std::to_string(message.size()));
  conn->SendLine("");
  conn->SendLine(message);

  conn->Close();
}

void Response::redirect(std::string path_to)
{
  conn->SendLine("HTTP/1.1 301 Moved Permanently");
  conn->SendLine("Location: " + path_to);
  conn->SendLine("");

  conn->Close();
}

std::string Response::get_file()
{
  std::string contents = "";
  std::string path_to = HOME_DIR + header["path"];

  std::ifstream file(path_to, std::ios::binary);

  if (!file.is_open()) {
    return "\0";
  }

  while (file) {
    contents += file.get();
  }

  file.close();

  // Reading data from file this way leaves unwanted character (EOF) at the end
  // So it has to be removed
  // Otherwise it would crash some data
  // For example javascript code
  if (contents.length() > 0) {
    contents.pop_back();
  }

  return contents;
}

std::string Response::get_mime()
{
  size_t pos = header["path"].find_last_of('.');
  std::string extension = header["path"].substr(pos + 1, header["path"].length() - pos);

  if (MIME.find(extension) == MIME.end()) {
    return "\0";
  } else {
    return MIME[extension];
  }
}
