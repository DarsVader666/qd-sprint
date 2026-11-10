#include<bits/stdc++.h>
#define int long long 
using namespace std;



int n,m;
vector<vector<int>> cal;
vector<vector<int>> dp1;//左上出发
vector<vector<int>> dp2;//左下出发
vector<vector<int>> dp3;//右上出发
vector<vector<int>> dp4;//右下出发

void solve(){
    cin>>n>>m;
    cal.assign(n+1,vector<int>(m+1,0));
    for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            cin>>cal[i][j];
        }
    }
    dp1.assign(n+2,vector<int>(m+2,0));
    dp2.assign(n+2,vector<int>(m+2,0));
    dp3.assign(n+2,vector<int>(m+2,0));
    dp4.assign(n+2,vector<int>(m+2,0));

    for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            dp1[i][j]=max(dp1[i-1][j],dp1[i][j-1])+cal[i][j];
        }
    }
    for(int i=n;i>=1;i--){
        for(int j=1;j<=m;j++){
            dp2[i][j]=max(dp2[i+1][j],dp2[i][j-1])+cal[i][j];
        }
    }
    for(int i=1;i<=n;i++){
        for(int j=m;j>=1;j--){
            dp3[i][j]=max(dp3[i-1][j],dp3[i][j+1])+cal[i][j];
        }
    }
    for(int i=n;i>=1;i--){
        for(int j=m;j>=1;j--){
            dp4[i][j]=max(dp4[i+1][j],dp4[i][j+1])+cal[i][j];
        }
    }


    int res=0;
    for(int i=2;i<=n-1;i++){
        for(int j=2;j<=m-1;j++){
            int x1=dp1[i-1][j]+dp4[i+1][j]+dp2[i][j-1]+dp3[i][j+1];
            int x2=dp1[i][j-1]+dp4[i][j+1]+dp2[i+1][j]+dp3[i-1][j];
            int xx=max(x1,x2);
            res=max(res,xx);
        }
    }
    cout<<res<<'\n';
    return;
}

signed main(){
    ios::sync_with_stdio(false);
    cin.tie(0),cout.tie(0);
    int _=1;
    while(_--) solve();
    return 0;
}