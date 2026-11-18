#include<bits/stdc++.h>
using namespace std;

int n;
vector<int> nums;
vector<vector<int>> dp;

void solve(){
    cin>>n;
    nums.resize(n+1);
    dp.assign(n+1,vector<int>(n+1,INT_MAX));
    for(int i=1;i<=n;i++){
        cin>>nums[i];
    }
    for(int len =1;len<=n;len++){
        for(int i=1;i<=n-len+1;i++){
            int j=i+len-1;//左右端点
            int min_dp=INT_MAX;
            if(len==1){
                dp[i][i]=1;
                continue;
            }
            min_dp=min(dp[i+1][j],dp[i][j-1])+1;//单独拎出来
            if (nums[i] == nums[j]) {
                int inner = (i+1 <= j-1) ? dp[i+1][j-1] : 0;   // 空区间给 0
                min_dp = min(min_dp, max(1, inner));
            }
            int min_temp=INT_MAX;
            for(int k=i+1;k<=j-1;k++){
                min_temp=min(min_temp,dp[i][k]+dp[k+1][j]);
            }
            min_dp=min(min_temp,min_dp);
            dp[i][j]=min_dp;
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