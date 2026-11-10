#include<bits/stdc++.h>
using namespace std;


#define int long long


vector<vector<int>> dp;
vector<int> nums;
int n;
int target;


void solve(){
    cin>>n>>target;
    dp.assign(n+1,vector<int>(2010,0));//都是0,所有数整体向右平移1002,0变1002,target变1<1002+target<2003
    nums.resize(n+1);
    for(int i=0;i<n;i++){
        cin>>nums[i+1];
    }  
    dp[0][1002]=1;//什么都不选，要求为0时，方案数为1
    // dp[0].assign(target+1,0);//不选数都是0
    for(int i=1;i<=n;i++){
        for(int j=0;j<2010;j++){
            if(j+nums[i]<2010){
                dp[i][j]+=dp[i-1][j+nums[i]];
            }
            if(j-nums[i]>=0){
                dp[i][j]+=dp[i-1][j-nums[i]];
            }

        }
    }


    cout<<dp[n][1002+target]<<'\n';
    return;
}


signed main(){
    ios::sync_with_stdio();
    cin.tie(0),cout.tie(0);
    int _=1;
    while(_--) solve();



    return 0;
}