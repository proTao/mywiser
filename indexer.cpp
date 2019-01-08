#include <iostream>
using std::cout;
using std::endl;

#include <string>
using std::string;

#include <vector>
using std::vector;

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

/*
求两个倒排列表的交集，得到的结果到排列表只有文档id是有意义的
倒排项中的position没有意义
*/
// InverseListHeader& 
// InverseListHeader::mergeIntersection(const InverseListHeader& rhs) {
//    InverseList* p1 = next;
//    InverseList* p2 = rhs.next;
//    InverseList* pre = this;
//    while(p1 && p2){
//       if(p1->documentId == p2->documentId){
//          p1 = p1->next;
//          p2 = p2->next;
//          pre = pre->next;
//       }
//       else if (p1->documentId < p2->documentId){
//          while(p1 && p1->documentId < p2->documentId){
//             pre->next = p1->next;
//             delete p1;
//             p1 = pre->next;
//          }
//       }
//       else { // p1->documentId > p2->documentId
//          while(p2 && p1->documentId > p2->documentId){
//             p2 = p2->next;
//          }
//       }
//    }
//    return *this;
// }

vector<uint32_t> search_phrase(const vector<const InverseList*>& cursors, 
                      const vector<uint32_t>& offsets) {
   vector<uint32_t> phrase_positions;

   // 初始化游标
   vector<vector<uint32_t>::const_iterator> position_its(cursors.size());
   for(int i=0; i!=position_its.size(); ++i){
      position_its[i] = begin(cursors[i]->positions);
   }

   // 开始搜索短语位置
   while(position_its[0] != cursors[0]->positions.end()){
      const uint32_t pos{*(position_its[0])};
      uint32_t next_pos{0};

      // 移动除了第一个位置向量以外的游标
      for(int i=1; i!=position_its.size(); ++i){
         while(position_its[i] != cursors[i]->positions.end() 
               && *(position_its[i]) - offsets[i] < pos) {
            ++(position_its[i]);
         }
         // 其中一个词找完了全部的位置
         if(position_its[i] == cursors[i]->positions.end()) 
            return phrase_positions;

         // 更新 next_pos
         if(*(position_its[i]) - offsets[i] > pos 
            && *(position_its[i]) - offsets[i] > next_pos) {
               next_pos = *(position_its[i]) - offsets[i];
            }
      }
      if(next_pos > 0){
         // 说明第一个位置迭代器指向的位置不是候选短语
         while(position_its[0] != cursors[0] -> positions.end()
                && *(position_its[0]) < next_pos)
            ++(position_its[0]);
      }
      else{
         // 找到了短语位置
         phrase_positions.push_back(pos);
         ++(position_its[0]);
      }
   }
   return phrase_positions;
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

shared_ptr<InverseListHeader> IndexConstructor::recall(const string& query) {
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
      sort(validQueryWords.begin(), validQueryWords.end(),
            [this](const cppjieba::Word& w1, const cppjieba::Word& w2){
               uint32_t id1, id2;
               db.getWordId(w1.word, id1);
               db.getWordId(w2.word, id2);
               return this->iindex.find(id1)->second->df < this->iindex.find(id2)->second->df;
            });

      // 初始化游标
      vector<const InverseList*> cursors(validQueryWords.size());
      for(int i=0; i!=validQueryWords.size(); ++i) {
         uint32_t wordid;
         db.getWordId(validQueryWords.at(i).word, wordid);
         cursors.at(i) = iindex.find(wordid)->second->next; // next是因为第一个InverseListHeader不考虑
      }
      
      auto res = make_shared<InverseListHeader>();
      while(cursors[0]) {
         uint32_t doc_id{cursors[0]->documentId};
         uint32_t next_doc_id{0};

         // 移动除了第一篇以外每一篇文档的游标
         for(int i=1; i!=cursors.size(); ++i){
            while(cursors[i] && cursors[i]->documentId < doc_id)
               cursors[i] = cursors[i]->next;

            // 其中一个词找完了全部的文档
            if(!cursors[i]) {
               return res;
            }
               
            // 更新 next_doc_id
            if(cursors[i]->documentId > doc_id && cursors[i]->documentId > next_doc_id) {
               next_doc_id = cursors[i]->documentId;
            }
         }
         if(next_doc_id>0){
            // 说明第一篇文档当前指向的文档不属于候选集合（没有出现在其它所有单词的到排列表中）
            while(cursors[0] && cursors[0]->documentId < next_doc_id)
               cursors[0] = cursors[0]->next;
         }
         else{
            // 找到了一个文档，所有query单词都在里面出现过
            vector<uint32_t> offsets(validQueryWords.size());
            for (int i=0; i!=offsets.size(); ++i){
               offsets[i] = validQueryWords[i].offset;
            }
            vector<uint32_t> phrase_positions = search_phrase(cursors, offsets);
            InverseList* temp = new InverseList(doc_id);
            temp->tf = phrase_positions.size();
            temp->positions = phrase_positions;
            temp->next = res->next;
            res->next = temp;
            res->df += 1;
            res->total_tf += temp->tf;
            // 移动游标指向下一个位置
            cursors[0] = cursors[0]->next;
         }
      }
      
      // uint32_t wordid;
      // db.getWordId(validQueryWords[0].word, wordid);
      // auto res = make_shared<InverseListHeader>(*(iindex.find(wordid)->second));

      // //合并后的inverselistheader指向的是全部词语共同出现的文档，inverselist内的position没有意义
      // for(auto w: validQueryWords){
      //    uint32_t id;
      //    db.getWordId(w.word, id);
      //    res->mergeIntersection(*(iindex.find(id)->second));
      // }

      
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