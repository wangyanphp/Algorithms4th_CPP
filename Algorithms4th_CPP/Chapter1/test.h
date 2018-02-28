#pragma once

//for 1.4 test programming's performance
#include<Windows.h>
/**
*  The {@code Stopwatch} data type is for measuring
*  the time that elapses between the start and end of a
*  programming task (wall-clock time).
*
*/

class Stopwatch
{
private:
	DWORD  start;

public:
	/**
	* Initializes a new stopwatch.
	*/
	Stopwatch() { start = ::GetTickCount(); }

	/**
	* Returns the elapsed CPU time (in seconds) since the stopwatch was created.
	*
	*/
	double elapsedTime()
	{
		DWORD now = ::GetTickCount();
		return (now - start)/1000.0;
	}

	void reset() { start = ::GetTickCount(); }
};

/**
 *test programming's performance
 *
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
 */
template<typename Func>
void doubleTesting(Func timeTrial)
{
	for (int N = 250; true; N += N)
	{
		double time = timeTrial(N);
		std::cout << N << '\t' << time << std::endl;
	}
}

template<typename Func>
void doubleRatio(Func timeTrial)
{
	double prev = timeTrial(125);
	for (int N = 250; true; N += N)
	{
		double time = timeTrial(N);
		std::cout << N << '\t' << time << '\t' << time / prev<<std::endl;
		prev = time;
	}
}
