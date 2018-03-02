#pragma once

/**
 *一般情况下来说，graph采用邻接表来实现，但是在其他情况下，也有可能会采用其他的数据结构。具体参加P337
 *
 */
#include"../Chapter1/bag.h"
#include<vector>
#include<algorithm>
class Graph
{

private:

	int V_;
	int E_ = 0;
	std::vector<Bag<int>> adj;

public:
	typedef Bag<int>::Iterator Iterator;

	Graph() :V_(0) {}
	Graph(int v) :V_(v), adj(v) {}

	Graph(std::istream &in)
	{
		in >> V_;
		adj = std::vector<Bag<int>>(V_);
		int E;
		in >> E;

		for (int i = 0; i < E; ++i)
		{
			int v, w;
			in >> v >> w;
			addEdge(v, w);
		}
	}

	int V() { return V_; }
	int E() { return E_; }

	void addEdge(int v, int w)
	{
		int u = std::max(v, w);
		if (V_-1 < u)
		{
			for (int i = V_; i <= u; ++i)
			{
				++V_;
				adj.push_back(Bag<int>());
			}
		}
		++E_;
		adj[v].add(w);
		adj[w].add(v);
	}

	Iterator begin(int v) { return adj[v].begin(); }
	Iterator end(int v) { return adj[v].end(); }

	std::string toString()
	{
		std::string ret = std::to_string(V_) + " vertices, " + std::to_string(E_) + "edges\n";
		for (int v = 0; v < V_; ++v)
		{
			ret += std::to_string(v) + ": ";
			for (auto w : adj[v])
				ret += std::to_string(w) + " ";
			ret += '\n';
		}

		return ret;
	}
};


/**
 *常用算法1：Search：判断与s点连通的点，与s点连通的点的个数
 *如果单纯是这个目的的话，不如构建Union-Find来得快
 *但是，此处使用DFS来实现
 */
#include"../Chapter1/queue.h"
class Search
{
private:
	Graph &graph;
	int s_;

	std::vector<bool> marked_;
	int count_ = 0;

public:

	Search(Graph& g, int s) :graph(g), s_(s), marked_(g.V(), false)
	{
		dfs(graph, s);
	}

	bool merked(int v) { return marked_[v]; }
	int count() { return count_; }

private:

	void dfs(Graph& g, int v)
	{
		marked_[v] = true;
		++count_;

		for (auto it = g.begin(v); it != g.end(v); ++it)
			if (!marked_[*it]) dfs(g, *it);
	}

	void bfs(Graph &g, int s)
	{
		Queue<int> q;
		q.enqueue(s);

		while (!q.isEmpty())
		{
			int v = q.dequeue();
			marked_[v] = true;
			++count_;
			for (auto it = g.begin(v); it != g.end(v); ++it)
				if (!marked_[*it]) q.enqueue(*it);
		}
	}
};

/**
 *常用算法2：寻找路径
 *路径就不能用union-find来寻找了，还是得使用搜索策略
 */
#include"../Chapter1/stack.h"
class Path
{
private:
	Graph &graph;
	int s_;

	std::vector<bool> marked_;
	std::vector<int> edgeTo_;

public:

	Path(Graph &g, int s) :graph(g), s_(s), marked_(g.V(), false), edgeTo_(g.V()) 
	{
		dfs(graph, s_);
	}

	bool hasPathTo(int v)
	{
		return marked_[v];
	}

	std::vector<int> pathTo(int v)
	{
		std::vector<int> ret;
		if (!hasPathTo(v)) return ret;
		Stack<int> stack;
		for (int x = v; x != s_; x = edgeTo_[v])
			stack.push(x);
		stack.push(s_);

		while(!stack.isEmpty())
			ret.push_back(stack.pop());
		return ret;
	}

private:

	void dfs(Graph &g, int v)
	{
		marked_[v] = true;

		for (auto it = g.begin(v); it != g.end(v); ++it)
			if (!marked_[*it])
			{
				edgeTo_[*it] = v;
				dfs(g, *it);
			}
	}
};

/**
 *常用算法3：连通分量
 *这里又可以使用union-find算法
 *相比于DFS，union-find是一个动态算法
 */
class CC
{
private:
	Graph &graph;

	std::vector<bool> marked_;
	std::vector<int> id_;
	int count_ = 0;

public:

	CC(Graph &g) :graph(g), marked_(g.V(), false), id_(g.V())
	{
		for (int s = 0; s < graph.V();++s)
			if (!marked_[s])
			{
				dfs(g, s);
				++count_;
			}
	}

	bool connected(int v, int w)
	{
		return id_[v] == id_[w];
	}

	int id(int v)
	{
		return id_[v];
	}

	int count()
	{
		return count_;
	}

private:

	void dfs(Graph &g, int v)
	{
		marked_[v] = true;
		id_[v] = count_;
		for (auto it = g.begin(v); it != g.end(v); ++it)
			if (!marked_[*it]) dfs(g, *it);
	}

};

/**
 *常用算法4：检测环
 *Graph是不是无环图？
 *检测原理：每个点只能有一个父节点
 */

class Cycle
{
private:
	Graph &graph;

	std::vector<bool> marked_;
	bool hasCycle_ = false;

public:
	Cycle(Graph &g) :graph(g), marked_(g.V(), false)
	{
		for (int s = 0; s < g.V(); ++s)
			if (!marked_[s]) dfs(graph, s, s);
	}

	bool hasCycle() { return hasCycle_; }

private:
	void dfs(Graph &g, int v, int u)
	{
		marked_[v] = true;
		for (auto it = g.begin(v); it != g.end(v); ++it)
			if (!marked_[*it]) dfs(g, *it,v);
			else if (*it != u) hasCycle_ = true;
	}

};

/**
 *常用算法5：二分图检查
 */

class TwoColor
{
private:
	Graph &graph;
	std::vector<bool> marked_;
	std::vector<bool> color_;
	bool isTwoColorable_ = true;

public:
	TwoColor(Graph &g) :graph(g), marked_(g.V(), false), color_(g.V(), false)
	{
		for (int s = 0; s < g.V(); ++s)
			if (!marked_[s]) dfs(graph, s);
	}

	bool isBipartite() { return isTwoColorable_; }

private:
	void dfs(Graph &g, int v)
	{
		marked_[v] = true;
		for (auto it = g.begin(v); it != g.end(v); ++it)
			if (!marked_[*it])
			{
				color_[*it] = !color_[v];
				dfs(g, *it);
			}
			else if (color_[*it] == color_[v]) isTwoColorable_ = false;
	}
};

/**
 *常用算法6：符号图
 */
#include"../Chapter3/ST2017.h"
class SymbolGraph
{
private:
	RedBlackBST<const std::string,int> st;//符号名->索引
	std::vector<std::string> keys;//索引->符号名
	Graph graph;

public:
	SymbolGraph(std::istream &in) :st("",-1)
	{
		std::string v, w;
		while (in >> v >> w)
		{
			addEdge(v, w);
		}
	}

	bool contains(const std::string &s) { return st.contains(s); }
	int index(const std::string &s) { return st.get(s); }
	std::string name(int v) { return keys[v]; }
	Graph G() { return graph; }

	void addEdge(const std::string &v, const std::string &w)
	{
		if (!st.contains(v))
		{	
			st.put(v, st.size());
			keys.push_back(v);
		}
		
		if (!st.contains(w))
		{
			st.put(w, st.size());
			keys.push_back(w);
		}
			
		graph.addEdge(st.get(v), st.get(w));
	}
	
};



