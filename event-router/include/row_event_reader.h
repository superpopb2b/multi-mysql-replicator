#ifndef _ROW_EVENT_READER_
#define _ROW_EVENT_READER_
#include <stdint.h>
#include <map>
#include <string>
#include <vector>
#include <tcp_driver.h>

#include "db_info.h"

using mysql::system::Binary_log_driver;
using mysql::system::Binlog_tcp_driver;

typedef std::map<uint64_t, mysql::Table_map_event*>::iterator TABLE_MAP_ITEM;

namespace event_router {

struct RowEventContent {
  enum OperationType {
    INSERT = 1, UPDATE = 2, DELETE = 3
  };
  OperationType type;
  std::string db_name;
  std::string table_name;
  std::vector<std::vector<std::string> > content;
};

class RowEventReader {
  friend class RowEventReaderFactory;

public:
  void Plug();

  void Unplug();

  bool RegistNewRequest(std::string file_name, uint32_t position, bool force = false);
  /**
   *  RowEventContent should be released by caller
   */
  RowEventContent* BlockGetEvent();

private:
  const DbInfo db_;
  Binary_log_driver *binlog_driver_;

  uint32_t binlog_position_;
  std::string binlog_file_name_;

  bool plugged_;

  std::map<uint64_t, mysql::Table_map_event*> tables;

  RowEventReader(const DbInfo& db, int slaveId, bool event_check_sum = false);

  RowEventReader(const DbInfo& db, int slaveId, const std::string binlog_file_name, const uint32_t offset, 
    bool event_check_sum = false);

  ~RowEventReader();

};

}
#endif