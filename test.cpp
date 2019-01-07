// #include "cppjieba/include/cppjieba/Jieba.hpp"
#include "indexer.hpp"


using namespace std;


int main(){
    InverseListHeader i1, i2;
    i1.df = 1;
    i2.df = 2;
    i1.next = new InverseList(1);
    i1.next->next = new InverseList(2);
    i1.next->next->next = new InverseList(3);
    i1.next->next->next->next = new InverseList(4);
    i2.next = new InverseList(1);
    i2.next->next = new InverseList(3);
    i2.next->next->next = new InverseList(5);
    i2.next->next->next->next = new InverseList(7);

    cout<<"begin merge"<<endl;
    i1.mergeIntersection(i2);
    cout<<"end merge"<<endl;

    InverseList* p = i1.next;
    while(p){
        cout<<p->documentId<<" ";
        p = p->next;
    }
    cout<<endl;
    return 0;
}