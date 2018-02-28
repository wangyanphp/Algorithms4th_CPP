
/**
 *之前一直在说的：利用快速排序寻找第k小的元素的具体实现如下
 *其平均时间复杂度为线性，具体证明见221页
 */
template<typename It>
class Select
{
public:
	Select(It beg, It end, int k)
	{
		//随机化整个数组
		int lo = 0, hi = (end - beg) - 1;
		while (hi > lo)
		{
			It j_it = partition(beg + lo, beg + hi);
			int j = j_it - beg;
			if (j == k) return *(beg + j);
			else if (j > k) hi = j - 1;
			else if (j < k) lo = j + 1;
		}

		//return *(beg + k);
	}

private:

	It partition(It beg, It back)
	{
		It i = beg;
		It j = back + 1;//左右扫描指针,注意跟下面的递增递减相统一
		auto v = *beg;//切分元素


		while (true)
		{
			while (*++i < v)
			{
				if (i == back) break;
			}
			while (v < *--j)
			{
				if (j == beg) break;
			}
			if (i - j >= 0) break;

			std::swap(*i, *j);
		}
		std::swap(*beg, *j);
		return j;
	}

};


/**
 *本章的题目都值得一做，需要掌握2.5.12-2.5.30
 *2.5.32的A*算法还需要学习一下
 *重点是2.5.19 Kendall tau距离问题，之前好几次都没有做出来
 *争取这次掌握了
 */
/**
 * A,B之间的距离，根据题意得：如果A是完全正确的顺序，那么B相对于A来说的逆序数
 * A = 0 3 1 6 2 5 4; B = 1 0 3 6 4 2 5
 * 根据2.5.26的思路可以这样： order =“0316254”
 * index of B[i] = order.indexOf(b.charAt(i));
 * 由此可以得出一个A的顺序是：0123456;B相对于这一个顺序是xxxxxxx，然后求解逆序数
 *
 * 这里的一个问题是：A的内容是整数，这样可能会简化问题的实现方式（根据IndexMinPQ的经验）
 * A[0,1...6]={0,3,1,6,2,5,4}，对应的order0=1，order1=3....
 * 可以通过order来获得数字，但是我们需要的却是通过数字来获取order
 * AInv[0,3,1,6,2,5,4]={0,1...6}
 * 由此index of B[i] AInv[B[i]]
 */

#include<vector>
int kendallTau(const std::vector<int>& a, const std::vector<int>& b)
{
	std::vector<int> ainv(a.size());

	for (int i = 0; i < a.size(); ++i)
		ainv[a[i]] = i;

	std::vector<int> bmaped(b.size());

	for (int i = 0; i < b.size(); ++i)
		bmaped[i] = ainv[b[i]];

	//__Inversion<decltype(bmaped.begin()),int> count(bmaped.begin(), bmaped.end());
	//return count.count(bmaped.begin(), bmaped.end()-1);
	return 0;
}
