#include "row_event_reader_factory.h"

namespace event_router {
// TODO: GetReader should be synchronized.
RowEventReader* RowEventReaderFactory::GetReader(const DbInfo& info, bool event_check_sum) {
  RowEventReader* event_reader = NULL;
  ROW_EVENT_READER_ITEM it = event_readers_.find(info);
  if (it != event_readers_.end()) {
    event_reader = it->second;
  } else {
    event_reader = new RowEventReader(info, slave_id_, event_check_sum);
    event_readers_[info] = event_reader;
  }
  return event_reader;
}

RowEventReader* RowEventReaderFactory::GetReader(const DbInfo& info, const std::string& binlog_file_name,
  uint32_t position, bool event_check_sum) {
  RowEventReader* event_reader = NULL;
  ROW_EVENT_READER_ITEM it = event_readers_.find(info);
  if (it != event_readers_.end()) {
    event_reader = it->second;
    event_reader->RegistNewRequest(binlog_file_name, position, true);
  } else {
    event_reader = new RowEventReader(info, slave_id_, binlog_file_name, position, event_check_sum);
    event_readers_[info] = event_reader;
  }
  return event_reader;
}

RowEventReaderFactory::~RowEventReaderFactory() {
  ROW_EVENT_READER_ITEM it;
  for (it = event_readers_.begin(); it != event_readers_.end(); it++) {
    delete it->second;
  }
}
}