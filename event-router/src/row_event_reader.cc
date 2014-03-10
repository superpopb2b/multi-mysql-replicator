#include "row_event_reader.h"
#include <iostream>
#include <string>
#include <boost/lexical_cast.hpp>

#include <binlog_api.h>
using mysql::Error_code;
using mysql::Row_event;
using mysql::Table_map_event;
using mysql::Binary_log_event;

namespace event_router {
/**
 * Return true if request old event.
 */
static bool CompareBinaryFile(const std::string& oringin_name, const uint32_t oringin_position,
                        const std::string& new_name, const uint32_t new_position);

static RowEventContent* ParseRowEvent(Row_event* row_event, Table_map_event* table_map, RowEventContent::OperationType type);

void RowEventReader::Plug() {
  std::string master_file_name;
  uint64_t master_position;
  mysql::Error_code err = (mysql::Error_code)binlog_driver_->get_position(&master_file_name, &master_position);
    // TODO: Log err.
  if (err != mysql::ERR_OK)
    return;
  if (!plugged_ && binlog_driver_->connect() == 0)
    plugged_ = true;
}

void RowEventReader::Unplug() {
  if (plugged_)
    binlog_driver_->kill();
}

bool RowEventReader::RegistNewRequest(std::string file_name, uint32_t position, bool force) {
  if (CompareBinaryFile(binlog_file_name_, binlog_position_, file_name, position)) {
    if (force) {
      mysql::Error_code err = (mysql::Error_code)binlog_driver_->set_position(file_name, position);
      if (err != mysql::ERR_OK)
        return false;
    } else
    return false;
  }
  return true;
}

RowEventContent* RowEventReader::BlockGetEvent() {
  RowEventContent *event_content = NULL;
  Binary_log_event *event;
  while (!event_content) {
    if (binlog_driver_->wait_for_next_event(&event) != ERR_OK)
      return NULL;
      // TODO: Dump event meta
      // TODO: Log query message
    Log_event_type type = event->get_event_type();
    std::cout << "Event type: " << (int)type << std::endl;
    if (type == TABLE_MAP_EVENT) {
      Table_map_event *table_map_event = (Table_map_event*)event;

      TABLE_MAP_ITEM mit = tables.find(table_map_event->table_id);
      if (mit != tables.end()) {
        Table_map_event* obsolete = mit->second;
        delete obsolete;
      }
      tables[table_map_event->table_id] = table_map_event;
      continue;
    }

    if (type == mysql::WRITE_ROWS_EVENT || type == mysql::WRITE_ROWS_EVENTv2 || 
        type == mysql::UPDATE_ROWS_EVENT || type == mysql::UPDATE_ROWS_EVENTv2 ||
        type == mysql::DELETE_ROWS_EVENT || type == mysql::DELETE_ROWS_EVENTv2) {
      mysql::Row_event* row_event = (Row_event*) event;
      uint64_t table_id = row_event->table_id;
      TABLE_MAP_ITEM mit = tables.find(table_id);
      RowEventContent::OperationType operation_type;
      if (mit != tables.end()) {
        switch(type) {
          case mysql::WRITE_ROWS_EVENT:
          case mysql::WRITE_ROWS_EVENTv2:
            operation_type = RowEventContent::INSERT; break;
          case mysql::UPDATE_ROWS_EVENT:
          case mysql::UPDATE_ROWS_EVENTv2:
            operation_type = RowEventContent::UPDATE; break;
          case mysql::DELETE_ROWS_EVENT:
          case mysql::DELETE_ROWS_EVENTv2:
            operation_type = RowEventContent::DELETE; break;
          default: ;
        }
        event_content = ParseRowEvent(row_event, mit->second, operation_type);
      }
      else 
        event_content = NULL;
      delete row_event;
      continue;
    }
    delete event;
  }
  return event_content;
}

RowEventReader::RowEventReader(const DbInfo& db, int slaveId, bool event_check_sum) : db_(db), plugged_(false){
  binlog_driver_ = new mysql::system::Binlog_tcp_driver(db_.user_name_, db_.password_, 
    db_.ip_, db_.port_, db_.master_id_,
    slaveId, event_check_sum);
}

RowEventReader::RowEventReader(const DbInfo& db, int slaveId, const std::string binlog_file_name, const uint32_t offset, 
  bool event_check_sum) : db_(db), binlog_file_name_(binlog_file_name), binlog_position_(offset), plugged_(false){
  binlog_driver_ = new mysql::system::Binlog_tcp_driver(db_.user_name_, db_.password_, 
    db_.ip_, db_.port_, db_.master_id_, slaveId, event_check_sum,
    binlog_file_name, offset);
}

RowEventReader::~RowEventReader() {
  if (plugged_)
    Unplug();
  delete binlog_driver_;
}

static bool CompareBinaryFile(const std::string& oringin_name, const uint32_t oringin_position,
                        const std::string& new_name, const uint32_t new_position) {
      // TODO: Log error.
    std::size_t oringin_dot = oringin_name.find('.');
    if (oringin_dot == std::string::npos)
      return false;

    std::size_t new_dot = new_name.find('.');
    if (oringin_dot == std::string::npos)
      return false;

    if (oringin_name.substr(0, oringin_dot).compare(new_name.substr(0, new_dot))) {
      //TODO: Log different prefix.
      return false;
    }

    uint32_t oringin_index = boost::lexical_cast<uint32_t>(oringin_name.substr(new_dot + 1));
    uint32_t new_index = boost::lexical_cast<uint32_t>(new_name.substr(new_dot + 1));

    return new_index < oringin_index || 
        (new_index == oringin_index && new_position < oringin_position);
}

static RowEventContent* ParseRowEvent(Row_event* row_event, Table_map_event* table_map, RowEventContent::OperationType type) {
  RowEventContent* contents = new RowEventContent();
  contents->db_name = table_map->db_name;
  contents->table_name = table_map->table_name;
  contents->type = type;

  mysql::Row_event_set rows(row_event, table_map);
  mysql::Row_event_set::iterator it = rows.begin();
  mysql::Converter converter;
  do {
    mysql::Row_of_fields fields = *it;
    mysql::Row_of_fields::iterator fields_it = fields.begin();
    std::vector<std::string> row;
    do {
      std::string content;
      converter.to(content, *fields_it);
      row.push_back(content);
    } while (++fields_it != fields.end());
    contents->content.push_back(row);
  } while (++it != rows.end());
  return contents;
}
}