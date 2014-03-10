#ifndef _ROW_EVENT_READER_FACTORY_
#define _ROW_EVENT_READER_FACTORY_

#include <map>
#include "db_info.h"
#include "row_event_reader.h"
namespace event_router {
class RowEventReader;
typedef std::map<DbInfo, RowEventReader*>::iterator ROW_EVENT_READER_ITEM;
class RowEventReaderFactory {
public:
  RowEventReaderFactory(int slave_id) : slave_id_(slave_id) {}

  RowEventReader* GetReader(const DbInfo& info, bool event_check_sum = false);

  RowEventReader* GetReader(const DbInfo& info, const std::string& binlog_file_name,
    uint32_t position, bool event_check_sum = false);

  ~RowEventReaderFactory();
private:
  const uint64_t slave_id_;
  std::map<DbInfo, RowEventReader*> event_readers_;
};
}

#endif