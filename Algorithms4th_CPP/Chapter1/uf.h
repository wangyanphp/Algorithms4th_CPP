#pragma once
#include<vector>
class UF
{
private:
	std::vector<int> id;
	std::vector<int> sz;
	int cnt;

public:
	UF(int N) :cnt(N)
	{
		for (int i = 0; i < N; ++i)
		{
			id.push_back(i);
			sz.push_back(1);
		}
	}

	int count() const { return cnt; }
	bool connected(int p, int q) { return find(p) == find(q); }
	int find(int p)
	{
		while (p != id[p]) p = id[p];
		return p;
	}

	void unions(int p, int q)
	{
		int pRoot = find(p);
		int qRoot = find(q);
		if (pRoot == qRoot) return;

		if (sz[pRoot] < sz[qRoot])
		{
			id[pRoot] = qRoot;
			sz[qRoot] += sz[pRoot];
		}
		else
		{
			id[qRoot] = pRoot;
			sz[pRoot] += sz[qRoot];
		}
		--cnt;
	}
};
