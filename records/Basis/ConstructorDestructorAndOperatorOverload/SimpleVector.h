#ifndef SIMPLE_VECTOR_H
#define SIMPLE_VECTOR_H
#include<cstddef>
#include<iostream>
#include<cassert>
class SimpleVector {
private:
    int* m_data=nullptr; // 指向堆上的动态数组

    std::size_t m_size=0; // 当前元素个数

    std::size_t m_capacity=0; // 当前分配的容量

public:
    SimpleVector(){
        
    };
    explicit SimpleVector(std::size_t n):m_capacity(n){
        if(n<=0) return;//这里异常处理还没学
        m_data = new int[m_capacity];//初始化动态数组
    }
    SimpleVector(const SimpleVector&);
    SimpleVector(SimpleVector &&other);
    SimpleVector& operator=(const SimpleVector&);
    SimpleVector& operator=(SimpleVector&&);
    ~SimpleVector(){
        delete []m_data;
    }
    std::size_t size() const;
    std::size_t capacity() const;
    bool empty() const;
    void reserve(std::size_t n);
    void push_back(int v);
    void pop_back();
    void clear();
    int& operator[](std::size_t i);
    const int& operator[](std::size_t i) const;
    int* data();
};
#endif