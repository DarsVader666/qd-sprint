#include "MyUniquePtr.h"
#include <cstdio>
#include <vector>

int main() {
    std::vector<myUnique<int>> v;
    for (int i = 0; i < 1000; ++i)
        v.push_back(myUnique<int>(new int(i)));

    printf("size = %zu\n", v.size());
    printf("移动构造被调用 %d 次\n", myUnique<int>::moves);

    long long sum = 0;
    for (auto& p : v) sum += *p;
    printf("sum = %lld  (期望 499500)\n", sum);
}