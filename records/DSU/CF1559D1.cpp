#include<bits/stdc++.h>
using namespace std;
#define ll long long

using P=pair<int,int>;
vector<int> pre1;
vector<int> pre2;//两个并查集 

int n,m1,m2;
int root(int x,vector<int>& pre){
    return pre[x]=(pre[x]==x?x:root(pre[x],pre));//路径压缩
}

void merge(int x,int y,vector<int>& pre){
    if(x==y) return;
    pre[root(x,pre)]=root(y,pre);//把x的根接到y的根上
}

void solve(){
    cin>>n;
    cin>>m1>>m2;
    pre1.resize(n+1);//1-based
    pre2.resize(n+1);
    for(int i=1;i<=n;i++){
        pre1[i]=i;
    }
    for(int i=1;i<=n;i++){
        pre2[i]=i;
    }//初始化
    for(int i=1;i<=m1;i++){
        int x,y;
        cin>>x>>y;
        merge(x,y,pre1);
    }
    for(int i=1;i<=m2;i++){
        int x,y;
        cin>>x>>y;
        merge(x,y,pre2);//开始查根
    }
    if(m1<m2){
        swap(m1,m2);//始终是前面一个图的边多
        swap(pre1,pre2);
    }
    int cnt = n-1-m1;//可以加的边数
    if(cnt ==0 ){//没边了直接return;
        cout<<0<<'\n';
        return;
    }
    vector<P> res;//记录加的边
    //unordered_map<int,bool> vis;//检验根有没有连上
    for(int i=1;i<n;i++){
        for(int j=i+1;j<=n;j++){
            if(root(i,pre1)==root(j,pre1)) continue;//路径压缩根直连，根一样直接返回
            if(root(i,pre2)==root(j,pre2)) continue;//给另一个图检验
            res.emplace_back(i,j);
            merge(i,j,pre1);
            merge(i,j,pre2);
            if(!--cnt) break;//边加完了，结束循环
        }
        if(!cnt) break;
    }
    cout<<n-1-m1<<'\n';
    for(int i=0;i<n-1-m1;i++){
        cout<<res[i].first<<" "<<res[i].second<<'\n';
    }
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0),cout.tie(0);
    int _=1;
    while(_--) solve();
    return 0;
}