#include "utils.hpp"
#include <codecvt>
#include <string>
#include <locale>
#include <iostream>

using std::wstring_convert;
using std::string;
using std::wstring;
using std::codecvt_utf8;
using std::locale;

using std::cin;
using std::cout;
using std::endl;


bool isUtf8Punct(const std::string& s)  {
    // 参考：https://www.zhihu.com/question/35254977/answer/61945181
    
    wstring_convert<codecvt_utf8<wchar_t>> conv;
    wstring ws = conv.from_bytes(s);
    wchar_t ch = ws.at(0);
    locale loc("en_US.UTF-8");
    if (!ispunct(ch, loc) && !iswspace(ch) ){
        return false;
    }
    return true;
}

// bool isUtf8Punct(const cppjieba::Word& w)  {
//     string s = w.word;
//     wstring_convert<codecvt_utf8<wchar_t>> conv;
//     wstring ws = conv.from_bytes(s);
//     wchar_t ch = ws.at(0);
//     locale loc("en_US.UTF-8");
//     if (!ispunct(ch, loc) && !iswspace(ch) ){
//         return false;
//     }
//     return true;
// }

// int main(){
//     string s;
//     cout<<"input"<<endl;
//     cin >> s;
//     cout<<isUtf8Punct(s)<<endl;
//     return 0;
// }