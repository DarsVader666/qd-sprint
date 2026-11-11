#include<bits/stdc++.h>
using namespace std;
#define int long long
using P=pair<int,int>;


int n;//顶点个数
vector<vector<int>> graph;//建立邻接关系
vector<P> val;//顶点左右值
vector<bool> vis;//建图用的
vector<bool> vis2;//逆序dp用的
vector<int> BFS;//这道题要用bfs

void solve(){
    cin>>n;
    graph.resize(n+1);
    val.resize(n+1);
    vis.assign(n+1,0);
    for(int i=1;i<=n;i++){
        cin>>val[i].first>>val[i].second;//存左右值
    }
    for(int i=1;i<n-1;i++){
        int x,y;
        cin>>x>>y;
        graph[x].push_back(x);
        graph[y].push_back(x);//存邻接
    }

    BFS.push_back(1);//把第一个节点塞进去
    int l=0,r=0;//左右指针
    while(l<=r){//点没塞玩
        if(vis[l++]) continue;//遍历过了直接跳过
        for(int i=0;i<graph[l].size();i++){//遍历每个邻接
            int nxt=graph[l][i];
            if(!vis[nxt]){//避免递归访问
                vis[nxt]=1;
                r++;
            }
        }


    }



    return; 
} 

signed main(){
    ios::sync_with_stdio(false);
    cin.tie(0),cout.tie(0);
    int _;
    cin>>_;
    while(_--) solve();



    return;
}

