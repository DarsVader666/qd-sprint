#include<bits/stdc++.h>
using namespace std;

int n,m;
vector<vector<int>> cal;
vector<vector<int>> dp1;//dp[i][j]代表左上到达i/j时最大calorie
vector<vector<int>> dp2;//dp[i][j]代表左下到达i/j时最大calorie
vector<vector<int>> dp3;//dp[i][j]代表右上到达i/j时最大calorie
vector<vector<int>> dp4;//dp[i][j]代表右下到达i/j时最大calorie

void  solve(){
    cin>>n>>m;
    cal.resize(n+2,vector<int>(m+2,0));
    dp1.resize(n+2,vector<int>(m+2,0));
    dp2.resize(n+2,vector<int>(m+2,0));
    dp3.resize(n+2,vector<int>(m+2,0));
    dp4.resize(n+2,vector<int>(m+2,0));
    for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            cin>>cal[i][j];
        }
    }//输入
    for (int i = 1; i <= n; i++)
    for (int j = 1; j <= m; j++)
        dp1[i][j] = max(dp1[i-1][j], dp1[i][j-1]) + cal[i][j];

    // dp2: (n,1) -> (i,j)
    for (int i = n; i >= 1; i--)
    for (int j = 1; j <= m; j++)
        dp2[i][j] = max(dp2[i+1][j], dp2[i][j-1]) + cal[i][j];

    // dp3: (1,m) -> (i,j)
    for (int i = 1; i <= n; i++)
    for (int j = m; j >= 1; j--)
        dp3[i][j] = max(dp3[i-1][j], dp3[i][j+1]) + cal[i][j];

    // dp4: (n,m) -> (i,j)
    for (int i = n; i >= 1; i--)
        for (int j = m; j >= 1; j--)
            dp4[i][j] = max(dp4[i+1][j], dp4[i][j+1]) + cal[i][j];
    int maxRes=0;
    for(int i=2;i<=n-1;i++){
        for(int j=2;j<=m-1;j++){
            maxRes=max(maxRes,max(dp1[i][j-1]+dp4[i][j+1]+dp2[i+1][j]+dp3[i-1][j],dp1[i-1][j]+dp4[i+1][j]+dp2[i][j-1]+dp3[i][j+1]));
        }
    }
    cout<<maxRes<<'\n';
    return;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0),cout.tie(0);
    int _=1;
    while(_--) solve();
    return 0;
}