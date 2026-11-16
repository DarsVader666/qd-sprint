#include<bits/stdc++.h>
using namespace std;
#define ll long long

int k;
int n;
int sum(int x){//递归算能写的代码行数
    return x==0?0:x+sum(x/k);
}
bool check(int v){//开始检验
    return sum(v)>=n;
}
void solve(){
//二分答案，单位[1,n]
    cin>>n>>k;
    int l=0,r=n+1;//左右指针，l为完不成的v最大值，r为完成的v最小值
    while(l+1<r){
        int mid=(l+r)/2;//找中间
        if(check(mid)){//满足了
            r=mid;
        }else{
            l=mid;
        } 
    }
    cout <<r;
}
int main(){
    ios::sync_with_stdio(false);
    cin.tie(0),cout.tie(0);
    int _=1;
    while(_--) solve();
    return 0;
}