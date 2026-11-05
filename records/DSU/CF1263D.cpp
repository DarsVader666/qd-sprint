#include<bits/stdc++.h>
using namespace std;


int pre[26];
bool vis[26]={0};
int root(int x){
    return pre[x]=(pre[x]==x?x:root(pre[x]));//路径压缩
}
void merge(int x, int y){
    x=root(x),y=root(y);//找各自的根
    if(x^y) pre[x]=y;//只有根不同才合并
}

void solve(){
    for(int i=0;i<26;i++) pre[i]=i;
    int n;
    cin>>n;
    for(int i=0;i<n;i++){
        string temp;
        cin>>temp;//对长度当于等于2的单词的每个字母做一遍并查集
        for(char c : temp) {
            vis[c - 'a'] = true;
        }
        if(temp.size()>=2){
            sort(temp.begin(),temp.end());
            int x=(int)(temp[0]-'a');
            for(const char & c:temp){
                int y=(int)(c-'a');
                if(y^x){
                    merge(y,x);//把不同的x和c合并
                    x=y;
                }
            }
        }
    }

    unordered_set<int> roots;
    for(int i=0; i<26; i++){
        if(vis[i]){
            roots.insert(root(i));
        }
    }
    cout<<roots.size();
}


int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    int _=1;
    while(_--)solve();
    return 0;
}