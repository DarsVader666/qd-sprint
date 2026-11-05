#include<iostream>
#include<climits>
#include<vector>
#include<queue>
#include<deque>  
using namespace std;
const int dir[5]={-1,0,1,0,-1};//四个方向顺序为左上右下
int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    int m,n,s_x,s_y,t_x,t_y;
    cin>>m>>n;
    deque<vector<int>> buffer;//双向队列
    vector<vector<vector<int>>> dist(m,vector<vector<int>>(n,vector<int>(4,INT_MAX)));//存距离
    vector<vector<char>> graph(m,vector<char>(n,'.'));//初始化图
    for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
            char temp;
            cin>>temp;
            graph[i][j]=temp;
            if(temp=='T'){
                t_x=i,t_y=j;
            }else if(temp=='S'){
                s_x=i,s_y=j;
            }
        }
    }
    for(int i=0;i<4;i++){
        dist[s_x][s_y][i]=0;//起点四个方向初始化为0
        buffer.push_front({s_x,s_y,i});//把起点放进去
    }
    int min_dist=INT_MAX;
    while(!buffer.empty()){
        vector<int> temp=buffer.front();
        int cur_x=temp[0],cur_y=temp[1],cur_dir=temp[2];
        buffer.pop_front();
        if(cur_x==t_x&&cur_y==t_y){
            for(int i=0;i<4;i++){
                min_dist=min(min_dist,dist[t_x][t_y][i]);
            }
            if(min_dist<=2) cout<<"YES";
            else cout<<"NO";
            return 0;
        }
        for(int j=0;j<4;j++){//要走的下一个点
            int nxt_x=cur_x+dir[j],nxt_y=cur_y+dir[j+1];
            if(nxt_x<0||nxt_x>=m||nxt_y<0||nxt_y>=n||graph[nxt_x][nxt_y]=='*') continue;//如果是障碍物或地图外，就跳过
            int weight = INT_MAX;
            weight=(cur_dir==j)?0:1;//方向相同无权重，不同则有权重
            if(dist[nxt_x][nxt_y][j]>dist[cur_x][cur_y][cur_dir]+weight){
                dist[nxt_x][nxt_y][j]=dist[cur_x][cur_y][cur_dir]+weight;//更新该点该方向的距离
                if(weight){
                    buffer.push_back({nxt_x,nxt_y,j});//权重为1放后面
                }else{
                    buffer.push_front({nxt_x,nxt_y,j});//权重为0放开头
                }
            }
        }
    }
    for(int i=0;i<4;i++){
        min_dist=min(min_dist,dist[t_x][t_y][i]);
    }
    if(min_dist<=2) cout<<"YES";
    else cout<<"NO";
    return 0;
}