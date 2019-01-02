#include "indexer.hpp"
#include "parser.hpp"
#include <iostream>
#include <codecvt>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::cin;
using std::string;
using std::vector;



int main() {
    string docpath, word;
    IndexConstructor indexer;
    DocParser parser;

    cout<<"输入要扫描的文档: "<<endl;
    cin>>docpath;
    parser.open(docpath);
    indexer.addDocument(parser);

    cout<<"输入要扫描的文档2: "<<endl;
    cin>>docpath;
    parser.open(docpath);
    indexer.addDocument(parser);

    cout<<"输入要扫描的文档3: "<<endl;
    cin>>docpath;
    parser.open(docpath);
    indexer.addDocument(parser);

    cout<<"要查询的单词"<<endl;
    cin>>word;
    indexer.searchWord(word);

    return 0;
}