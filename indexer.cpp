#include <fstream>
using std::ifstream;

#include <iostream>
using std::cout;
using std::endl;

#include <string>
using std::string;

#include "indexer.hpp"

using namespace std;

/*********** 静态成员初始化 ***********/
JiebaTokenizer 
IndexConstructor::tokenizer = JiebaTokenizer();
/*********** 静态成员初始化结束 ***********/

bool IndexConstructor::addDocument(BaseParser& parser) {
   string line;
   vector<cppjieba::Word> jiebawords;
   uint32_t document_offset{1};
   while(parser.getContent(line)) {
      // cout<<line<<" "<<document_offset << " " << line.size()<<endl;
      IndexConstructor::tokenizer.tokenize(line, jiebawords);
      for(auto jiebaword: jiebawords) {
         // cout<<jiebaword<<endl;
         addWord2InverseList(jiebaword.word, 
                             parser.getFilepath(),
                             jiebaword.offset+document_offset);
      }
      document_offset += line.size() + 1; // 加 1 是因为换行符
   }
   return true;
}

bool IndexConstructor::addWord2InverseList(const string& word, 
                                           const string& document, 
                                           uint32_t offset){
   if(iindex.find(word) == iindex.end()) {
      // 当前词不存在
      iindex[word] = new InverseListHeader();
      iindex[word]->list = new InverseList(document);
      iindex[word]->df = 1;
   }
   
   // 此时词一定存在 iindex 中
   auto it = iindex.find(word);
   auto header = it->second;
   ++(header->total_tf);
   if(header->list->document_path != document) {
      auto temp = new InverseList(document);
      temp->positions.push_back(offset);
      temp->tf = 1;
      temp->next = header->list;
      header->list = temp;
      ++(header->df);
   }
   else{
      header->list->positions.push_back(offset);
      ++(header->list->tf);
   }
   return true;
}

InverseList* 

void IndexConstructor::searchWord(const string& word) const {
   auto it = iindex.find(word);
   if(it == iindex.end()) {
      cout<<"该单词没有被收录"<<endl;
      return ;
   }
   auto header = it->second;
   cout<<"文档数:"<<header->df<<endl;
   auto list = header->list;
   while(list){
      cout << "文档" << list->document_path << "出现词频是:" << list->tf << endl;
      cout<<"第一篇文档出现的前五个位置是: ";
      int cnt{0};
      for(auto position:list->positions) {
         cout << position << " ";
         ++cnt;
         if(cnt==5) break;
      }
      cout<<endl;
      list = list->next;
   }
}