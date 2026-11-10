#ifndef MY_SHARE_H
#define MY_SHARE_H

// 1. 控制块有什么字段？
// 2. 拷贝构造：计数怎么变？
// 3. 拷贝赋值：先做什么后做什么？（这里有一个大坑）
// 4. 析构：什么时候 delete 资源？什么时候 delete 控制块？

struct cnt{
    int use_cnt;
    int weak_cnt; 
};
template<typename T>
class MyShared{
private:
    T* p_;
    cnt* cnt_;

public:
    MyShared();
    explicit MyShared(T* p);
    MyShared(const MyShared& o);
    MyShared& operator=(const MyShared& o);
    ~MyShared();
    T& operator*() const;
    T* operator->() const;
    int use_count() const;
};

template<typename T>
MyShared<T>::MyShared(){
    p_=nullptr;
    cnt_=nullptr;
}

template<typename T>
MyShared<T>::MyShared(T* p){
    p_=p;
    p=nullptr;//把原本的对象置空，不让它干扰
    cnt_=new cnt;//用裸指针初始化，必定是第一个智能指针
    cnt_->use_cnt=1;
    cnt_->weak_cnt=1;
}

template<typename T>
MyShared<T>::MyShared(const MyShared& o){
    if(this==&o) return;
    p_=o.p_;//指过去
    o.cnt_->use_cnt++;
    cnt_=o.cnt_;//指向同一个控制块
}

template<typename T>
MyShared<T>& MyShared<T>:: operator=(const MyShared& o){
    if(this==&o) return *this; 
    if(cnt_){
        if (--cnt_->use_cnt == 0) { delete p_; delete cnt_; }
    }
    p_=o.p_;
    o.cnt_->use_cnt++;
    cnt_=o.cnt_;
    return *this;
}

template<typename T>
MyShared<T>::~MyShared(){//析构函数
    if(!cnt_) return;
    if (--cnt_->use_cnt == 0) {
        delete p_;
        if (--cnt_->weak_cnt == 0) delete cnt_;
    }
}

template<typename T>
T& MyShared<T>::operator*() const{
    return *p_;
}

template<typename T>
T* MyShared<T>::operator->() const{
    return p_;
}

template<typename T>
int MyShared<T>::use_count() const{
    return cnt_->use_cnt;
}

#endif