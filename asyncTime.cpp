#include <boost/asio.hpp>
#include <boost/asio/detail/handler_work.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/io_service.hpp>
#include <ctime>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

#if BOOST_VERSION >= 107000
#define GET_IO_SERVICE(s) \
  ((boost::asio::io_context &)(s).get_executor().context())
#else
#define GET_IO_SERVICE(s) ((s).get_io_service())
#endif

using boost::asio::ip::tcp;

std::string make_daytime_string() {
  using namespace std;  // For time_t, time and ctime;
  auto now = time(nullptr);
  return ctime(&now);
}

class tcp_connection : public std::enable_shared_from_this<tcp_connection> {
 public:
  using pointer = std::shared_ptr<tcp_connection>;

  static pointer create(boost::asio::io_service &io_service) {
    // std::make_shared
    return pointer(new tcp_connection(io_service));
    // return std::make_shared<tcp_connection>(io_service);
  }

  tcp::socket &socket() { return _socket; }

  void start() {
    _message = make_daytime_string();

    auto self = shared_from_this();
    boost::asio::async_write(
        _socket, boost::asio::buffer(_message),
        [self = std::move(self)](auto error, auto bytes_transferred) {
          self->handle_write(error, bytes_transferred);
        });
  }

 private:
  tcp_connection(boost::asio::io_service &io_service) : _socket(io_service) {}

  void handle_write(const boost::system::error_code & /*error*/,
                    size_t /*bytes_transferred*/) {}

  tcp::socket _socket;
  std::string _message;
};

class tcp_server {
 public:
  tcp_server(boost::asio::io_service &io_service)
      : _acceptor(io_service, tcp::endpoint(tcp::v4(), 13)) {
    start_accept();
  }

 private:
  void start_accept() {
    std::shared_ptr<tcp_connection> new_connection = tcp_connection::create(
        ((boost::asio::io_service &)((_acceptor).get_executor().context())));
    _acceptor.async_accept(new_connection->socket(),
                           [this, new_connection](auto error) {
                             this->handle_accept(new_connection, error);
                           });
  }

  void handle_accept(tcp_connection::pointer new_connection,
                     const boost::system::error_code &error) {
    if (!error) {
      new_connection->start();
    }

    start_accept();
  }

  tcp::acceptor _acceptor;
};

int main() {
  try {
    boost::asio::io_service io_service;
    tcp_server server(io_service);
    io_service.run();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
