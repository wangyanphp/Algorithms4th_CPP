#include"random.h"
#include"test.h"
#include<iostream>
#include<vector>

/**
 *threeSum: count triples that sum to 0
 */
template<typename It>
size_t threeSum(It beg, It end)
{
	int N = (end - beg);
	size_t cnt = 0;
	for (int i = 0; i < N; ++i)
		for (int j = i + 1; j < N; ++j)
			for (int k = j + 1; k < N; ++k)
				if (*(beg+i) + *(beg+j) + *(beg+k) == 0)
					++cnt;
	return cnt;
}

double timeTrial(int N)
{
	static Random r;
	int MAX = 1000000;
	std::vector<int> v;
	for (int i = 0; i < N; ++i)
		v.push_back(r.uniformInt(-MAX, MAX));

	Stopwatch timer;
	size_t cnt = threeSum(v.begin(), v.end());
	return timer.elapsedTime();	
}


/**
 *二分查找,返回key的rank
 *必然要求能随机访问
 */
template<typename T,typename It>
size_t binarySearch(It beg, It end, T key) 
{
	int lo = 0;
	int hi = (end - beg) - 1;
	while (lo <= hi)
	{
		int mid = lo + (hi - lo) / 2;
		if (key < *(beg + mid)) hi = mid - 1;
		else if (key>*(beg + mid)) lo = mid + 1;
		else return mid;
	}

	//找不到，返回-1
	return -1;
}

/**
 *热身运动2-sum:
 */
template<typename It>
size_t twoSumFast(It beg, It end)
{
	//先排序，为了使用binary search
	std::sort(beg, end);

	int N = (end - beg);
	int cnt = 0;
	/**
	 *关于if条件的一点解释：等价于binarySearch(-a[i],a)>i
	 *如果二分查找找不到则返回-1.我们不会增加计数器的值
	 *如果二分查找返回的是j>i,则a[j]+a[i]=0，增加计数器的值
	 *如果二分查找找到的j在0和i之间，则避免重复计数
	 */
	for (int i = 0; i < N; ++i)
		if (binarySearch(beg + i + 1, end,-*(beg+i)))
			++cnt;
	return cnt;
}

template<typename It>
size_t threeSumFast(It beg, It end)
{
	std::sort(beg, end);

	int N = (end - beg);
	int cnt = 0;
	for (int i = 0; i < N; ++i)
		for (int j = i + 1; j < N; ++j)
			if (binarySearch(beg + j + 1, end, -*(beg + i) - *(beg + j)))
				++cnt;
	return cnt;
}
/**************************************************************************************************************************
 ************************************************练习题********************************************************************
 *************************************************************************************************************************/
//1.4.2:int相加溢出的情况
//当x和y都是负数，结果是非负数（包含0），那么就是负溢出；如果x和y都是非负数，结果是负数，那么就是正溢出。

//1.4.10
//返回和查找的键匹配的索引最小的元素
template<typename T, typename It>
size_t _binarySearch(It beg, It end, T key)
{
	int lo = 0;
	int hi = (end - beg) - 1;
	while (lo <= hi)
	{
		int mid = lo + (hi - lo) / 2;
		if (key < *(beg + mid)) hi = mid - 1;
		else if (key>*(beg + mid)) lo = mid + 1;
		else if (key == *(beg + mid) && mid != 0 && *(beg+mid-1)==key) hi = mid;//如果没有最后一个条件，那么容易在最后关头陷入死循环 
		else return mid;
	}

	//找不到，返回-1
	return -1;
}

//1.4.12：应该是Merge

/**
 *1.4.15：使用线性级别的算法twoSum计算已排序的数组中和为0的整数对
 *假设每个数字只能使用一次：-2,2,2只有一个整数对
 */
template<typename It>
size_t twoSumFaster(It beg, It end)
{
	//已排序

	int N = (end - beg);
	int cnt = 0;

	auto left = beg;
	auto right = end - 1;
	while (left < right)
	{
		if (*right>-*left) --right;
		else if (*right < -*left) ++left;
		else
		{
			++cnt;
			++left;
			--right;
		}
	}
	return cnt;
}

/**
 *1.4.16和1.4.17：最接近的一对的值和最遥远的一对的值
 */

std::pair<double, double> distance(std::vector<double>& v)
{
	//排序
	std::sort(v.begin(), v.end());
	double min = INT_MAX, max = -INT_MAX;
	for (int i = 0; i < v.size() - 1; ++i)
	{
		if (v[i + 1] - v[i] < min)
			min = v[i + 1] - v[i];

		if (v[i + 1] - v[i] > max)
			max = v[i + 1] - v[i];
	}
	return{ min,max };
}

