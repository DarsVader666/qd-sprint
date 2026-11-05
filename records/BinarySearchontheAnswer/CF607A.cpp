#include<bits/stdc++.h>
using namespace std;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n;
    cin >> n;
    vector<pair<int, int>> p(n);
    for(int i = 0; i < n; i++) cin >> p[i].first >> p[i].second;
    
    sort(p.begin(), p.end());
    vector<int> a(n), b(n);
    for(int i = 0; i < n; i++) {
        a[i] = p[i].first;
        b[i] = p[i].second;
    }

    vector<int> dp(n, 0);
    int ans = n - 1; // 最少也是摧毁 n-1 个（只剩一个新塔和原塔中的最右侧存活）

    for(int i = 0; i < n; i++){
        int target = a[i] - b[i];
        // 找最右侧不被炸的 j
        int j = lower_bound(a.begin(), a.begin() + i, target) - a.begin() - 1;
        
        if(j < 0) {
            dp[i] = n - 1; // 左边全炸，右边全炸，只剩 i 自己
        } else {
            dp[i] = dp[j] - 1; // 💥 你的天才公式！
        }
        
        ans = min(ans, dp[i]);
    }

    cout << ans << '\n';
    return 0;
}