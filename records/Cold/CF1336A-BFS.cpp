#include<bits/stdc++.h>
using namespace std;


int n,k;
vector<vector<int>> graph;
vector<int> height;//高度
deque<int> node;//[i][j]代表i节点的层数为j
vector<bool> vis;//是否访问过
vector<int> father;//记录每个节点的父节点
vector<int> child;//记录每个节点的字节点数量
vector<int> order;//记录节点顺序
vector<int> val;

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
    order.push_back(0);
    vis[1]=1;//标记为访问过
    while(!node.empty()){//一共n-1条边
        int top=node.front();
        node.pop_front();//第一个出队
        order.push_back(top);
        for(int j=0;j<graph[top].size();j++){//遍历节点的连接点
            int temp=graph[top][j];//单个节点
            if(vis[temp]) continue;//访问过了，再见
            father[temp]=top;//父亲是当前节点
            height[temp]=height[top]+1;//更新高度
            vis[temp]=1;//标记为访问过
            node.push_back(temp);//入队
        }
    }//n-1条边处理完了
    //height存了每个节点的高度
    //把列表按照层数排序
    for(int i=n;i>=1;i--){
        int cur=order[i];
        child[father[cur]]+=child[cur];
        val[cur]=height[cur]-1-(child[cur]-1);//初始child为1,每个人会多加1,这里要减掉
    }
    sort(val.begin()+1,val.end(),greater<int>());

    long long res=0;

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