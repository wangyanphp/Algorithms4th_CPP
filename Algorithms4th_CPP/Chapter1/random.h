#pragma once
#include<random>
#include<exception>

/**
*  The {@code StdRandom} class provides static methods for generating
*  random number from various discrete and continuous distributions,
*  including Bernoulli, uniform, Gaussian, exponential, pareto,
*  Poisson, and Cauchy. It also provides method for shuffling an
*  array or subarray.
*  <p>
* API
* void setSeed(long seed); 设置种子
* int uniformInt(int N); 0到N之间的整数
* int uniformInt(int lo, int hi); lo到hi的整数
* double uniform();0到1的实数
* double uniform(double lo, double hi); lo到hi之间的实数
* bool bernoulli(double p); 返回真的概率为p
* double gaussian(); 标准正态分布
* double gaussian(double m, double s); 正态分布，期望值为m，标准差为s
* int discrete(double[] a); 返回i的概率为a[i]
* void shuffle(double[] a); 将数组a随机排序
*/

class Random
{
private:
	std::default_random_engine e;//引擎

public:
	void setSeed(long s) { e.seed(s); }

	/**
	 *随机返回整数
	 */
	int uniformInt(int N)
	{
		if (N <= 0)
			throw std::invalid_argument("Parameter N must be positive");
		std::uniform_int_distribution<unsigned> u(0, N);
		return u(e);
	}

	int uniformInt(int lo, int hi)
	{
		if (hi<lo || (long)(hi - lo)>INT_MAX)
			throw std::invalid_argument("range error");
		std::uniform_int_distribution<int> u(lo, hi);
		return u(e);
	}

	/**
	 *随机分布：返回实数
	 */
	double uniform()
	{
		std::uniform_real_distribution<double> u(0, 1);
		return u(e);
	}

	double uniform(double lo, double hi)
	{
		if (lo >= hi)
			throw std::invalid_argument("range error");
		std::uniform_real_distribution<double>u(lo, hi);
		return u(e);
	}

	/**
	 *bornoulli分布：返回true的概率为p
	 */
	bool bernoulli(double p=0.5)
	{
		if (p <= 0 || p >= 1)
			throw std::invalid_argument("Probability must be between 0.0 and 1.0");
		std::bernoulli_distribution b(p);
		return b(e);
	}

	/**
	 *gaussian分布：返回实数
	 */
	double gaussian()
	{
		std::normal_distribution<double> n;
		return n(e);
	}

	double gaussian(double m, double s)
	{
		std::normal_distribution<double> n(m, s);
		return n(e);
	}

	/**
	 *i出现的概率是a[i]
	 *使用轮盘策略
	 */
	template<typename It>
	int discrete(It beg, It end)
	{
		double r = uniform();
		double sum = 0;
		for (suto it = beg; it != end; ++it)
		{
			sum += *it;
			if (sum >= r)return (it - beg);
		}
		return -1;
	}

	/**
	 *随机排序
	 */
	template<typename It>
	void shuffle(It beg, It end)
	{
		int N = (end - beg);
		for (int i = 0; i < N; ++i)
		{
			int r = uniformInt(i, N - 1);
			using std::swap;
			swap(*(beg + i), *(beg + r));
		}
	}
};



/**
*  The {@code StdStats} class provides static methods for computing
*  statistics such as min, max, mean, sample standard deviation, and
*  sample variance.
*  <p>
* API
* max;min;mean;var;stddev;median
*/
#include<algorithm>
#include<numeric>
class __Stats
{
public:
	template<typename It>
	static auto max_(It beg, It end) -> decltype(*beg)
	{
		return std::max(beg, end);
	}

	template<typename It>
	static auto min_(It beg, It end) -> decltype(*beg)
	{
		return std::min(beg, end);
	}

	template<typename It>
	static double mean(It beg, It end) 
	{
		double sum = std::accumulate(beg, end, 0.0);
		return sum / (end - beg);
	}

	template<typename It>
	static double var(It beg, It end) 
	{
		double avg = mean(beg, end);
		double sum = 0.0;
		std::foreach(beg, end, [&sum, avg](const double d) {sum += (d - avg)*(d - avg); });
		return sum / (end - beg - 1);
	}

	template<typename It>
	static double stddev(It beg, It end)
	{
		return std::sqrt(var(beg, end));
	}

};

static __Stats Stats;



