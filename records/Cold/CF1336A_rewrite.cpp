#include<bits/stdc++.h>
#define int long long
using namespace std;

int n,k;
vector<vector<int>> graph;
vector<int> height;
vector<int> child;
vector<bool> vis;
vector<int> val;
void DFS(int x){
    for(int i=0;i<graph[x].size();i++){//每条边处理
        vis[x]=1;//标记为访问过
        int cur=graph[x][i];
        if(vis[cur]) continue;//访问过就跳过
        height[cur]=height[x]+1;//等于上一层+1，这个是向下算的，所以是前序
        DFS(cur);
        child[x]+=child[cur];//等于字节点儿子之和，这个是向上算的，所以是后序
    }
    //循环完了
    val[x]=height[x]-1-(child[x]-1);
}



void solve(){
    cin>>n>>k;
    graph.resize(n+1);
    child.assign(n+1,1);
    height.assign(n+1,1);
    vis.assign(n+1,0);
    val.assign(n+1,0);
    for(int i=1;i<=n-1;i++){
        int x,y;
        cin>>x>>y;
        graph[x].push_back(y);
        graph[y].push_back(x);//双向存边
    }
    DFS(1);
    sort(val.begin()+1,val.end(),greater<int>());
    int cnt=0;
    for(int i=1;i<=k;i++){
        cnt+=val[i];
    }
    cout<<cnt<<'\n';
    return;
}



signed main(){
    ios::sync_with_stdio(false);
    cin.tie(0),cout.tie(0);
    int _=1;
    while(_--) solve();
}