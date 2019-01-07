#ifndef __DATABASE_H
#define __DATABASE_H

#include <mysql.h>

#include <iostream>
using std::cout;
using std::endl;


class DBConn
{
public:
    using STATE = unsigned;
    DBConn(){
        cout<<"[ construct ] DBConn"<<endl;
    };
    STATE initial(const string& host, const string& user,
           const string& pwd, const string& db) {
        // 动态绑定派生类的虚函数
        this->init();
        this->connect(host, user, pwd, db);
        return 0;
    }

    virtual ~DBConn() {
        cout<<"[deconstruct] DBConn"<<endl;
    };
    
    virtual STATE init() = 0;
    virtual STATE connect(const string&, const string&, 
                          const string&, const string&) = 0;

    
    STATE getWordId(const string&, uint32_t&);
    STATE getDocumentId(const string&, uint32_t&);
    STATE getWordById(const uint32_t&, string&);
    STATE getDocumentById(const uint32_t&, string&);
    STATE putNewDoc(const string&, uint32_t&);
    STATE putNewWord(const string&, uint32_t&);
    
protected:
    // make sure just one row will be return
    virtual STATE _selectOneByOne(const string&, const string&, const string&,
                                  const string&, string&) = 0;
    virtual STATE _insertOne(const string&, const string&, const string&) = 0;
    virtual STATE _updateOneByOne(const string&, const string&, const string&,
                                  const string&, const string&) = 0;
    // data member
    STATE state;
    static string WORD_TABLE_NAME;
    static string DOC_TABLE_NAME;
};

class MysqlConn : public DBConn{
public:
    MysqlConn() : DBConn() {
        // 执行基类的构造函数
        cout<<"[ construct ] MysqlConn"<<endl;
    }
    ~MysqlConn()
    {
        cout<<"[deconstruct] MysqlConn"<<endl;
        mysql_close(&conn);
    }
    virtual STATE init () override {
        if(mysql_init(&conn)) return 0;
        else return 1;
    }
    virtual STATE connect(const string&, 
                          const string&, 
                          const string&,
                          const string&) override;
    
private:
    virtual STATE _selectOneByOne(const string&, const string&, 
                                  const string&, const string&, 
                                  string&) override;
    virtual STATE _insertOne(const string&, const string&, 
                             const string&) override;
    virtual STATE _updateOneByOne(const string&, const string&, 
                                  const string&, const string&, 
                                  const string&) override;
    void freeResult(MYSQL_RES*);
    
    
    MYSQL conn;
};

#endif