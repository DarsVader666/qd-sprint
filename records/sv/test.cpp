#include "SimpleVector.h"
#include <cassert>
#include <cstdio>
int main(){
    SimpleVector a;
    assert(a.size()==0 && a.capacity()==0 && a.empty());
    for(int i=0;i<10;i++) a.push_back(i);
    assert(a.size()==10 && a.capacity()>=10);
    for(int i=0;i<10;i++) assert(a[i]==i);

    SimpleVector b(a);
    a[0]=99;
    assert(b[0]==0);

    a = a;
    assert(a[0]==99 && a.size()==10);

    SimpleVector c; c = b = a;
    assert(c.size()==10 && c[0]==99 && b[0]==99);

    b.reserve(100);
    assert(b.capacity()>=100 && b.size()==10);

    SimpleVector d;
    for(int i=0;i<5;i++) d.push_back(i);
    while(!d.empty()) d.pop_back();
    assert(d.size()==0);
    d.push_back(42);
    assert(d[0]==42 && d.size()==1);
    printf("all tests passed\n");
}
