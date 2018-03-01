#pragma once
#include<string>
#include<vector>
class LSD
{
public:
	LSD() = delete;
	static void sort(std::vector<std::string>& a, int w)
	{
		int n = a.size();
		int R = 256;
		std::vector<std::string> aux(n);
		std::vector<int> count(R + 1, 0);

		for (int d = w - 1; d >= 0; --d)
		{			
			for (auto& i : count) i = 0;

			//compute frequency counts
			for (int i = 0; i < n; ++i)
				++count[a[i][d] + 1];

			//compute cumulates
			for (int r = 0; r < R; ++r)
				count[r + 1] += count[r];

			//move data
			for (int i = 0; i < n; ++i)
				aux[count[a[i][d]]++] = std::move(a[i]);

			//copy back
			for (int i = 0; i < n; ++i)
				a[i] = std::move(aux[i]);
		}
	}

	static void sort(std::vector<int> &a)
	{
		const int BITS_PER_BYTE = 8;
		const int BITS = 32;
		const int R = 1 << BITS_PER_BYTE;//r=256
		const int MASK = R - 1;//0xff
		const int W = BITS / BITS_PER_BYTE;//4

		int N = a.size();
		std::vector<int> aux(N);
		std::vector<int> count(R+1, 0);

		for (int d = 0; d < W; ++d)
		{
			for (int &i : count) i = 0;

			for (int i = 0; i < N; ++i)
			{
				int c = (a[i] >> BITS_PER_BYTE*d)&MASK;
				count[c + 1]++;
			}

			for (int r = 0; r < R; ++r)
				count[r + 1] += count[r];

			//处理significant byte
			if (d == W - 1)
			{
				int shift1 = count[R] - count[R>>1];//负数的个数
				int shift2 = count[R >> 1];//自然数的个数

				for (int r = 0; r < (R >> 1); ++r)
					count[r] += shift1;
				for (int r = (R >> 1); r < R; ++r)
					count[r] -= shift2;
			}

			//move data
			for (int i = 0; i < N; ++i)
			{
				int c = (a[i] >> BITS_PER_BYTE*d)&MASK;
				aux[count[c]++] = a[i];
			}

			for (int i = 0; i < N; ++i)
				a[i] = aux[i];
		}
	}
};
#include<algorithm>
class MSD
{
private:
	static const int BITS_PER_BYTE = 8;
	static const int BITS_PER_INT = 32;
	static const int R = 256;
	static const int CUTOFF = 15;//cutoff to insrtion sort

public:
	MSD() = delete;

	static void sort(std::vector<std::string> &a)
	{
		int N = a.size();
		std::vector<std::string> aux(N);
		sort(a, 0, N - 1, 0, aux);
	}

	static void sort(std::vector<int> &a)
	{
		int N = a.size();
		std::vector<int> aux(N);
		sort(a, 0, N - 1, 0, aux);
	}

private:

	static int charAt(std::string& s, int d)
	{
		if (d >= s.size()) return -1;
		return s[d];
	}

	static void sort(std::vector<std::string> &a, int lo, int hi, int d, std::vector<std::string> &aux)
	{
		if (hi <= lo +CUTOFF)
		{
			insertion(a, lo, hi, d);
			return;
		}

		//compute frequency counts
		std::vector<int> count(R + 2, 0);

		for (int i = lo; i <= hi; ++i)
		{
			int c = charAt(a[i], d);
			count[c + 2]++;
		}

		//transform counts to indices
		for (int r = 0; r < R + 1; ++r)
		{
			count[r + 1] += count[r];
		}

		//distribute
		for (int i = lo; i <= hi; ++i)
		{
			int c = charAt(a[i], d);
			aux[count[c + 1]++] = std::move(a[i]);
		}

		for (int i = lo; i <= hi; ++i)
			a[i] = std::move(aux[i - lo]);

		//感觉r=0的情况下，不需要排序了。。肯定都是已序了啊
		for (int r = 1; r < R; ++r)
			if(count[r+1]>count[r])
				sort(a, lo + count[r], lo + count[r + 1] - 1, d + 1, aux);
	}

	//starting at dth character
	static void insertion(std::vector<std::string> &a, int lo, int hi, int d)
	{
		for (int i = lo; i <= hi; ++i)
		{
			int j = i;
			std::string cur = std::move(a[i]);
			for (; j > lo &&less(cur, a[j - 1], d); --j)
				a[j] = std::move(a[j - 1]);

			a[j] = std::move(cur);
		}
	}

	static bool less(std::string &s1, std::string &s2, int d)
	{
		for (int i = d; i < std::min(s1.size(), s2.size()); ++i)
		{
			if (s1[i] < s2[i]) return true;
			if (s2[i] < s1[i]) return false;
		}

		return s1.size() < s2.size();
	}


