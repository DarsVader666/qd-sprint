#include"SimpleVector.h"
using namespace std;

SimpleVector::SimpleVector(const SimpleVector& other){
    // 1. 先拷贝大小和容量
    m_size = other.m_size;
    m_capacity = other.m_capacity;
    
    // 2. 防御性判空（如果别人是空的，你直接给空指针）
    if (other.m_data != nullptr && m_capacity > 0) {
        // 3. 必须自己重新申请一块【全新】的内存！
        m_data = new int[m_capacity]; 
        
        // 4. 把别人的数据一个个搬到自己家里来
        for (std::size_t i = 0; i < m_size; ++i) {
            m_data[i] = other.m_data[i];
        }
    } else {
        m_data = nullptr;
    }
}
SimpleVector::SimpleVector(SimpleVector &&other) noexcept:m_data(other.m_data),m_size(other.m_size),m_capacity(other.m_capacity){
    other.m_data=nullptr;
    other.m_capacity=0;
    other.m_size=0;
}
SimpleVector& SimpleVector::operator=(const SimpleVector& other){
    if(this==&other) return *this;//自拷贝直接返回
    delete[] m_data;
    m_size=other.size();
    m_capacity=other.capacity();
    if(!other.empty()){
        m_data=new int[m_capacity];
        for(int i=0;i<m_size;i++){
            m_data[i]=other.m_data[i];//深拷贝数据
        }
    }else{
        m_data=nullptr;
        m_capacity=0;
    }
    return *this;
}

SimpleVector& SimpleVector::operator=(SimpleVector&& other) noexcept{
    if(this==&other) return *this;
    delete[] m_data;
    m_data=other.m_data;
    m_size=other.m_size;
    m_capacity=other.m_capacity;
    other.m_data=nullptr;
    other.m_size=0;
    other.m_capacity=0;
    return *this;
}

size_t SimpleVector:: size() const{
    return m_size;
};
size_t SimpleVector::capacity() const{
    return m_capacity;
};
bool SimpleVector:: empty() const{
    return m_size==0;
};

void SimpleVector::reserve(std::size_t n) {
    if (n <= m_capacity) return;
    int* p = new int[n];
    for (std::size_t i = 0; i < m_size; ++i) p[i] = m_data[i];
    delete[] m_data;
    m_data = p;
    m_capacity = n;
}

void SimpleVector:: push_back(int v){
    if(m_capacity>m_size){
        m_data[m_size]=v;
        m_size++;
    }
    else{
        reserve(m_capacity ? m_capacity * 2 : 1);
        m_data[m_size]=v;
        ++m_size;

    }
};
void SimpleVector:: pop_back(){
    if(m_size==0) return;
    --m_size;
}
void SimpleVector:: clear(){
    m_size=0;
}
int& SimpleVector:: operator[](size_t i){
    assert(i<m_size);
    return m_data[i];
}
const int& SimpleVector:: operator[](size_t i) const{
    assert(i<m_size);
    return m_data[i];
}
int* SimpleVector:: data(){
    return m_data;    
}