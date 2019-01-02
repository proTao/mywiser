#ifndef _PARSER_HPP
#define _PARSER_HPP

#include <fstream>
using std::ifstream;

#include <string>
using std::string;

#include <iostream>
using std::cout;
using std::endl;
using std::ios;

class BaseParser {
public:
    BaseParser() = default;
    BaseParser(const string& path):
        ifs(path, ios::in), filepath(path) { } // 不会创建文件
    ~BaseParser(){
        if(ifs.is_open())
            ifs.close();
        ifs.clear();
    }
    virtual bool getContent(string&) = 0;
    bool open(const string&);
    const string getFilepath() const {
        return filepath;
    }
protected:
    ifstream ifs;
    string filepath;
};

class DocParser : public BaseParser {
public:
    DocParser():
        BaseParser() { }
    DocParser(const string& path):
        BaseParser(path) { }
    bool getContent(string&) override;
};

#endif