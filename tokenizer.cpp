#include "tokenizer.hpp"

/*********** 静态成员初始化 ***********/
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
void JiebaTokenizer::tokenize(const string& line, 
                              vector<cppjieba::Word>& jiebawords, 
                              bool forSearch) {
   if(forSearch) {
      JiebaTokenizer::jieba.CutForSearch(line, jiebawords);
   }
   else {
      JiebaTokenizer::seg.Cut(line, jiebawords);
   }
}