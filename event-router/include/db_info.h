#ifndef _DB_INFO_
#define _DB_INFO_
#include <stdint.h>
#include <string>
namespace event_router {
class DbInfo {
public:
  std::string user_name_;
  std::string password_;
  std::string ip_;
  uint16_t port_;
  int master_id_;
  //DbInfo() {}
  DbInfo(std::string user_name, std::string password, 
    std::string ip, uint16_t port, int master_id) : 
      user_name_(user_name), password_(password),
      ip_(ip), port_(port), master_id_(master_id) {}

  DbInfo(const DbInfo& info) :
      user_name_(info.user_name_), password_(info.password_),
      ip_(info.ip_), port_(info.port_), master_id_(info.master_id_) {}

  bool operator < (const DbInfo& info) const {
    return ip_ < info.ip_ || (ip_ == info.ip_ && port_ < info.port_);
  }
};
}
#endif
