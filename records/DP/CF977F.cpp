#include<bits/stdc++.h>
using namespace std;

int n;
vector<int> nums;
unordered_map<int,int> idx;
vector<int> dp,pre;
void solve(){
    cin>>n;
    nums.resize(n+1);
    dp.resize(n+1);
    pre.resize(n+1);

    int best = 0, besti = 0;

    for(int i=1;i<=n;i++) cin>>nums[i];
    for(int i=1;i<=n;i++){
        int cur=nums[i];
        auto it=idx.find(cur-1);
        if(it==idx.end()){//没有n-1;
            dp[i]=1;
            pre[i]=0;
        }else{
            dp[i]=dp[it->second]+1;//有的话等于它+1
            pre[i]=it->second;
        }

        auto jt=idx.find(cur);
        if (jt == idx.end() || dp[jt->second] < dp[i]) {
            idx[cur] = i;
        }

        if (dp[i] > best) {
            best = dp[i];
            besti = i;
        }
    }

    vector<int> seq;
    for (int i = besti; i != 0; i = pre[i]) seq.push_back(i);
    reverse(seq.begin(), seq.end());

    cout << best << '\n';
    for (int i = 0; i < (int)seq.size(); i++) {
        cout << seq[i] << (i + 1 == (int)seq.size() ? '\n' : ' ');
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
