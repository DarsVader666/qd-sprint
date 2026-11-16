#include<bits/stdc++.h>
using namespace std;

#define int long long
const int N=1e9+7;
vector<int> dp(1e5+5,0);
vector<int> sum(1e5+5,0);//前缀和
int n;
int k;
int a,b,b_max=1;
void solve(){
    cin>>a>>b;
    int cnt=0;
    
    for(int i=b_max;i<=b;i++){
        if(i>=k){
            dp[i]=dp[i-k]+dp[i-1];
            dp[i]%=N;
            sum[i]=(dp[i]+sum[i-1])%N;
        }else{
            dp[i]=dp[i-1];
            sum[i]=(dp[i]+sum[i-1])%N;
        }
    }
    b_max=max(b,b_max);
    cnt=(sum[b]-sum[a-1]+N)%N;
    cout<<cnt<<'\n';
    return;
}
signed main(){
    ios::sync_with_stdio(false);
    cin.tie(0),cout.tie(0);
    int _;
    dp[0]=1;
    cin>>_>>k;
    while(_--) solve();


    return 0;
}