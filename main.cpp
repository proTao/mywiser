#include "indexer.hpp"
#include "utils.hpp"
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
    vector<cppjieba::Word> jiebawords;
    IndexConstructor indexer;
    indexer.db.initial("localhost","root","123456","mywiser");
    DocParser parser;

    // cout<<"输入要扫描的文档: "<<endl;
    // cin>>docpath;
    // parser.open(docpath);
    parser.open("temp1");
    indexer.addDocument(parser);

    // cout<<"输入要扫描的文档2: "<<endl;
    // cin>>docpath;
    // parser.open(docpath);
    parser.open("temp2");
    indexer.addDocument(parser);

    // cout<<"输入要扫描的文档3: "<<endl;
    // cin>>docpath;
    // parser.open(docpath);
    parser.open("temp3");
    indexer.addDocument(parser);

    cout<<"输入要查询的短语: "<<endl;
    cin>>word;
    auto header = indexer.getQueryResult(word);
    InverseList* p = header->next;
    while(p){
        string doc;
        indexer.db.getDocumentById(p->documentId, doc);
        cout<<doc<<endl;
        p = p->next;
    }

    // cout<<"输入要查询的单词: "<<endl;
    // header = indexer.getQueryResult(word);
    // InverseList* p = header->next;
    
    return 0;
}