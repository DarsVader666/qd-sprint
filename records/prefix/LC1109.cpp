#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

class Solution {
public:
    vector<int> corpFlightBookings(vector<vector<int>>& bookings, int n) {
        vector<int> res(n,0);
        for(int i=0;i<bookings.size();i++){
            int l=bookings[i][0],r=bookings[i][1],nums=bookings[i][2];
            if (l-1 >= 0 && l-1 < n) res[l-1]+=nums;
            if (r-2 >= 0 && r-2 < n) res[r-2]-=nums;
        }
        for(int i=1;i<n;i++){
            res[i]+=res[i-1];
        }
        return res;
    }
};

int main() {
    string line;
    string s;
    // 兼容带空格、换行、带逗号的输入格式
    while (getline(cin, line)) {
        if (line.empty()) continue;
        s += line + " ";
    }
    
    // 将所有非数字和非负号的字符替换为空格
    for(char& c : s) {
        if (!isdigit(c) && c != '-') {
            c = ' ';
        }
    }
    
    stringstream ss(s);
    vector<int> nums;
    int val;
    // 提取出所有的数字
    while(ss >> val) {
        nums.push_back(val);
    }
    
    if(nums.empty()) return 0;
    
    // 最后一个数字是 n
    int n = nums.back();
    nums.pop_back();
    
    // 剩下每 3 个数字组成一个 booking
    vector<vector<int>> bookings;
    for(int i=0; i < nums.size(); i+=3) {
        if(i+2 < nums.size()) {
            bookings.push_back({nums[i], nums[i+1], nums[i+2]});
        }
    }
    
    Solution sol;
    vector<int> res = sol.corpFlightBookings(bookings, n);
    
    for(int i=0; i<res.size(); i++) {
        cout << res[i] << (i == res.size() - 1 ? "" : " ");
    }
    cout << endl;
    
    return 0;
}