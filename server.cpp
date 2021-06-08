#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <ctime>
#include <exception>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;

std::string make_daytime_string() {
  auto now = std::time(nullptr);
  return std::ctime(&now);
}

int main() {
  try {
    boost::asio::io_service io;
    tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 13));
    std::cout << "start service\n";
    while (1) {
      tcp::socket socket(io);
      acceptor.accept(socket);
      std::string message = make_daytime_string();
      boost::system::error_code ignored_error;
      boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
    }
  }
  __catch(std::exception & e) { std::cerr << e.what() << std::endl; }
  std::cout << "Bye ....\n";
  return 0;
}