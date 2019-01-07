#include "parser.hpp"

bool BaseParser::open(const string& path) {
    if(ifs){
        if(ifs.is_open()) {
            cout<<"open error：当前文件没有读完"<<endl;
            return false;
        }
        else {
            ifs.open(path);
            filepath = path;
            return ifs.good();
        }
    }
    else {
        ifs.close();
        ifs.clear();
        ifs.open(path);
        filepath = path;
        return ifs.good();
    }
}
    

bool DocParser::getContent(string& line) {
    if(ifs){ // same as ifs.good()
        if(ifs.is_open()) {
            std::getline(ifs, line);
            return ifs.good();
        }
        // 到达这里意味着没有open文件或者ifs已经关闭
        return false;
    }
    else 
        // 可能是文件到达末尾文件打开失败（文件不存在）
        return false;
}