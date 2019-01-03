#include <iostream>
#include <mysql.h>

using namespace std;

int main(int argc,char *argv[])
{
    MYSQL conn;
    int res;
    mysql_init(&conn);
    if(mysql_real_connect(&conn,"localhost","root","123456","mywiser",0,NULL,CLIENT_FOUND_ROWS)) //"root":数据库管理员 "":root密码 "test":数据库的名字
    {
        cout<<"connect success!\n"<<endl;
        res=mysql_query(&conn,"insert into documents (documentPath, docType) values ('user','123456');");
        if(res)
        {
         cout<<"error\n"<<endl;
        }
        else
        {
            cout<<"OK\n"<<endl;
        }
        mysql_close(&conn);
    }
    return 0;
}