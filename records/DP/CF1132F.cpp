#include<bits/stdc++.h>
using namespace std;

int n;
string s;
vector<vector<int>> dp(505,vector<int>(505,INT_MAX));

void solve(){
    
    cin>>n>>s;
    for (int i = 1; i <= n + 1; i++) dp[i][i-1] = 0;   // 空区间 = 0
    s.insert(s.begin(),'A');
    for(int len=1;len<=n;len++){
        for(int l=1;l<=n-len+1;l++){
            int r=l+len-1;
            dp[l][r] = 1+dp[l+1][r];
            for(int k=l+1;k<=r;k++){//找相同
                if(s[k]==s[l]){
                    dp[l][r]=min(dp[l][r],dp[l+1][k-1]+dp[k][r]);
                }
            }
        }
    }

    cout<<dp[1][n]<<'\n';
    return;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0),cout.tie(0);
    int _=1;
    while(_--) solve();



    return 0;
}