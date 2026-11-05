#include<bits/stdc++.h>

using namespace std;

#define int long long

vector<int> nums(5005,0);
vector<int> prefix(5005,0);//前缀和
vector<int> seq(5005,0);//以i结尾的，[i-(m-1),i]区间内的各段和
vector<vector<int>> dp(5005);//dp[i][j]代表以i结尾，选了j个时的和最大值
int n,m,k;

void solve(){
    cin>>n>>m>>k;
    dp.assign(n,vector<int>(k+1,0));
    for(int i=0;i<n;i++){
        cin>>nums[i];
        prefix[i+1]=prefix[i]+nums[i];
    }
    for(int i=m-1;i<n;i++){
        seq[i]=prefix[i+1]-prefix[i-(m-1)];
    }
    for(int i=m-1;i<n;i++){
        for(int j=1;j<=k;j++){
            if(i>=m)
            dp[i][j]=max(dp[i-1][j],dp[i-m][j-1]+seq[i]);//选择不选自己和选自己中的最大值
            else dp[i][j]=seq[i];
        }

    }
    cout<<dp[n-1][k]<<'\n';
    return;
}

signed main(){
    ios::sync_with_stdio(false);
    cin.tie(0),cout.tie(0);
    int _=1;
    while(_--) solve();


}