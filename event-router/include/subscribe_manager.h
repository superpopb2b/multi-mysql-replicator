#include <boost/asio.hpp>
#include <map>
using boost::asio::ip;
using boost::asio::io_service;
using boost::shared_ptr;
namespace event_router {
class SubscribeManager {
public:
  SubscribeManager(io_service& ios);
  void Start();
private:
  io_service& ios_;
  ip::tcp::acceptor acceptor_;
  std::map<TableInfo, ip::tcp::socket&> subscrible_table_;

  void AddSubscriber(const boost::system& error, shared_ptr<ip::tcp::socket> sock);
};

SubscribeManager::SubscribeManager(io_service& ios) :
    ios_(ios), acceptor_(ios_, ip::tcp::endpoint(ip::tcp::v4(), 9100)) {}

void SubscribeManager::Start() {
  shared_ptr<ip::tcp::socket> sock(new ip::tcp::socket(ios_));
  acceptor_.async_accept(*sock, 
      boost::bind(&SubscribeManager::AddSubscriber, this, placeholders::error, sock));
}

void SubscribeManager::AddSubscriber(const boost::system& error, shared_ptr<ip::tcp::socket> sock) {

}
}