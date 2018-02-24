/**
为了达到期望的效果，我们需要设计一个数据结构来保存程序已知的所有整数对的足够信息，并用它来判断一对新对象是否是相连的。
我们将这个问题叫做动态连通性问题
*/

/*在设计算法的时候，面对的第一个问题就是精确地定义问题*/
/** API 
 * UF(int N) 以整数标识（0到N-1）初始化N个触点
 * void union(int p, int q) 在p和q之间添加一个连接
 * int find(int p) p所在分量的标识符（0到N-1）
 * bool connected(int p, int q) 如果p和q存在于同一个分量中则返回true
 * int count() 连通分量的个数
 */
#include<vector>
class UF1
{
private:
	std::vector<int> id; //分量id
	int cnt;//分量数量

public:
	UF1(int N) :cnt(N)
	{//初始化分量id数组
		for (int i = 0; i < N; ++i)
			id.push_back(i);
	}

	int count() const { return cnt; }

	bool connected(int p, int q) { return find(p) == find(q); }

	int find(int p);
	void unions(int p, int q);
	int find2(int p);
	void unions2(int p, int q);
};

/**
 *模型1：quick find模型
 *让所有的连通的值用同一个id
 */

int UF1::find(int p)
{
	return id[p];
}

void UF1::unions(int p, int q)
{
	int pID = find(p);
	int qID = find(q);

	if (pID == qID) return;

	for (int i = 0; i < id.size(); ++i)
		if (id[i] == pID) id[i] = qID;
	--cnt;
}

/**
 *分析：如果我们最后只得到一个连通分量，那么最少要调用N-1次unions，所以quick-find策略是平方级的
 *我们必须要改进
 */

/**
 *模型二：quick-union
 *采用树状结构来组织id，只需要有一个根触点
 */

int UF1::find2(int p)
{
	while (p != id[p]) p = id[p];
	return p;
}

void UF1::unions2(int p, int q)
{
	int pRoot = find2(p);
	int qRoot = find2(q);
	if (pRoot == qRoot) return;

	id[pRoot] = qRoot;
	--cnt;
}

/**
 *分析：根据树的定义：find访问数组的次数为1+节点的深度的两倍，unions和connected访问数组的次数为两次find操作
 *当最后得到一个连通分量的情况下：最坏情况下，是平方级别的。
 *是否有办法避免最坏情况的出现？（例如随机化）
 */

/**
 *模型3：加权的quick-union
 *不是随意的将一棵树连接到另一棵树，我们现在会记录每一棵树的大小，并将较小的树连接到较大的树上
 */
/*
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
*/
/**
 *分析：对于N触点，加权quick-union算法构造的森林中的任意节点的深度最多为lgN，用数学归纳法分析：如果i+j=k, 设i<=j,则小树的所有节点的深度增加
 *了1,1+lgi=lg(i+i)<=lg(i+j)=lgk
 *所以：在最坏情况下，find的增长数量级为logN
 *由此可见：加权quick-union算法是三种算法中唯一可以用于大型实际问题的算法
 */


/****************************************************************************************************************************
 ************************************************练习题***********************************************************************
 ****************************************************************************************************************************
 */

/**1.5.8,这种实现为什么会有错误？？
void union(int p, int q) {
if (connected(p, q)) return;
for (int i = 0; i < id.length; i++)
if (id[i] == id[p]) id[i] = id[q];
count--;
}
 *不仔细看，感觉时没有什么错误的，
 *错误在于：我们在for循环中更改了id[p],但程序暗含的假定是：循环中id[p]不发生改变
 */

//我们可以看到：如果经常使用find操作，那么路径压缩可以极大地提高查找效率
//1.5.13 :使用路径压缩的加权quick-union
class PathCompressionUF
{
private:
	std::vector<int> id;
	std::vector<int> sz;
	int cnt;

public:
	PathCompressionUF(int N) :cnt(N)
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
		int root = p;
		while (root != id[root]) root = id[root];

		//路径压缩
		while (p != root)
		{
			int father = id[p];
			id[p] = root;
			sz[father] -= sz[p];
			p = father;
		}
		return root;
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

//根据之前的算法，我们可以知道，只有根节点的size起作用，否则我们还需要维护中间节点的size，反而增大程序难度
//根据数据结构，因为id[root]=root;所以我们可以用id[root]来保存-size

/**
 *1.5.14：也可以根据树的高度来加权，总是将爱得树连接到高的树上
 *下面的练习题将实现上述两种想法
 */
class WeightedUFByHeight
{
private:
	std::vector<int> id;//id[root]存储的是树的高度
	int cnt;

public:
	WeightedUFByHeight(int N) :cnt(N)
	{
		for (int i = 0; i < N; ++i)
			id.push_back(-1);//-1代表的是高度0
	}

	int count() const { return cnt; }

	bool connected(int p, int q) { return find(p) == find(q); }

	int find(int p)
	{
		int root = p;
		while (id[root] > 0) root = id[root];

		//路径压缩,很难捕捉高度的变化，所以不实现，除非使用一个数组来保存height
		return root;

	}

	void unions(int p, int q)
	{
		int pRoot = find(p);
		int qRoot = find(q);

		if (pRoot == qRoot) return;

		if (-id[pRoot] > -id[qRoot])
		{
			//p树比较高
			id[qRoot] = pRoot;
			//q子树的加入并不能改变p子树的高度
		}
		else if (-id[pRoot] < -id[qRoot])
		{
			id[pRoot] = qRoot;
		}
		else
		{
			id[qRoot] = pRoot;
			--id[pRoot];
		}
	}
};

