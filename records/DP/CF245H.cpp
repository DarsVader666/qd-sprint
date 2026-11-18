#include<bits/stdc++.h>
using namespace std;

int n;
string s;
vector<vector<bool>> dp;//存是否是回文
vector<int> ret;
vector<vector<int>> sum;

void solve(){
    cin>>s>>n;
    int len=s.size();
    s.insert(s.begin(),'A');
    sum.assign(len+2,vector<int>(len+2,0));
    ret.resize(2*n+1);
    dp.assign(len+2,vector<bool>(len+2,0));//一开始都是假的
    for(int i=1;i<=2*n;i+=2){
        cin>>ret[i]>>ret[i+1];//存区间
    }

    //先计算字串是不是回文的
    for(int i=1;i<=len;i++){
        for(int l=1;l<=len-i+1;l++){
            int r=l-1+i;
            if(i==1) {
                dp[l][l]=1;//自己和自己是回文的
                continue;
            }
            if(s[l]==s[r]){//区间相等
                if(i==2){
                    dp[l][r]=1;
                }else if(dp[l+1][r-1]==1) dp[l][r]=1;//中间回文外面相等那就是1
            }
        }
    }
    for(int i=1;i<=len;i++){
        for(int l=1;l<=len-i+1;l++){
            int r=l-1+i;
            sum[l][r]=sum[l+1][r]+sum[l][r-1]-sum[l+1][r-1]+dp[l][r];
        }
    }

    for(int i=1;i<=2*n;i+=2){
        int x=ret[i],y=ret[i+1];
        cout<<sum[x][y]<<'\n';
    }

    return;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0),cout.tie(0);
    int _=1;
    while(_--) solve();


    return 0;

}