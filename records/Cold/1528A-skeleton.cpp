#include <bits/stdc++.h>
using namespace std;

void solve() {
    int n;
    cin >> n;

    vector<pair<long long, long long>> v(n + 1);   // v[i].first = l[i], .second = r[i]
    for (int i = 1; i <= n; i++) cin >> v[i].first >> v[i].second;

    vector<vector<int>> g(n + 1);
    for (int i = 0; i < n - 1; i++) {              // n 个点有 n-1 条边
        int a, b; cin >> a >> b;
        g[a].push_back(b);
        g[b].push_back(a);
    }

    // ===== ① 一次 BFS：拿父亲 par[] 和访问顺序 order[] =====
    vector<int> par(n + 1, 0), order;
    order.reserve(n);
    vector<char> vis(n + 1, 0);

    vis[1] = 1;
    order.push_back(1);
    for (size_t h = 0; h < order.size(); h++) {
        int u = order[h];
        for (int w : g[u]) {
            if (vis[w]) continue;
            vis[w] = 1;
            par[w] = u;
            order.push_back(w);
        }
    }

    // ===== ② 逆序扫 order = 后序：孩子一定先于父亲处理完 =====
    // dp[u][0] = u 取 l[u] 时，u 这棵子树内的最大总和
    // dp[u][1] = u 取 r[u] 时，u 这棵子树内的最大总和
    vector<array<long long, 2>> dp(n + 1);

    for (int i = n - 1; i >= 0; i--) {
        int u = order[i];
        dp[u] = {0, 0};                            // 叶子：没有孩子，贡献为 0

        for (int w : g[u]) {
            if (w == par[u]) continue;             // 跳过父亲，剩下的全是儿子

            // ===== TODO：你要填的地方 =====
            // 对 a ∈ {0,1}（u 取哪个值）、b ∈ {0,1}（w 取哪个值）
            // 取 max( dp[w][b] + |v[u][a] - v[w][b]| )，累加到 dp[u][a]

            for (int a = 0; a < 2; a++) {
                long long best = -1;
                // ---- 你写这几行 ----
                //
                //
                // ---------------------
                dp[u][a] += best;
            }
        }
    }

    cout << max(dp[1][0], dp[1][1]) << '\n';       // 根节点两种取法取大者
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T; cin >> T;
    while (T--) solve();
}