/**
 *1.4.18 & 1.4.19 :查找数组的局部最小值
 *对于一维数组，最坏情况下比较次数是~2lgN
 *检查数组的中间值a[N/2]以及相邻的元素a[N/2-1] & a[N/2+1]。如果a[N/2]是一个局部最小值则算法终止；否则在较小相邻边中继续查找-->lgN
 *如果仍然查不到，则查询另外半边-->lgN
 *对于二维数组：最坏情况下，与N成正比。
 *首先第N/2行上的最小值a[N/2][j]，比较 a[N/2-1][j] &a[N/2+1][j]. 
 *Recur in the half with the smaller neighbor. 
 *In that half, find the minimum entry in column N/2.
 *N+N/2+N/4...~N
 */


/**
 *1.4.20:双调查找：第一步查找最大值，第二步在左侧查找；第三步在右侧查找，比较次数~3lgN
 */

template<typename T, typename It>
bool searchOfBitonic(It beg, It end, T key)
{
	//find max elements
	//假设必然存在，所以不用考虑边缘的检测
	int lo = 0;
	int hi = (end - beg) - 1;
	int mid = 0;
	while (lo <= hi)
	{
		mid = lo + (hi - lo) / 2;
		if (*(beg + mid) < *(beg + mid + 1)) lo = mid + 1;
		else if (*(beg + mid + 1) < *(beg + mid)) hi = mid - 1;
		else break;
	}

	int ret = -1;
	if ((ret = binarySearch(beg, beg + mid + 1, key)) != -1)
		return true;
	if ((ret = binarySearch(beg + mid + 1, end, key)) != -1)
		return true;

	return false;
}

/**
 *仅用加减法来实现二分查找：Mihai Patrascu
 *使用 Fibonacci numbers来代替二分法
 *这样做有没有好处呢？只不过是用加减法来代替了除以2的操作而已啊，完全可以用<<左移来实现除以2操作
 *这里的重点是放在Fibnocci与power of 2的相似性比较上：例如二叉树与Fibnocci树的比较
 *参考：http://blog.csdn.net/zsw12013/article/details/50003505
 */
template<typename T, typename It>
int fibnocciSearch(It beg, It end, T key)
{
	//排序
	std::vector<decltype(*It)> v(beg, end);
	std::sort(v.begin(), v.end());

	int N = v.size();
	//首先确定需要用到的Fibnocci number
	std::vector<int> f{ 1,1 };
	while (f.back() < N)
		f.push_back(f[f.size() - 1] + f[f.size() - 2]);

	//补齐数组
	for (int i = v.size(); v < f.back(); ++i)
		v.push_back((T)INT_MAX);//填充元素

	//因为我们使用的是数组，所以必须时刻记住，迭代的层次
	//我们使用k来代表当前要搜索范围的Fibnocci 数的索引
	int k = f.size()-1;

	int lo = 0, hi = v.size() - 1;
	while (lo <= hi)
	{
		//对应的mid元素:
		mid = lo + f[k - 1] -1;
		if (v[mid] < key)
		{
			lo = mid + 1;
			//此时的查找长度为f[k-2]
			k -= 2;
		}
		else if (v[mid]>key)
		{
			hi = mid;
			//此时的查找长度为f[k-1]
			k -= 1;
		}
		else
		{
			if (mid > N - 1) return -1;
			return mid;
		}
	}

	return -1;
}

/**
 *1.4.25:扔鸡蛋问题：N层大楼和许多鸡蛋，假设F层是一个临界点，如何找到F？
 *lgN次的话，可以用二分法查找。
 *能不能改进呢？
 *我们知道：层数低的话，鸡蛋不会碎，所以1,2,4,8....这样查找到F∈区间[c,2c]
 *区间长度小于F，在此区间二分查找F，总共~2lgF次查找
 *
 *假设现在我们只有两个鸡蛋，此情况对应于：查找命中比未命中的成本小的多的情况
 *如何查找？
 *肯定需要一个一层层实验的过程。一个碎鸡蛋来缩小范围，另一个鸡蛋来一层层查找
 *~2sqrt（N)的话：
 *To achieve 2 * sqrt(N), drop eggs at floors sqrt(N), 2 * sqrt(N), 3 * sqrt(N), ..., sqrt(N) * sqrt(N). 
 *(For simplicity, we assume here that sqrt(N) is an integer.) 
 *Let assume that the egg broke at level k * sqrt(N).
 *With the second egg you should then perform a linear search in the interval (k-1) * sqrt(N) to k * sqrt(N). 
 *In total you will be able to find the floor F in at most 2 * sqrt(N) trials.
 *如何实现~csqrt（F）次呢？
 *http://www.cnblogs.com/ltang/archive/2010/11/23/1885791.html
 */


