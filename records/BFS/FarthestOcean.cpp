#include <iostream>
#include<vector>
#include<queue>
using namespace std;

struct point{
    int x,y,dist;
};
static const int dir[5]={-1,0,1,0,-1};//方向数组

int main()
{
	int n_max=1000,m_max=1000;
	int n,m;
	queue<point> buffer;
	cin>>m>>n;
    int num=0;//每一层出队数
    int dist=1;//当前距离
	vector<vector<int>> graph(m_max,vector<int>(n_max,0));
    vector<vector<int>> vis(m_max,vector<int>(n_max,0));//记录是否访问过
	for(int i=0;i<m;i++){//读取海绵与陆地同时入队陆地
	    for(int j=0;j<n;j++){
	        int temp=0;
	        cin>>temp;
    	    graph[i][j]=temp;
    	    
    	    if(temp==dist) {
                num++;//陆地
    	        point tempp{i,j,temp};
    	        buffer.push(tempp);
                vis[i][j]=1;//标记访问过
            }
	    }

	}
	
	if(buffer.size()==m*n) {cout<<-1;return 0;}//全是陆地
    else if(buffer.empty()) {cout<<-1;return 0;}//全是海洋
    else{
    
	while(!buffer.empty()&&num){
        int temp=0;
        dist++;//距离加一
        for(int i=0;i<num;i++){
            point p=buffer.front();
            buffer.pop();
            for(int j=0;j<4;j++){
                int x=p.x+dir[j],y=p.y+dir[j+1];
                if(x>=0&&x<m&&y>=0&&y<n&&!vis[x][y]){
                    buffer.push({x,y,dist});//入队近邻海洋
                    vis[x][y]=1;//标记访问过
                    temp++;//记录本层出队数
                }
            }
        }
        num=temp;//更新本层出队数
    }

    cout<<dist-2<<endl;
    return 0;
}