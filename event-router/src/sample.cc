//#include <stdint.h>
#include <iostream>
#include <map>
#include <string>
#include <gflags/gflags.h>
#include "row_event_reader_factory.h"
DEFINE_string(mysql_host, "127.0.0.1", "Mysql master host name.");
DEFINE_int32(mysql_port, 3306, "Mysql port");
DEFINE_string(username, "root", "Mysql username");
DEFINE_string(password, "root", "Mysql password");
DEFINE_int32(master_id, 123, "Master id");
DEFINE_int32(slave_id, 1234, "Slave id");
DEFINE_bool(event_checksum, true, "If v5.5 and before should set to false, else depends.");
DEFINE_bool(only_self_mode, true, "Test connecting single db and not connecting zookeeper if true");

void TestMode();

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  if (FLAGS_only_self_mode) {
    TestMode();
    return 0;
  }
  return 0;
}

void TestMode () {
  using namespace event_router;
  RowEventReaderFactory factory(FLAGS_slave_id);
  DbInfo info(FLAGS_username, FLAGS_password, FLAGS_mysql_host, FLAGS_mysql_port, FLAGS_master_id);
  RowEventReader* event_reader = factory.GetReader(info, true);
  event_reader->Plug();
  std::map<int, std::string> meaning;
  meaning[1] = "INSERT";
  meaning[2] = "UPDATE";
  meaning[3] = "DELETE";
  for (int i = 0; i < 2; i++) {
    RowEventContent* row_event = event_reader->BlockGetEvent();
    if (row_event) {
      std::cout << meaning[row_event->type] << ' ';
      std::cout << row_event->db_name << ':' << row_event->table_name << std::endl;
      int l;
      for (l = 0; l < row_event->content.size(); l++) {
        for (int m = 0; m < row_event->content[l].size(); m++) 
          std::cout << row_event->content[l][m] << '\t';
        std::cout << std::endl;
      }
      delete row_event;
    }
  }
  event_reader->Unplug();
}

