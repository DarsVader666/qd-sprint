#include<bits/stdc++.h>
using namespace std;


int n,m;
vector<vector<int>> dp;//dp[i][j]代表a第i位，b第j位结尾时最长子序列
//边界条件：
vector<int> a;
vector<int> b;


void solve(){
    cin>>n>>m;
    a.resize(n+1);
    b.resize(m+1);
    for(int i=1;i<=n;i++) cin>>a[i];
    for(int i=1;i<=m;i++) cin>>b[i];
    dp.assign(n+1,vector<int>(m+1,0));
    for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){//内层遍历的是b
            if(a[i]==b[j]) dp[i][j]=dp[i-1][j-1]+1;//白嫖长度
            else dp[i][j]=max(dp[i][j-1],dp[i-1][j]);
        }
    }
    cout<<dp[n][m]<<'\n';
    return;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0),cout.tie(0);
    int _=1;
    while(_--) solve();
    return 0;
}