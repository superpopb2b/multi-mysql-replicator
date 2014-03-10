#include "binlog_api.h"
#include "binlog_event.h"
/**
 *   @file basic-1
 *     @author Mats Kindahl <mats.kindahl@oracle.com>
 *
 *       This is a basic example that just opens a binary log either from a
 *         file or a server and print out what events are found.  It uses a
 *           simple event loop and checks information in the events using a
 *             switch.
 *              */

using mysql::Binary_log;
using mysql::system::create_transport;
using mysql::system::Binlog_tcp_driver;
using namespace mysql;
void dump_query_event(Query_event* ev) {
  using namespace std;
  //using namespace boost;
  cout << "Query event:" << endl;
  cout << "\tthread id: " << ev->thread_id << '\t';
  cout << "exec_time: " << ev->exec_time << '\t';
  cout << "error_code: " << ev->error_code << '\t';
  cout << "variables: ";
  vector<boost::uint8_t>::iterator it;
  for (it = ev->variables.begin(); it != ev->variables.end(); it++) {
    cout << (uint32_t)*it << '#';
  }
  cout << endl;
  cout << "\tdb_name: " << ev->db_name << '\t';
  cout << "query: " << ev->query << endl;
}

void dump_map_event(Table_map_event* ev) {
  using namespace std;
  cout << "Map event" << endl;
  cout << "\ttable_id: " << ev->table_id << '\t';
  cout << "flags: " << ev->flags << '\t';
  cout << "db_name: " << ev->db_name << '\t';
  cout << "table_name: " << ev->table_name << endl;
  cout << "\tcolumns: ";
  vector<boost::uint8_t>::iterator it;
  for (it = ev->columns.begin(); it != ev->columns.end(); it++) {
    cout << (uint32_t)*it << '#';
  }
  cout << endl << "\tmetadata: ";
  for (it = ev->metadata.begin(); it != ev->metadata.end(); it++) {
    cout << (uint32_t)*it << '#';
  }
  cout << endl << "\tnull_bits: ";
  for (it = ev->null_bits.begin(); it != ev->null_bits.end(); it++){
    cout << (uint32_t)*it << '#';
  }
  cout << endl;
}

void dump_xid_event(Xid* ev) {
  using namespace std;
  cout << "Xid event" << endl;
  cout << "\txid_id: " << ev->xid_id << endl;
}

void dump_rows_query_event(Rows_query_event* ev) {
  using namespace std;
  cout << "Rows query event" << endl;
  cout << "\tlength: " << ev->length;
  cout << "\tquery text: " << ev->query_text << endl;
}
int main(int argc, char** argv) {
  //Binary_log binlog(create_transport(argv[1]));
  Binary_log binlog(new Binlog_tcp_driver(std::string("root"), std::string("root"), std::string("127.0.0.1"), 3306, 123, 1, true));
  binlog.connect();

  Binary_log_event *event;

  Table_map_event *table_event = NULL;

  while (true) {
    int result = binlog.wait_for_next_event(&event);
    if (result == ERR_EOF)
      break;
    std::cout << "Found event of type "
      << event->get_event_type();
    std::cout << "[" << mysql::system::get_event_type_str(event->get_event_type()) <<"]" 
      << " at pos: " << event->header()->next_position
      << std::endl;

    Log_event_type type = event->get_event_type();
    if (type == QUERY_EVENT) {
      Query_event *query_event = (Query_event *)event;
      dump_query_event(query_event);
      delete query_event;
      continue;
    }

    if (type == TABLE_MAP_EVENT) {
      table_event = (Table_map_event *) event;
      dump_map_event(table_event);
      continue;
    }
    
    if (type == XID_EVENT) {
      Xid* xid_event = (Xid*) event;
      dump_xid_event(xid_event);
      if (table_event) {
        delete table_event;
        table_event = NULL;
      }
      delete xid_event;
      continue;
    }

    if (type == ROWS_QUERY_EVENT) {
      dump_rows_query_event((Rows_query_event*)event);
      continue;
    }
    if (type == WRITE_ROWS_EVENT || type == UPDATE_ROWS_EVENT || type == DELETE_ROWS_EVENT
      || type == WRITE_ROWS_EVENTv2 || type == UPDATE_ROWS_EVENTv2 || type == DELETE_ROWS_EVENTv2) {
      Row_event *row_event = (Row_event *) event;
      std::cout << "\tdb_name[" << table_event->db_name << "]\ttable_name:[" << table_event->table_name 
        << "]\ttable id:[" << row_event->table_id << "]\tflags: [" << row_event->flags
        << "]\tcolumns_len:[" << row_event->columns_len << "]\tnull_bits_len:["
        << row_event->null_bits_len << "]\n";
      
      mysql::Row_event_set rows(row_event, table_event);

      mysql::Row_event_set::iterator it= rows.begin();
      do {
        mysql::Row_of_fields fields = *it;
        mysql::Row_of_fields::iterator field_it= fields.begin();

        do {
          mysql::Converter converter;
          std::string key;
          converter.to(key, *field_it);

          std::cout << "\t\tColum change to: [" << key << "]\n";
        } while (++field_it != fields.end());
        std::cout << "===================" << std::endl;
      } while (++it != rows.end());

      /* Consume the event */
      delete row_event;
      continue;
    }

    //std::cout << "Others" << std::endl;

  }
  return 0;
}

