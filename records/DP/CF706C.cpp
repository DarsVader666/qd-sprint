#include<bits/stdc++.h>
using namespace std;
#define int long long
int n;
const int NEG=-(1LL<<60);
vector<string> words(100005);
vector<string> rwords(100005);//翻转的字符串
vector<int> energy(100005);
vector<vector<int>> dp(100005,vector<int>(2,NEG));//dp[i][0/1]代表第i+1个单词结尾时，并且这个单词不翻转/翻转的能量最小值
bool lex(string a,string b){//判断字典序
    int as=a.size(),bs=b.size();
    int l=0;//指针比较
    while(l<as&&l<bs){//不超过两个单词长度
        if(a[l]==b[l]) l++;//比下一个
        else if(a[l]>b[l]) return false;
        else return true;
    }
    if(as>bs){//前缀相同，a更长
        return false;
    }
    return true;//前缀相同，b更长
}
void solve(){
    cin>>n;
    for(int i=0;i<n;i++){
        cin>>energy[i];
    }
    dp[0][0]=0,dp[0][1]=energy[0];//第一个单词必定成立，不用翻转
    for(int i=0;i<n;i++){
        cin>>words[i];
        rwords[i]=words[i];
        reverse(rwords[i].begin(),rwords[i].end());
    }
    for(int i=1;i<n;i++){
        // if(dp[i-1][0]!=NEG&&lex(words[i-1],words[i])){//前一位可以不翻转比较上一位不翻转和这一位不翻转满足吗
        //     dp[i][0]=dp[i-1][0];//满足就不用翻转
        // }else if(dp[i-1][0]!=NEG&&lex(words[i-1],rwords[i])){//比较上一位不翻转和这一位翻转满足吗
        //     dp[i][1]=dp[i-1][0]+energy[i];//上一位不翻转最小花费+自己这一位花费
        // }else if(dp[i-1][1]==NEG){//上一位不翻转，自己翻不翻转都不行，那么上一位必须翻转
        //     cout<<-1<<'\n';//如果上一位不能翻转，说明不能搞=排序
        //     return;
        // }else if(lex(rwords[i-1],words[i])){//上一位翻转是可以的,检验自己这一位要不要翻转
        //     dp[i][0]=dp[i-1][1];//从上一位翻转转移过来，因为自己不用转
        // }else if(lex(rwords[i-1],rwords[i])){//自己必须翻转
        //     dp[i][1]=dp[i-1][1]+energy[i];
        // }else{//前面翻不翻转自己这里都不行
        //     cout<<-1<<'\n';
        //     return;
        // }

        if(dp[i-1][0]!=NEG&&lex(words[i-1],words[i])){//前一位可以不翻转比较上一位不翻转和这一位不翻转满足吗
            dp[i][0]=dp[i-1][0];//满足就不用翻转
        }
        if(dp[i-1][1]!=NEG&&lex(rwords[i-1],words[i])){//比较上一位翻转和这一位不翻转满足吗
            if(dp[i][0]!=NEG){
                dp[i][0]=min(dp[i][0],dp[i-1][1]);
            }else dp[i][0]=dp[i-1][1];
            //上一位翻转最小花费
        }
        if(dp[i-1][0]!=NEG&&lex(words[i-1],rwords[i])){//前一位可以不翻转比较上一位不翻转和这一位翻转满足吗
            dp[i][1]=dp[i-1][0]+energy[i];//满足就不用翻转
        }
        if(dp[i-1][1]!=NEG&&lex(rwords[i-1],rwords[i])){//比较上一位翻转和这一位不翻转满足吗
            if(dp[i][1]!=NEG){
                dp[i][1]=min(dp[i][1],dp[i-1][1]+energy[i]);
            }else dp[i][1]=dp[i-1][1]+energy[i];//上一位翻转最小花费
            //上一位翻转最小花费
        }
        if(dp[i][0]==NEG&&dp[i][1]==NEG){
            cout<<-1<<'\n';
            return;
        }
    }
    if(dp[n-1][0]!=NEG&&dp[n-1][1]!=NEG) cout<<min(dp[n-1][0],dp[n-1][1])<<'\n';
    else if(dp[n-1][0]!=NEG) cout<<dp[n-1][0]<<'\n';
    else cout<<dp[n-1][1]<<'\n';

    return;
}

signed main(){
    ios::sync_with_stdio(false);
    cin.tie(0),cout.tie(0);
    int _=1;
    while(_--) solve();



    return 0;
}