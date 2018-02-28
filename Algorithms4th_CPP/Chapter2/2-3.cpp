/**************************************************************************************************************************
**********************************************练习题************************************************************************
**************************************************************************************************************************/
#include"sort.h"
#include"../Chapter1/test.h"
//2.3.5:只有两种主键值的数组排序，可以模仿3-way排序
//不过是递归深度多了一层而已
template<typename T>
void sort2distinct(std::vector<T>& a)
{
	int lt = 0, gt = a.size() - 1;
	//[0,lt-1],[lt,i-1][i,gt],[gt+1,hi]
	int i = lt+1;
	T v = a[0];
	while (i <= gt)
	{
		if (a[i] < v)
			std::swap(a[i++], a[lt++]);//为什么要交换呢？根据之前的定义，lt是等于v的，所以，要腾出空间来给v
		else if (v < a[i])
			std::swap(a[i], a[gt--]);
		else
			++i;
	}
	//跟之前的quick sort中（i，j）方式相比，多了很多不必要的交换，这是因为我们无法预测，各区间的长度，只能如此
	//由此，我认为没有重复键的情况下，还是二分法好啊
}


//2.3.15:螺丝和螺帽
//根据快速排序：先取螺丝K为切分元素，切分螺帽，同时找出与螺丝K匹配的螺帽K'；
//根据螺帽K'切分螺丝
//重复上述步骤


/**
 *2.3.16:最佳情况，如何生成对于sort来说，表现最佳的数组
 *首先看看能不能使用分治法
 *如果前面一般是最好的，后面一般是最好的，那么能不能->整体上最好的？
 *或者整体分成两部分最好的，能不能每个部分都是最好的？
 *分治的两种思路
 */
void __best(std::vector<int>& a, int lo, int hi)
{
	if (hi <= lo) return;
	int mid = lo + (hi - lo)/2;//可以将数组一分为二的切分元素
	__best(a, lo, mid - 1);
	__best(a, mid + 1, hi);
	std::swap(a[lo], a[mid]);
}
std::vector<int> best(int N)
{
	std::vector<int> ret(N);
	for (int i = 0; i < N; ++i)
		ret[i] = i;
	__best(ret, 0, N - 1);
	return ret;
}


/**
 *2.3.17 哨兵——除去边界检测
 *回想一下插入排序的哨兵：将最小元素放到最边界
 *这里也是通过哨兵元素设立边界
 *左侧边界检测是多余的：即：切分元素本身就是一个哨兵（v不可能小于a[lo]）
 *如何设立右侧哨兵？
 */
/**
 *我们唯一的改动就是把最大元素放置到整个数组的最右侧，这样就可以保证i不会越界
 *那么对于子数组呢？最右边的子数组肯定是没有问题的
 *左子数组右侧哨兵是什么呢？这里是原址排序,对于之前插入排序的坏处反而成了好处
 *我们可以访问超过子数组边界的元素
 *对于左子数组，它的下一个元素必然是大于左子数组中的任意元素的！我们可以使用这一特性来作为哨兵
 *问题是：这样的哨兵正确吗？
 *1.会不会改变“超出边界的右侧哨兵”？
 *当i==hi+1时，必然有i>=j,所以不会更改
 *当i==hi+1或j==lo时，必然是要退出总循环的，所以我们可以在内循环中使用while (*++i < v)这样的语句，因为只要i==lo了，必然会跳出总循环
 *而不用担心，接下来的循环会再次增加i
 */
template<typename It>
class __QuickWithSentry
{
public:
	void sort(It beg, It end)
	{
		static Random r;
		r.shuffle(beg, end);//随机化，消除对输入的依赖

		//将最大元素移到最右侧，作为哨兵
		auto max = beg;
		for (auto it = beg + 1; it != end; ++it)
		{
			if (*max < *it)
				max = it;
		}
		std::swap(*max, *(end - 1));

		__sort(beg, end - 1);
	}
private:
	void __sort(It beg, It back)
	{
		if (back - beg <= 0) return;
		auto j = partition(beg, back);
		if (j != beg)
			__sort(beg, j - 1);
		if (j != back)
			__sort(j + 1, back);
	}


