#include<bits/stdc++.h>
using namespace std;

#define ll long long
#define int long long
const ll p=1e9+7;

int n,k,d;
vector<int> dp1(105,0);//初始化都是0
vector<int> dp0(105,0);
void solve(){
    cin>>n>>k>>d;
    if(d>n){
        cout<<0<<'\n';
        return;
    }
    dp0[0]=1;
    for(int i=1;i<=n;i++){
        for(int j=1;j<=k&&j<=i;j++){
            //对dp0操作，只有一种情况，从dp0来并且加的j<d
            if(j<d){
                dp0[i]+=dp0[i-j];//加上dp0方案数
                dp1[i]+=dp1[i-j];
                dp0[i]%=p;
                dp1[i]%=p;
            }else{//j>=d了可以从0到1转移了
                dp1[i]+=dp0[i-j];//加上dp0方案数
                dp1[i]+=dp1[i-j];//加上dp1方案数
                dp0[i]%=p;
                dp1[i]%=p;
            }

        }
    }
    cout<<dp1[n]<<'\n';
    return ;
}
signed main(){
    ios::sync_with_stdio(0);
    cin.tie(0),cout.tie(0);
    int _=1;
    while(_--) solve();




    return 0;
}