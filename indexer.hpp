#ifndef __INDEXER_H
#define __INDEXER_H

#include <vector>
using std::vector;

#include <string>
using std::string;

#include <map>
using std::map;

#include <memory>
using std::make_shared;
using std::shared_ptr;

#include "tokenizer.hpp"
#include "parser.hpp"
#include "database.hpp"

using std::size_t;

struct InverseList
{
    // 存放一个词在某个文档中的相关信息
  public:
    // 因为有const成员，所以需要构造函数
    InverseList(): documentId(), tf(0), positions(), next(nullptr) {}
    InverseList(const uint32_t& id, const vector<uint32_t>& pos):
         documentId(id), tf(pos.size()), positions(pos), next(nullptr) {}
    InverseList(const uint32_t& id) : documentId(id), tf(0), positions(), next(nullptr) {}
    InverseList(const InverseList &old) : documentId(old.documentId), tf(old.tf),
                                          positions(old.positions), next(nullptr) {}
    ~InverseList()
    {
        // 链表部分交给 header 来析构
        // cout<<"[deconstruct] InverseList"<<documentId <<endl;
    }
    const uint32_t documentId;
    size_t tf; // 文档中的词频
    vector<uint32_t> positions;
    InverseList *next; // 链表指针
};

struct InverseListHeader : InverseList
{
    InverseListHeader() : df(0), total_tf(0), InverseList() {}

    InverseListHeader(const InverseListHeader &old) : df(old.df), total_tf(old.total_tf), InverseList()
    {

        InverseList *p2 = old.next;
        if (p2)
        {
            // 复制第一个inverselist节点
            this->next = new InverseList(*p2);

            InverseList *p1 = this->next;
            while (p2->next)
            {
                p1->next = new InverseList(*(p2->next));
                p2 = p2->next;
                p1 = p1->next;
            }
        }
    }
    ~InverseListHeader()
    {
        // cout<<"[deconstruct] InverseListHeader"<<df<<endl;
        InverseList *p;

        while (next)
        {
            p = next->next;
            delete next;
            next = p;
        }
        next = nullptr;
    }
    uint32_t df;       // 出现的文档数目
    uint32_t total_tf; //所有文档中的词频
    // InverseList* next; // 倒排列表主体

    // InverseListHeader& mergeIntersection(const InverseListHeader&);
};

class IndexConstructor
{
  public:
    using InverseIndex = map<uint32_t, InverseListHeader *>;
    IndexConstructor() : iindex()
    {
        cout << "[ construct ] IndexConstructor" << endl;
    };
    bool addDocument(BaseParser &);
    void searchWordForTest(const string &) const;
    shared_ptr<InverseListHeader> recall(const string &);
    ~IndexConstructor()
    {
        cout << "[deconstruct] IndexConstructor" << endl;
        for (auto it = iindex.begin(); it != iindex.end(); ++it)
        {
            delete it->second;
        }
        iindex.clear();
    }

    static MysqlConn db;

  private:
    bool addWord2InverseList(const string &, const string &, uint32_t);

    static JiebaTokenizer tokenizer;
    InverseIndex iindex;
};

vector<uint32_t> search_phrase(const vector<const InverseList*> &, const vector<uint32_t> &);

#endif