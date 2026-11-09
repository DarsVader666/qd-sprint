#ifndef MY_UNIQUE_PTR
#define MY_UNIQUE_PTR

template<typename T>
class myUnique{
private:
    T* p_=nullptr;
public:
    static int moves;

    myUnique() noexcept=default;
    explicit myUnique(T* p) noexcept;//加了explicit代表什么？必须显式调用？不会再隐式调用？
    ~myUnique() noexcept;


    myUnique(const myUnique &) =delete;
    myUnique& operator=(const myUnique &) =delete;

    myUnique(myUnique && o) noexcept;
    myUnique& operator=(myUnique && o) noexcept;//为什么返回值是&？

    T* get() const;
    T& operator*() const;// 为什么这两个不用noexcept,后两个要？
    T* operator->() const noexcept;
    explicit operator bool() const noexcept;//这个是什么运算符来着？忘记了
    
    T* release() noexcept;
    void reset(T*p=nullptr) noexcept;//这两个函数什么意思来着

};
template<typename T>
int myUnique<T>::moves=0;
template<typename T>
myUnique<T>::myUnique(T* p) noexcept:p_(p){
}

template<typename T>
myUnique<T>::~myUnique() noexcept{
    delete p_;
}

template<typename T>
myUnique<T>::myUnique(myUnique &&o)noexcept{
    ++moves;
    p_=o.p_;//抢走o的资源
    o.p_=nullptr;//把o置空
}

template<typename T>
myUnique<T>& myUnique<T>::operator=(myUnique &&o)noexcept{
    if(this==&o) return *this;
    delete p_;
    p_=o.p_;
    o.p_=nullptr;
    return *this;
}

template<typename T>
T* myUnique<T>:: get() const{
    return p_;
}

template<typename T>
T& myUnique<T>::operator*() const{
    return *p_;//不太确定opereator*干嘛的，因为感觉不用不用重载也能用,是把解引用的类型从myunique变回T？
}

template<typename T>
T* myUnique<T>::operator->() const noexcept{
    return p_;//->感觉也和上面一个函数一个原理？
}

template<typename T>
myUnique<T>:: operator bool() const noexcept{
    return !(p_==nullptr);
}

template<typename T>
T* myUnique<T>::release()noexcept{
    T* temp_=p_;
    p_=nullptr;
    return temp_;
}

template<typename T>
void myUnique<T>::reset(T*p) noexcept{
    if(p==p_) return;
    delete p_;
    p_=p;
}

#endif