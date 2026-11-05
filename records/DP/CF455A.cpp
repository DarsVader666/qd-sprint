#include<bits/stdc++.h>
using namespace std;
#define ll long long
#define int long long
int n;
vector<int> nums;


void solve(){
    cin>>n;
    for(int i=0;i<n;i++){
        int temp;
        cin>>temp;
        nums.push_back(temp);
    }
    if(n==1){
        cout<<nums[0]<<'\n';
        return;
    }
    sort(nums.begin(),nums.end());
    vector<pair<int,int>> times;//统计不同字符与出现次数 
    int diff=0;//统计不同的数字数
    times.emplace_back(nums[0],1);//第一个塞进去
    for(int i=1;i<n;i++){
        if(nums[i]!=times.back().first){
            times.emplace_back(nums[i],1);//不一样就加新的数
        }
        else times.back().second++;//一样就加次数
    }

    diff=times.size();//不同的数字个数
    vector<ll> dp(diff,0);//dp[i]代表选第i种不同数字的情况下能达到的最大分数，开ll防止乘积爆炸
    dp[0]=times[0].first*times[0].second;
    if(diff==1){
        cout<<dp[0]<<'\n';
        return;
    }
    if(times[1].first==times[0].first+1){
        dp[1]=max((ll)(times[1].first*times[1].second),dp[0]);
        if(diff==2){
            cout<<max(dp[0],dp[1])<<'\n';
            return;
        }
    }else{
        dp[1]=times[1].first*times[1].second+dp[0];
        if(diff==2){
            cout<<dp[1]<<'\n';
            return;
        }
    }//边界条件判断
    

    for(int i=2;i<diff;i++){
        ll cur_points=times[i].first*times[i].second;
        if(times[i].first==times[i-1].first+1){
            dp[i]=max(cur_points+dp[i-2],dp[i-1]);
        }else{
            dp[i]=cur_points+dp[i-1];
        }
    }
    cout<<max(dp[diff-1],dp[diff-2])<<'\n';
    return;

}
signed main(){
    ios::sync_with_stdio(false);
    cin.tie(0),cout.tie(0);
    int _=1;
    while(_--) solve();
    return 0;
}