#include<bits/stdc++.h>
using namespace std;


int n,k;
vector<vector<int>> graph;
vector<int> height;//高度
deque<int> node;//[i][j]代表i节点的层数为j
vector<bool> vis;//是否访问过
vector<int> father;//记录每个节点的父节点
vector<int> child;//记录每个节点的字节点数量
// vector<int> order;//记录节点顺序

vector<int> val;
void DFS(int x){
    for(int i=0;i<graph[x].size();i++){//遍历它的节点
        int cur=graph[x][i];
        if(!vis[cur]){
            height[cur]=height[x]+1;//下一层等于我这层+1
            vis[cur]=1;//标记为访问过了
            DFS(cur);//递归字节点
            child[x]+=child[cur];
        }
    } 
    val[x]=(height[x]-1)-(child[x]-1);
}


void solve(){
    cin>>n>>k;
    graph.resize(n+1);
    height.assign(n+1,0);
    vis.assign(n+1,0);
    child.assign(n+1,1);//刚开始都没有子节点,得有1个，保证累加的时候有个初始值
    father.assign(n+1,0);//每个人的父亲都先连着1
    val.assign(n+1,0);
    for(int i=1;i<=n-1;i++){
        int u,v;
        cin>>u>>v;
        graph[u].push_back(v);
        graph[v].push_back(u);//定义无向图
    }
    node.push_back(1);//1入队
    height[1]=1;
    vis[1]=1;//标记为访问过
    DFS(1);//处理编号1的节点
    
    long long res=0;
    sort(val.begin()+1,val.end(),greater<int>());
    for(int i=1;i<=k;i++){
        res+=val[i];
    }
    cout<<res<<'\n';
    return;
}


int main(){
    ios::sync_with_stdio(false);
    cin.tie(0),cout.tie(0);
    int _=1;
    while(_--) solve();





    return 0;
}