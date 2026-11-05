#include<bits/stdc++.h>
using namespace std;

int n;
vector<int> deg(26,0);
vector<string> names;
vector<vector<char>> graph(26);
vector<char> old;
vector<char> alphabet(26);//存新字母顺序
list<char> alp;//便于增删位置
unordered_map<char,bool> vis;

void add(char cur,char nxt){
    graph[(int)(cur-'a')].push_back(nxt);
    deg[(int)(nxt-'a')]++;
}
void solve(){
    cin>>n;
    for(int i=0;i<n;i++) {
        string temp;
        cin>>temp;
        names.push_back(temp);
    }//存名字
    
    string cur=names[0];
    int cur_size=cur.size();
    alp.push_back(cur[0]);
    for(int i=0;i<n-1;i++){//i与i+1比较建图
        string nxt=names[i+1];
        int nxt_size=nxt.size();
        if(cur_size<=nxt_size){
            for(int j=0;j<cur_size;j++){
                if(cur[j]==nxt[j]) continue;//检验j位是否一样
                add(cur[j],nxt[j]);//如果不一样，那么加边
                break;
            }
            //如果cur搞完了都一样，那没信息了
        }else{
            int j=0;
            for(;j<nxt_size;j++){
                if(cur[j]==nxt[j]) continue;//检验j位是否一样
                add(cur[j],nxt[j]);//如果不一样，那么加边
                break;
            }
            if(j==nxt_size){//都一样，但是前面更长，绝对不可能
                cout<<"Impossible"<<'\n';
                return;
            }
        }
        cur=nxt;
        cur_size=nxt_size;
    }
    //建图完毕开始检验有没有环，a->b代表a比b小
    //用小根堆：每次从入度为0的点里取最小的，得到字典序最小的拓扑序
    priority_queue<int,vector<int>,greater<int>> pq;
    string res;//存拓扑序
    for(int i=0;i<26;i++){
        if(deg[i]==0){//如果入度为0,说明最小
            pq.push(i);//入堆
        }
    }
    while(!pq.empty()){
        int cur=pq.top();pq.pop();
        res+=(char)(cur+'a');
        for(auto &x:graph[cur]){
            int idx=(int)(x-'a');
            if(--deg[idx]==0){
                pq.push(idx);//入堆
            }
        }
    }
    if((int)res.size()<26){//凑不齐26个字母，说明有环
        cout<<"Impossible"<<'\n';
        return;
    }
    cout<<res<<'\n';

}
int main(){
    ios::sync_with_stdio(false);
    cin.tie(0),cout.tie(0);
    int _=1;
    while(_--) solve();
    return 0;
}