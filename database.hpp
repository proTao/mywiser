#include <mysql.h>

class DBConn
{
  public:
    using STATE = unsigned;
    DBConn() = default;
    

    virtual STATE connect(host, user, pwd, db) = 0;
    virtual STATE getWordId(const string&, uint32_t&) = 0;
    virtual STATE getDocumentId(const string&, uint32_t&) = 0;
    virtual STATE getWordById(const uint32_t&, string&) = 0;
    virtual STATE getDocumentById(const uint32_t&, string&) = 0;
    virtual STATE putNewDoc(const string&, uint32_t&) = 0;
    virtual STATE putNewWord(const string&, uint32_t&) = 0;

  protected:
    STATE state;
};

class MysqlConn {
    MysqlConn() {
        mysql_init(&conn);
    }
    ~MysqlConn()
    {
        mysql_commit(&conn);
        mysql_close(&conn);
    }
    virtual STATE connect(host, user, pwd, db) override;
    virtual STATE getWordId(const string&, uint32_t&) override;
    virtual STATE getDocumentId(const string&, uint32_t&) override;
    virtual STATE getWordById(const uint32_t&, string&) override;
    virtual STATE getDocumentById(const uint32_t&, string&) override;
    virtual STATE putNewDoc(const string&, uint32_t&) override;
    virtual STATE putNewWord(const string&, uint32_t&) override;
private:
    MYSQL conn;
    MYSQL_RES* res;
};