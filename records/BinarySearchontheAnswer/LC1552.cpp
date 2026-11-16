#include <bits/stdc++.h>
using namespace std;

class Solution {
private:
    bool check(int mid, int m, int n, const vector<int> &position) {
        int pre = 0;
        m--;
        for (int i = 1; i < n; i++) {
            int cur_position = position[i];
            int pre_position = position[pre];
            if (cur_position - pre_position >= mid) {
                pre = i;
                m--;
            }
            if (!m) return true;
        }
        return false;
    }

public:
    int maxDistance(vector<int>& position, int m) {
        sort(position.begin(), position.end());
        int n = position.size();
        int l = 0, r = position[n - 1];
        while (l + 1 < r) {

            int mid = (l + r) / 2;
            if (check(mid, m, n, position)) {
                l = mid;
            } else {
                r = mid;
            }
        }
        return l;
    }
};
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int m;

    vector<int> position;
    while(cin>>m) {
        position.push_back(m);
    }

    m=position.back();
    position.pop_back();
    Solution sol;
    cout << sol.maxDistance(position, m) << "\n";
    return 0;
}