	static void sort(std::vector<int> &a, int lo, int hi, int d, std::vector<int> &aux)
	{
		if (d == 4) return;
		if (hi <= lo + CUTOFF)
		{
			insertion(a, lo, hi, d);
			return;
		}

		std::vector<int> count(R + 1, 0);
		int MASK = R - 1;//0xff
		int shift = BITS_PER_INT - BITS_PER_BYTE*d - BITS_PER_BYTE;

		for (int i = lo; i <= hi; ++i)
		{
			int c = (a[i] >> shift) && MASK;
			count[c + 1]++;
		}

		for (int r = 0; r < R; ++r)
			count[r + 1] += count[r];

		// for most significant byte, 0x80-0xFF comes before 0x00-0x7F
		if (d == 0) {
			int shift1 = count[R] - count[R / 2];
			int shift2 = count[R / 2];
			for (int r = 0; r < R / 2; r++)
				count[r] += shift1;
			for (int r = R / 2; r < R; r++)
				count[r] -= shift2;
		}

		for (int i = lo; i <= hi; ++i)
		{
			int c = (a[i] >> shift) && MASK;
			aux[count[c]++] = a[i];
		}

		for (int i = lo; i <= hi; ++i)
			a[i] = aux[i - lo];

		if (count[0] > 0)
			sort(a, lo, lo + count[0] - 1, d + 1, aux);
		for (int r = 0; r < R; ++r)
			if (count[r + 1]>count[r])
				sort(a, lo + count[r], lo + count[r + 1] - 1, d + 1, aux);

	}

	static void insertion(std::vector<int> &a, int lo, int hi, int d)
	{
		for (int i = lo; i <= hi; ++i)
		{
			int j = i;
			int t = a[i];
			for (; j > lo && t < a[j - 1]; --j)
				a[j] = a[j - 1];

			a[j] = t;
		}
	}
};


class Quick3String
{
private:
	static const int CUTOFF = 15;

public:
	Quick3String() = delete;

	static void sort(std::vector<std::string> &a)
	{
		//先随机化
		//random.shuffle(a)
		sort(a, 0, a.size() - 1, 0);
	}

private:
	static int charAt(std::string &s, int d)
	{
		if (d >= s.size()) return -1;
		return s[d];
	}

	static void sort(std::vector<std::string> &a, int lo, int hi, int d)
	{
		if (hi <= lo + CUTOFF)
		{
			insertion(a, lo, hi, d);
			return;
		}

		int lt = lo, gt = hi;
		int v = charAt(a[lo], d);

		int i = lo + 1;
		while (i <= gt)
		{
			int t = charAt(a[i], d);
			if (t < v) std::swap(a[lt++], a[i++]);
			else if (t>v) std::swap(a[i], a[gt--]);
			else ++i;
		}

		sort(a, lo, lt - 1, d);
		if (v >= 0) sort(a, lt, gt, d + 1);
		sort(a, gt + 1, hi, d);
	}

	//starting at dth character
	static void insertion(std::vector<std::string> &a, int lo, int hi, int d)
	{
		for (int i = lo; i <= hi; ++i)
		{
			int j = i;
			std::string cur = std::move(a[i]);
			for (; j > lo &&less(cur, a[j - 1], d); --j)
				a[j] = std::move(a[j - 1]);

			a[j] = std::move(cur);
		}
	}

	static bool less(std::string &s1, std::string &s2, int d)
	{
		for (int i = d; i < std::min(s1.size(), s2.size()); ++i)
		{
			if (s1[i] < s2[i]) return true;
			if (s2[i] < s1[i]) return false;
		}

		return s1.size() < s2.size();
	}

};
#include<iostream>
#include"ST.h"
class Alphabet
{
private:
	std::string ts;
	RedBlackBST<char, int> st;
	
	
public:
	Alphabet(const std::string &s)
		:ts(s),st('\0',-1)
	{
		for (int i = 0; i < s.size(); ++i)
			st.put(s[i], i);
	}

	char toChar(int index) { return ts[index]; }

	int toIndex(char c) { return st.get(c); }

	bool contains(char c) { return st.contains(c); }

	int R() { return ts.size(); }

	int lgR() { return std::log10(R()) / std::log10(2); }

	std::vector<int> toIndices(const std::string &s)
	{
		std::vector<int> ret;
		for (auto c : s)
			ret.push_back(st.get(c));
		return ret;
	}

	std::string toChars(std::vector<int> &indices)
	{
		std::string ret;
		for (auto i : indices)
			ret += ts[i];
		return ret;
	}

};