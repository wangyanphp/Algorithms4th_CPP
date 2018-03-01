#include"edgeweightedgraph.h"
#include<fstream>
#include<iostream>
#include"3-5.cpp"

/**
 *4.3.23 Vyssotsky 方法
 *要求图可以删除边
 *
 */
class Graph4_3_23
{
private:
	int V_;
	int E_ = 0;
	std::vector<SET<Edge>> adj_;

public:
	typedef Bag<Edge>::Iterator Iterator;

	Graph4_3_23(int V) :V_(V), adj_(V) {}

	Graph4_3_23(std::istream &in)
	{
		in >> V_;
		adj_ = std::vector<SET<Edge>>(V_);
		int E;
		in >> E;

		for (int i = 0; i < E; ++i)
		{
			int v, w;
			double weight;
			in >> v >> w >> weight;
			addEdge(Edge(v, w, weight));
		}
	}

	int V() { return V_; }
	int E() { return E_; }

	void addEdge(const Edge &e)
	{
		int v = e.either(), w = e.other(v);

		int u = std::max(v, w);
		if (V_ - 1 < u)
		{
			for (int i = V_; i <= u; ++i)
			{
				++V_;
				adj_.push_back(SET<Edge>(Edge()));
			}
		}
		adj_[v].add(e);
		adj_[w].add(e);
		++E_;
	}

	void deleteEdge(const Edge &e)
	{
		int v = e.either(), w = e.other(v);
		adj_[v].deleteKey(e);
		adj_[w].deleteKey(e);
		--E_;
	}

	std::vector<Edge> adj(int v) { return adj_[v].toArray(); }

	std::vector<Edge> edges()
	{
		std::vector<Edge> ret;
		for (int v = 0; v < V_; ++v)
			for (auto e : adj(v))
				ret.push_back(e);
		return ret;
	}

	std::string toString()
	{
		std::string ret = std::to_string(V_) + " vertices, " + std::to_string(E_) + "edges\n";
		for (int v = 0; v < V_; ++v)
		{
			ret += std::to_string(v) + ": ";
			for (auto w : adj(v))
				ret += w.toString() + " ";
			ret += '\n';
		}
		return ret;
	}
};

class  RemoveCycle
{
private:
	std::vector<bool> marked_;
	std::vector<Edge> edgeTo_;
	std::vector<bool> onStack_;
	Stack<Edge> cycle_;

public:
	RemoveCycle(Graph4_3_23 &g) :marked_(g.V(), false), edgeTo_(g.V(), -1), onStack_(g.V(), false)
	{
		dfs(g, 0, 0);
		while (hasCycle())
		{
			Edge maxe;
			for (auto e : cycle_)
				if (e > maxe) maxe = e;

			g.deleteEdge(maxe);
			for (int v = 0; v < g.V(); ++v)
			{
				marked_[v] = false;
				onStack_[v] = false;
			}
			cycle_ = Stack<Edge>();
			dfs(g, 0, 0);
		}

	}

	bool hasCycle() { return !cycle_.isEmpty(); }

	std::vector<Edge> cycle()
	{
		std::vector<Edge> c;
		for (auto x : cycle_)
			c.push_back(x);
		return c;
	}

private:
	void dfs(Graph4_3_23 &g, int v, int u)
	{
		onStack_[v] = true;
		marked_[v] = true;

		for (Edge e:g.adj(v))
		{
			int w = e.other(v);
			if (hasCycle()) return;
			else if (!marked_[w])
			{
				edgeTo_[w] = e;
				dfs(g, w, v);
			}
			else if (onStack_[w]&& w != u)
			{
				cycle_.push(e);
				for (int x = v; x != w; x = edgeTo_[x].other(x))
					cycle_.push(edgeTo_[x]);
			}
		}
		onStack_[v] = false;
	}
};

/**
 *4.3.24 删除最大边：逆向删除算法
 */


/*

	//4.3.23用例
	std::ifstream in("E:\\tinyEWG.txt");
	Graph4_3_23 g(in);
	RemoveCycle rc(g);
	std::cout << g.toString() << std::endl;


*/	