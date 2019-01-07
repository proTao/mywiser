#include <iostream>
using std::cout;
using std::cin;
using std::endl;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include "database.hpp"

// 参考：https://blog.csdn.net/code_star_one/article/details/76522422

string DBConn::WORD_TABLE_NAME = "words";
string DBConn::DOC_TABLE_NAME = "documents";


void MysqlConn::freeResult(MYSQL_RES* result) {
    // insert 语句没有返回值，所以result一定是0x00
    // 执行完mysql_query之后一定要释放MYSQL_RES
    // 否则可能会报错：Commands out of sync; you can't run this command now
    mysql_free_result(result);
    while( !mysql_next_result(&conn)) {
        //cout<<"clear"<<endl;
        result = mysql_store_result(&conn);
        mysql_free_result(result);
    }
}

DBConn::STATE MysqlConn::connect (const string& host, 
                          const string& user, 
                          const string& pwd,
                          const string& db){
    if(mysql_real_connect(&conn, host.c_str(), user.c_str(), pwd.c_str(), db.c_str(),
                          0,NULL,CLIENT_FOUND_ROWS)) {
        state = 0;
        // 设置中文编码
        mysql_query(&conn, "SET NAMES UTF8");
    }
    else
        state = 1;
    return state;
}

DBConn::STATE MysqlConn::_selectOneByOne(const string& table,
                                         const string& queryfield,
                                         const string& queryval,
                                         const string& targetfield,
                                         string& targetval) {
    string sql{"select " + targetfield + " from " + table + \
               " where " + queryfield + " = '" + queryval + "';"};
    //cout<<"[execute sql] "<<sql<<endl;
    mysql_query(&conn, sql.c_str());
    const char* msg = mysql_error(&conn);
    MYSQL_RES* result = mysql_store_result(&conn);
    STATE rc;

    if(result && mysql_num_rows(result) == 1) {
        MYSQL_ROW row = mysql_fetch_row(result);
        targetval = row[0];
        rc = 0;
    }
    else {
        if(!result)
            cout<<"[error message]" << msg << endl;
        rc = 1;
    }

    freeResult(result);
    return rc;
}

DBConn::STATE MysqlConn::_insertOne(const string& table, 
                                    const string& field,
                                    const string& val) {
    string sql{"insert into " + table + " (" + field + \
               ") values ('" + val + "');"};
    // cout<<"[execute sql] "<<sql<<endl;
    mysql_query(&conn, sql.c_str());

    int row_count = mysql_affected_rows(&conn);
    const char* msg = mysql_error(&conn);

    MYSQL_RES* result = mysql_store_result(&conn);
    freeResult(result);

    if(row_count == 1){
        return 0;
    }
    // 要是row_count==-1说明sql执行失败
    cout<<"[execute fail] affect rows: "<<row_count<<endl;
    cout<<"[error message]"<< msg << endl;
    cout<<"[error sql]"<< sql << endl;
    return 1;
}

DBConn::STATE MysqlConn::_updateOneByOne(const string& table, 
                                         const string& wherefield, 
                                         const string& whereval,
                                         const string& newfield,
                                         const string& newval) {
    string sql{"update " + table + \
               " set " + newfield + "='" + newval + \
               "' where " + wherefield + "='" + whereval +"';"};
    //cout<<"[execute sql] "<<sql<<endl;
    mysql_query(&conn, sql.c_str());
    MYSQL_RES* result = mysql_store_result(&conn);

    int row_count = mysql_affected_rows(&conn);
    freeResult(result);
    if(row_count == 1){
        return 0;
    }
    return 1;

}

DBConn::STATE DBConn::getWordId(const string& word, uint32_t& id) {
    string id_str;
    //强调一点这里是动态绑定的虚函数，其他的类似。
    STATE rc = this->_selectOneByOne(DBConn::WORD_TABLE_NAME, 
                                "word", word, "wordId", id_str);
    if(rc == 0)
    // stoi 会做检查，如果超出int范围会报错
        id = std::stoi(id_str);
    return rc;
}

DBConn::STATE DBConn::getDocumentId(const string& document, uint32_t& id) {
    string id_str;
    STATE rc = _selectOneByOne(DBConn::DOC_TABLE_NAME, 
                                "documentPath", document, "documentId", id_str);
    if (rc == 0)
        id = std::stoi(id_str);
    return rc;
}

DBConn::STATE DBConn::getWordById(const uint32_t& id, string& word) {
    STATE rc = _selectOneByOne(DBConn::WORD_TABLE_NAME, 
                                "wordId", std::to_string(id), "word", word);
    return rc;
}

DBConn::STATE DBConn::getDocumentById(const uint32_t& id, string& path) {
    STATE rc = _selectOneByOne(DBConn::DOC_TABLE_NAME, 
                                "documentId", std::to_string(id), "documentPath", path);
    return rc;
}

DBConn::STATE DBConn::putNewDoc(const string& newdoc, uint32_t& newid) {
    STATE rc = _insertOne(DBConn::DOC_TABLE_NAME, "documentPath", newdoc);
    if(rc != 0)
        return rc;
    rc = getDocumentId(newdoc, newid);
    return rc;
}

DBConn::STATE DBConn::putNewWord(const string& newword, uint32_t& newid) {
    STATE rc = _insertOne(DBConn::WORD_TABLE_NAME, "word", newword);
    if(rc != 0)
        return rc;
    rc = getWordId(newword, newid);
    return rc;
}
