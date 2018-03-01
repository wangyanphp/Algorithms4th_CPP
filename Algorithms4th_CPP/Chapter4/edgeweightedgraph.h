#pragma once
#include<vector>
#include<string>
class Edge
{
private:
	int v_;//顶点1
	int w_;//顶点2
	double weight_;//边的权重

public:
	Edge(int v=-1, int w=-1, double weight=0) :v_(v), w_(w), weight_(weight) {}

	double weight() const { return weight_; }

	int either() const { return v_; }

	int other(int v) const
	{
		if (v == v_) return w_;
		else if (v == w_) return v_;
	}

	bool operator<(const Edge &e) const { return weight_ < e.weight_; }

	bool operator==(const Edge &e) const { return weight_ == e.weight_; }

	bool operator>(const Edge &e) const { return weight_ > e.weight_; }
	
	bool operator!=(const Edge &e) const{ return weight_ != e.weight_; }

	bool operator>=(const Edge &e) const { return weight_ >= e.weight_; }

	bool operator<=(const Edge &e) const { return weight_ <= e.weight_; }

	std::string toString() const
	{
		std::string ret = std::to_string(v_) + '-' + std::to_string(w_) + ' ';
		std::string wei = std::to_string(weight_);
		wei = wei.substr(0,wei.find('.') + 3);
		return ret + wei;

	}
};

#include"bag.h"
#include<istream>
#include<algorithm>
class EdgeWeightedGraph
{
private:
	int V_;
	int E_ = 0;
	std::vector<Bag<Edge>> adj_;

public:
	typedef Bag<Edge>::Iterator Iterator;

	EdgeWeightedGraph(int V) :V_(V), adj_(V) {}

	EdgeWeightedGraph(std::istream &in)
	{
		in >> V_;
		adj_ = std::vector<Bag<Edge>>(V_);
		int E;
		in >> E;

		for (int i = 0; i < E; ++i)
		{
			int v, w;
			double weight;
			in >> v >> w >> weight;
			addEdge(Edge(v,w,weight));
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
				adj_.push_back(Bag<Edge>());
			}
		}
		adj_[v].add(e);
		adj_[w].add(e);
		++E_;
	}

	Iterator begin(int v) { return adj_[v].begin(); }
	Iterator end(int v) { return adj_[v].end(); }

	std::vector<Edge> edges()
	{
		std::vector<Edge> ret;
		for (int v = 0; v < V_; ++v)
			for (auto e : adj_[v])
				ret.push_back(e);
		return ret;
	}

	std::string toString()
	{
		std::string ret = std::to_string(V_) + " vertices, " + std::to_string(E_) + "edges\n";
		for (int v = 0; v < V_; ++v)
		{
			ret += std::to_string(v) + ": ";
			for (auto w : adj_[v])
				ret += w.toString() + " ";
			ret += '\n';
		}
		return ret;
	}	
};

#include"maxpq.h"
class LazyPrimMST
{
private:
	std::vector<bool> marked_;
	std::vector<Edge> mst_;
	MinPQ<Edge> pq_;
	double weight_ = 0;

public:
	LazyPrimMST(EdgeWeightedGraph &g) :marked_(g.V(), false)
	{
		visit(g, 0);
		while (!pq_.isEmpty())
		{
			Edge e = pq_.delMin();

			int v = e.either(), w = e.other(v);
			if (marked_[v] && marked_[w])continue;
			mst_.push_back(e);
			weight_ += e.weight();

			if (!marked_[v]) visit(g, v);
			if (!marked_[w]) visit(g, w);
		}
	}

	std::vector<Edge> edges() { return mst_; }
	double weight() { return weight_; }

private:
	void visit(EdgeWeightedGraph &g, int v)
	{
		marked_[v] = true;
		for (auto it = g.begin(v); it != g.end(v); ++it)
		{
			if (!marked_[it->other(v)]) pq_.insert(*it);
		}
	}
};

class PrimMST
{
private:
	std::vector<Edge> edgeTo_;
	std::vector<double> distTo_;
	std::vector<bool> marked_;

	IndexMinPQ<double> pq_;

public:
	PrimMST(EdgeWeightedGraph &g) :edgeTo_(g.V()), distTo_(g.V(), INT_MAX), marked_(g.V(), false), pq_(g.V())
	{
		distTo_[0] = 0;
		pq_.insert(0, 0);
		while (!pq_.isEmpty())
		{
			auto wei = pq_.minKey();
			auto v = pq_.delMin();
			visit(g, v);
		}
			
	}

	std::vector<Edge> edges() { return edgeTo_; }
	double weight()
	{
		double we = 0;
		for (auto e : edgeTo_)
			we += e.weight();
		return we;
	}

private:
	void visit(EdgeWeightedGraph &g, int v)
	{
		marked_[v] = true;

		for (auto it = g.begin(v); it != g.end(v); ++it)
		{
			int w = it->other(v);
			if (marked_[w]) continue;
			if (it->weight() < distTo_[w])
			{
				edgeTo_[w] = *it;
				distTo_[w] = it->weight();

				if (pq_.contains(w)) pq_.changeKey(w, distTo_[w]);
				else pq_.insert(w, distTo_[w]);
			}
		}
	}

};
#include"uf.h"
class KruskalMST
{
private:
	std::vector<Edge> mst_;

public:
	KruskalMST(EdgeWeightedGraph &g)
	{
		auto edges = g.edges();
		MinPQ<Edge> pq(edges.begin(), edges.end());
		UF uf(g.V());

		while(!pq.isEmpty() && mst_.size()<g.V()-1)
		{
			auto e = pq.delMin();
			int v = e.either(), w = e.other(v);
			if (uf.connected(v, w)) continue;
			uf.unions(v, w);
			mst_.push_back(e);
		}
	}

	std::vector<Edge> edges() { return mst_; }

	double weight()
	{
		double we = 0;
		for (auto e : mst_)
			we += e.weight();
		return we;
	}

	bool check(EdgeWeightedGraph &g)
	{
		//check1:生成树是否是一棵树（无环、连通）
		UF uf(g.V());
		//是否有环
		for (auto e : mst_)
		{
			int v = e.either(), w = e.other(v);
			if (uf.connected(v, w))
				return false;
			uf.unions(v, w);
		}
		//是否连通
		for (auto e : g.edges())
		{
			int v = e.either(), w = e.other(v);
			if (!uf.connected(v, w))
				return false;
		}


		//check2:是否是最小生成树
		for (auto e : mst_)
		{
			//删除e
			uf = UF(g.V());
			for (auto f : mst_)
			{
				int x = f.either(), y = f.other(x);
				if (f != e) uf.unions(x, y);
			}

			//e是否是横切边中最小的边
			for (auto f : g.edges())
			{
				int x = f.either(), y = f.other(x);
				if (!uf.connected(x, y))
					if (f.weight() < e.weight())
						return false;
			}
		}

		return true;
	}
};