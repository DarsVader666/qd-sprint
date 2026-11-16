#include<bits/stdc++.h>
#define int long long
using namespace std;

int m,n,c0,d0;
vector<int> dp(1005,0);
vector<int> d(15);
vector<int> c(15);
vector<int> b(15);
vector<int> a(15);

void solve(){
    cin>>n>>m>>c0>>d0;
    for(int i=1;i<=m;i++){
        cin>>a[i]>>b[i]>>c[i]>>d[i];
    } 

    for(int i=1;i<=m;i++){//先把带馅料的面团算了
        int cur=a[i]/b[i];//当前能做的带馅面团个数
        int temp=1;
        --cur;
        while(cur>0){

            for(int k=n;k>=c[i]*temp;k--){//倒序当01背包
                dp[k]=max(dp[k],dp[k-c[i]*temp]+temp*d[i]);//开始加了
            }
            temp*=2;
            cur-=temp;
        }
        cur+=temp;//减完1248...剩余的大小
        if(cur!=0){//有剩余，再倒序来一次
            for(int k=n;k>=c[i]*cur;k--){//倒序当01背包
                dp[k]=max(dp[k],dp[k-c[i]*cur]+cur*d[i]);//开始加了
            }
        }
    }
    for(int i=c0;i<=n;i++){//不带馅料是个完全背包
        dp[i]=max(dp[i],dp[i-c0]+d0);
    }

    cout<<dp[n]<<'\n';
    return;
}



signed main(){
    ios::sync_with_stdio(false);
    cin.tie(0),cout.tie(0);
    int _=1;
    while(_--) solve();

    return 0;
}