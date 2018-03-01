#include"graph.h"
#include<fstream>
#include<iostream>

/**
 *4.1.35:边的联通性，检测桥
 *基于深度优先搜索
 *什么是桥：桥的所有连通结点都不会连到桥之前的点、
 *对于这一特性如何实现？用一个数组来记录访问顺序
 *那么桥之后的所有连通结点都不会连到桥之前的连通结点，这个如何来实现？
 *我们需要一种手段来记录。
 */

class Bridge
{
private:
	int bridges = 0;//桥的个数
	int cnt = 0;//标记访问顺序
	std::vector<int> order;//访问顺序
	std::vector<int> low;

public:

	Bridge(Graph &g) :order(g.V(),-1),low(g.V(),-1)
	{
		for (int s = 0; s < g.V(); ++s)
			if (order[s] == -1)
				dfs(g, s, s);
	}

private:
	void dfs(Graph &g, int v,int u)
	{
		order[v] = cnt++;
		low[v] = order[v];//初始化
		for (auto it = g.begin(v); it != g.end(v); ++it)
		{
			int w = *it;
			if (order[w] == -1)
			{
				dfs(g, w,v);
				low[v] = std::min(low[v], low[w]);
				if (low[w] == order[w])
				{
					std::cout << v << '-' << w << " is a bridge\n";
					++bridges;
				}
			}
			else if (w != u)
				low[v] = std::min(low[v], order[w]);
		}

	}
};