	It partition(It beg, It back)
	{
		It i = beg;
		It j = back + 1;
		auto v = *beg;

		while (true)
		{//扫描元素
			while (*++i < v);
			while (v < *--j);
			//当指针i和j相遇时退出主循环
			if (i - j >= 0) break;

			std::swap(*i, *j);
		}
		//有*j不大于v，而*i不小于v，根据184页图例
		std::swap(*beg, *j);
		return j;
	}

};

template<typename It>
void quickSortWithSentry(It beg, It end)
{
	static __QuickWithSentry<It> q;//static是为了random
	q.sort(beg, end);
}


/**
 *2.3.18 三取样切分，五取样切分
 *之后结合STL源代码实现
 *寻找第K大的元素，正好可以借助于quicksort算法，因为此算法正好可以寻找第K大元素
 */

/**
 *2.3.20：非递归的快速排序
 *我本来的想法是：我们先保存没有排序的区间？为什么呢，想一想栈的执行过程
 *具体见书中附件
 *也可以使用另一种，网上广为流传的算法实现
 */
#include"../Chapter1/stack.h"
template<typename It>
class __QuickNonrecursive
{
public:
	void sort(It beg, It end)
	{
		static Random r;
		//r.shuffle(beg, end);//随机化，消除对输入的依赖
		__sort(beg, end - 1);
	}
private:
	void __sort(It beg, It back)
	{
		Stack<It> stack;
		
		if (back - beg <= 0) return;
		stack.push(beg);
		stack.push(back);
		
		//开始执行：栈中的数组都没有排序
		while (!stack.isEmpty())
		{
			auto hi = stack.pop();
			auto lo = stack.pop();

			if (hi <= lo) continue;

			auto mid = partition(lo, hi);
			if (lo < mid - 1)
			{
				stack.push(lo);
				stack.push(mid - 1);
			}

			if (mid + 1 < hi)
			{
				stack.push(mid + 1);
				stack.push(hi);
			}
		}
	}


	It partition(It beg, It back)
	{
		It i = beg;
		It j = back + 1;//左右扫描指针,注意跟下面的递增递减相统一
		auto v = *beg;//切分元素

					  //循环不变式是：i左侧的元素都不大于v，j右侧的元素都不小于v
		while (true)
		{//扫描元素
			while (*++i < v)
			{
				if (i == back) break;
			}
			while (v < *--j)
			{
				if (j == beg) break;
			}
			//当指针i和j相遇时退出主循环
			if (i - j >= 0) break;

			using std::swap;
			swap(*i, *j);
		}
		//有*j不大于v，而*i不小于v，根据184页图例
		using std::swap;
		swap(*beg, *j);
		return j;
	}

};

template<typename It>
void quickSortNonrecursive(It beg, It end)
{
	static __QuickNonrecursive<It> q;//static是为了random
	q.sort(beg, end);
}



#include<iostream>
double time2_3(const std::string& alg, std::vector<double>& v)
{
	Stopwatch timer;
	auto beg = v.begin();
	auto end = v.end();
	if (alg == "Merge") mergeSort(beg, end);
	else if (alg == "MergeX") mergeSortX(beg, end);
	else if (alg == "InsertionX") insertionSortX(beg, end);
	else if (alg == "Quick") quickSort(beg, end);
	else if (alg == "Quick3Way") quick3WaySort(beg, end);
	else if (alg == "QuickNon") quickSortNonrecursive(beg, end);
	else if (alg == "heap") heapSort(beg, end);
	
	

	return timer.elapsedTime();
}

//int main()
//{
//	int N = 10000;
//	int T = 100;
//	double t1 = timeRondomInput(time2_3, "MergeX", N, T);
//	double t2 = timeRondomInput(time2_3, "heap", N, T);
//	double t3 = timeRondomInput(time2_3, "Quick", N, T);
//	double t4 = timeRondomInput(time2_3, "Quick3Way", N, T);
//	double t5 = timeRondomInput(time2_3, "QuickNon", N, T);
//	std::cout << t1 << '\t' << t2 << '\t' << t1 / t2 << '\n';
//	std::cout << t2 << '\t' << t3 << '\t' << t2 / t3 << '\n';
//	std::cout << t2 << '\t' << t4 << '\t' << t2 / t4 << '\n';
//	std::cout << t2 << '\t' << t5 << '\t' << t2 / t5 << '\n';
//	system("pause");
//}
