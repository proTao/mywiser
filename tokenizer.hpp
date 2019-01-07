#ifndef __TOKENIZER_H
#define __TOKENIZER_H

#include <string>
using std::string;

#include <iostream>
using std::cout;
using std::endl;

#include <vector>
using std::vector;

#include <set>
using std::set;

#include "cppjieba/include/cppjieba/Jieba.hpp"

class Tokenizer {
    public:
        virtual void
        tokenize(const string&, vector<cppjieba::Word>&, bool) = 0;
    
    protected:
        static set<string> stopset;

};


class JiebaTokenizer: public Tokenizer {

public:
    JiebaTokenizer() {
        initStopSet();
        cout << "[ initialize ] StopWordSet : "<< stopset.size() << endl;
    };
    virtual void tokenize(const string&, vector<cppjieba::Word>&, bool) override;
    static void initStopSet();
    // 用于tokenize中的copy-if中的过滤条件
    static bool isNotStop(const cppjieba::Word&);

    static const string DICT_PATH;
    static const string HMM_PATH;
    static const string USER_DICT_PATH;
    static const string IDF_PATH;
    static const string STOP_WORD_PATH;
    static cppjieba::Jieba jieba;
    static cppjieba::MPSegment seg;
};

#endif