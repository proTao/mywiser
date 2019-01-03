#include <string>
using std::string;

#include <vector>
using std::vector;

#include "cppjieba/include/cppjieba/Jieba.hpp"



class Tokenizer {
    public:
        virtual void
        tokenize(const string&, vector<cppjieba::Word>&, bool) = 0;
};


class JiebaTokenizer: public Tokenizer {

public:
    JiebaTokenizer() = default;

    virtual void
    tokenize(const string&, vector<cppjieba::Word>&, bool) override;

    static const string DICT_PATH;
    static const string HMM_PATH;
    static const string USER_DICT_PATH;
    static const string IDF_PATH;
    static const string STOP_WORD_PATH;
    static cppjieba::Jieba jieba;
    static cppjieba::MPSegment seg;
};
