#include<bits/stdc++.h>
using namespace std;
template <typename T>
class myHeap{
private:
    vector<T> data{0};//拿来装数据，树的底层实现
    int n=0;//元素个数
    void sift_down(int i);
    void sift_up(int i);
public:
    myHeap(){}
    myHeap(const vector<T> & other);//直接拷贝过来初始化，再在函数体内建堆
    T pop();
    void push(T t);
    bool empty();
    T top();
    

};

template <typename T>
void myHeap<T>::sift_down(int i){
    while(i*2<=n){//有左孩子就一直沉到底
        int left=i*2;
        int right=i*2+1;
        int largest=i;
        if(data[left]>data[largest]) largest=left;
        if(right<=n&&data[right]>data[largest]) largest=right;
        if(largest!=i){
            swap(data[largest],data[i]);
            i=largest;//一直沉
        }else{break;}
    }
}
template <typename T>
void myHeap<T>::sift_up(int i){
    while(i>1){//有父节点就一直往上
        if(data[i]>data[i/2]){
            swap(data[i],data[i/2]);
            i/=2;
        }else break;
    }
}
template <typename T>
myHeap<T>::myHeap(const vector<T> & other):data(other){
    n=data.size()-1;
    for(int i=n/2;i>=1;i--){
        sift_down(i);
    }
}
template <typename T>
T myHeap<T>::pop(){
    T temp=data[1];//记录第一个元素
    data[1]=data[n];
    data.pop_back();
    --n;
    sift_down(1);
    return temp;
}
template <typename T>
void myHeap<T>::push(T t){
    ++n;
    data.push_back(t);
    sift_up(n);
}

template <typename T>
bool myHeap<T>::empty(){
    return n==0;
}
template <typename T>
T myHeap<T>::top(){
    return data[1];
}


int main(){
    int n,temp,num=0;
    long long life =0;
    cin>>n;
    num=n;
    vector<int> potion(1,0);
    for(int i=0;i<n;i++){
        cin>>temp;
        potion.push_back(-temp);//存负数，大顶堆变小顶堆
    }
    myHeap<int> mh;
    for(int i=1;i<=n;i++){
        mh.push(potion[i]);
        life-=potion[i];
        if(life<0){
            life+=mh.top();
            mh.pop();
            num--;
        }
    }
    cout<<num;
}