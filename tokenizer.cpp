#include "tokenizer.hpp"
#include "utils.hpp"

#include <fstream>
using std::ifstream;

#include <algorithm>
using std::copy_if;
using std::copy;

#include <iterator>
using std::back_inserter;

#include <stdexcept>
using std::runtime_error;

/*********** 静态成员初始化 ***********/
set<string> Tokenizer::stopset = set<string>();

const string 
JiebaTokenizer::DICT_PATH = "cppjieba/dict/jieba.dict.utf8";
const string 
JiebaTokenizer::HMM_PATH = "cppjieba/dict/hmm_model.utf8";
const string 
JiebaTokenizer::USER_DICT_PATH = "cppjieba/dict/user.dict.utf8"; 
const string 
JiebaTokenizer::IDF_PATH = "cppjieba/dict/idf.utf8"; 
const string 
JiebaTokenizer::STOP_WORD_PATH = "cppjieba/dict/stop_words.utf8";

cppjieba::Jieba 
JiebaTokenizer::jieba = cppjieba::Jieba(DICT_PATH,
   HMM_PATH,
   USER_DICT_PATH,
   IDF_PATH,
   STOP_WORD_PATH
);

cppjieba::MPSegment
JiebaTokenizer::seg = cppjieba::MPSegment(JiebaTokenizer::DICT_PATH);

/*********** 静态成员初始化结束 ***********/

void JiebaTokenizer::initStopSet() {
   ifstream ifs(JiebaTokenizer::STOP_WORD_PATH);
   if(ifs){
      string s;
      while(ifs>>s){
         stopset.insert(s);
      }
   }
}

bool JiebaTokenizer::isNotStop(const cppjieba::Word& jiebaword) {
   string word = jiebaword.word;
   if(isUtf8Punct(word)){
      return false;
   }
   if(stopset.find(word) != stopset.end()){
      return false;
   }
   return true;
}


void JiebaTokenizer::tokenize(const string& line, 
                              vector<cppjieba::Word>& jiebawords, 
                              bool forSearch) {
   vector<cppjieba::Word> tempwords;
   if(forSearch) {
      JiebaTokenizer::jieba.CutForSearch(line, tempwords);
   }
   else {
      JiebaTokenizer::seg.Cut(line, tempwords);
   }

   // 过滤停用词
   if(jiebawords.size() > 0){
      // cout<<"[error] "<<jiebawords.size()<<endl;
      // throw runtime_error("parameter <jiebawords> must be empty for saving results;");
      jiebawords.clear();
   }
   auto inserter = back_inserter(jiebawords);
   auto it=copy_if(tempwords.begin(), tempwords.end(), inserter, isNotStop);
   // jiebawords.resize(distance(jiebawords.begin(), it)); //释放多余vector空间
   jiebawords.shrink_to_fit();
}