#include <iostream>
using std::cout;
using std::endl;

#include <string>
using std::string;

#include "database.hpp"

unsigned MysqlConn::connect {
    if(mysql_real_connect(&conn,
                            host,
                            user,
                            pwd,
                            db,
                            0,NULL,CLIENT_FOUND_ROWS))
        state = 0;
    else
        state = 1;
    return state;
}

STATE MysqlConn::getWordId(const string& word, uint32_t& id) {
    string sql{"select wordId from words where word = '"+word+"'"};
    mysql_query(&conn, sql);
    res = mysql_store_result(&mysql);
}


STATE MysqlConn::getDocumentId(const string&, uint32_t&)
STATE MysqlConn::getWordById(const uint32_t&, string&)
STATE MysqlConn::getDocumentById(const uint32_t&, string&)
STATE MysqlConn::putNewDoc(const string&, uint32_t&)
STATE MysqlConn::putNewWord(const string&, uint32_t&)

