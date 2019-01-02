#ifndef INDEXER_H
#define INDEXER_H

#include <vector>
using std::vector;

#include <string>
using std::string;

#include <map>
using std::map;

#include "tokenizer.hpp"
#include "parser.hpp"

using std::size_t;

struct InverseList {
    // 存放一个词在某个文档中的相关信息
public:
    // 因为有const成员，所以需要一个构造函数
    InverseList(const string path):
        document_path(path), tf(0), positions(), next(nullptr) { }
    ~InverseList() {
        // 链表部分交给 header 来析构
        cout<<"[deconstruct] InverseList"<<endl;
    }
    const string document_path;
    size_t tf; // 文档中的词频
    vector<uint32_t> positions;
    InverseList* next; // 链表指针
};

struct InverseListHeader {
    InverseListHeader():df(0), total_tf(0), list(nullptr) {}
    ~InverseListHeader() {
        cout<<"[deconstruct] InverseListHeader"<<endl;
        InverseList* p;
        while(list->next) {
            p = list->next;
            list->next = list->next->next;
            delete p;
        }
        delete list;
    }
    uint32_t df; // 出现的文档数目
    uint32_t total_tf; //所有文档中的词频
    InverseList* list; // 倒排列表主体
};




class IndexConstructor {
    public:
        IndexConstructor() = default;
        bool addDocument(BaseParser&);
        using InverseIndex = map<string, InverseListHeader*>;
        void searchWord(const string&) const;
        InverseList* getQueryResult(const string&);
        ~IndexConstructor() {
            cout<<"[deconstruct] IndexConstructor"<<endl;
            for(auto it=iindex.begin(); it!=iindex.end(); ++it){
                delete it->second;
            }
            iindex.clear();
        }
    private:
        bool addWord2InverseList(const string&, const string&, uint32_t);
        static JiebaTokenizer tokenizer;
        InverseIndex iindex;
};


#endif