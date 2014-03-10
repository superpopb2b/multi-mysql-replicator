#include <string>
namespace event_router {
class SubscribeInfo {
public:
  std::string db_name_;
  std::string table_name_;

  SubscribeInfo(const std::string& db_name, const std::string& table_name) :
    db_name_(db_name), table_name_(table_name) {}

  SubscribeInfo(const SubscribeInfo& info) :
    db_name_(info.db_name_), table_name_(info.table_name_) {}

  bool operator < (const SubscribeInfo& info) {
    return db_name_ < info.db_name_ ||
      (db_name_ == info.db_name_ && table_name_ < info.table_name_);
  }
};
}