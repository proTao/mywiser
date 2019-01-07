
#include <iostream>
using std::cout;
using std::endl;

#include <string>
using std::string;

#include <algorithm>
using std::sort;

#include "indexer.hpp"
#include "database.hpp"

using namespace std;

/*********** 静态成员初始化 ***********/
JiebaTokenizer 
IndexConstructor::tokenizer = JiebaTokenizer();

MysqlConn 
IndexConstructor::db = MysqlConn();
/*********** 静态成员初始化结束 ***********/


InverseListHeader& 
InverseListHeader::mergeIntersection(const InverseListHeader& rhs) {
   InverseList* p1 = next;
   InverseList* p2 = rhs.next;
   InverseList* pre = this;
   while(p1 && p2){
      if(p1->documentId == p2->documentId){
         p1 = p1->next;
         p2 = p2->next;
         pre = pre->next;
      }
      else if (p1->documentId < p2->documentId){
         while(p1 && p1->documentId < p2->documentId){
            pre->next = p1->next;
            delete p1;
            p1 = pre->next;
         }
      }
      else { // p1->documentId > p2->documentId
         while(p2 && p1->documentId > p2->documentId){
            p2 = p2->next;
         }
      }
   }
   return *this;
}


bool IndexConstructor::addDocument(BaseParser& parser) {
   string line;
   vector<cppjieba::Word> jiebawords;
   uint32_t document_offset{1};
   while(parser.getContent(line)) {
      IndexConstructor::tokenizer.tokenize(line, jiebawords, true);
      for(auto jiebaword: jiebawords) {
         addWord2InverseList(jiebaword.word, 
                             parser.getFilepath(),
                             jiebaword.offset+document_offset);
      }
      document_offset += line.size() + 1; // 加 1 是因为换行符
   }
   return true;
}

bool IndexConstructor::addWord2InverseList(const string& word, 
                                           const string& document,                                           uint32_t offset){
   uint32_t wordId, documentId;
   DBConn::STATE rc;
   if((rc=db.getWordId(word,wordId))) {
      db.putNewWord(word, wordId);
   }
   if((rc=db.getDocumentId(document, documentId))) {
      db.putNewDoc(document, documentId);
   }

   if(iindex.find(wordId) == iindex.end()) {
      // 当前词不存在
      iindex[wordId] = new InverseListHeader();
      iindex[wordId]->next = new InverseList(documentId);
      iindex[wordId]->df = 1;
   }
   
   // 此时词一定存在 iindex 中
   auto it = iindex.find(wordId);
   auto header = it->second;
   ++(header->total_tf);
   if(header->next->documentId != documentId) {
      auto temp = new InverseList(documentId);
      temp->positions.push_back(offset);
      temp->tf = 1;
      temp->next = header->next;
      header->next = temp;
      ++(header->df);
   }
   else{
      header->next->positions.push_back(offset);
      ++(header->next->tf);
   }
   return true;
}

shared_ptr<InverseListHeader> IndexConstructor::getQueryResult(const string& query) {
   vector<cppjieba::Word> queryWords, validQueryWords;
   this->tokenizer.tokenize(query, queryWords, false);
   // TODO
   // 只保留索引中存在的词,这里是通过数据库来查询，其实通过iindex查也可以
   copy_if(queryWords.begin(), queryWords.end(), back_inserter(validQueryWords), 
            [](const cppjieba::Word& jiebaword){
                  uint32_t temp;
                  return db.getWordId(jiebaword.word, temp) == 0;
               });
   // validQueryWords.shrink_to_fit();

   if(validQueryWords.size() == 0){
      return make_shared<InverseListHeader>();
   }
   else if (validQueryWords.size() == 1)
   {  
      uint32_t wordid;
      db.getWordId(validQueryWords[0].word, wordid);
      return make_shared<InverseListHeader>(*(iindex.find(wordid)->second));
   }
   else { // validQueryWords.size() > 1
      // 多个查询词意味着要对多个
      // 根据单词出现的文档数目排序，出现的越少的单词越靠前
      for(auto a: validQueryWords)
         cout<<a.word<<endl;
      sort(validQueryWords.begin(), validQueryWords.end(),
            [this](const cppjieba::Word& w1, const cppjieba::Word& w2){
               uint32_t id1, id2;
               db.getWordId(w1.word, id1);
               db.getWordId(w2.word, id2);
               return this->iindex.find(id1)->second->df < this->iindex.find(id2)->second->df;
            });
      uint32_t wordid;
      db.getWordId(validQueryWords[0].word, wordid);
      auto res = make_shared<InverseListHeader>(*(iindex.find(wordid)->second));

      //合并后的inverselistheader指向的是全部词语共同出现的文档，inverselist内的position没有意义
      for(auto w: validQueryWords){
         uint32_t id;
         db.getWordId(w.word, id);
         res->mergeIntersection(*(iindex.find(id)->second));
      }

      
      return res;
   }
   
}

void IndexConstructor::searchWordForTest(const string& word) const {
   uint32_t wordId;
   DBConn::STATE rc = db.getWordId(word, wordId);
   if(rc!=0) {
      rc = db.putNewWord(word, wordId);
   }

   auto it = iindex.find(wordId);
   if(it == iindex.end()) {
      return ;
   }
   auto header = it->second;
   auto next = header->next;
   while(next){
      cout << "文档" << next->documentId << "出现词频是:" << next->tf << endl;
      cout<<"第一篇文档出现的前五个位置是: ";
      int cnt{0};
      for(auto position:next->positions) {
         cout << position << " ";
         ++cnt;
         if(cnt==5) break;
      }
      cout<<endl;
      next = next->next;
   }
}