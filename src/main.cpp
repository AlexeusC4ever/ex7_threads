#include <iostream>
#include <vector>
#include <thread>
#include <iostream>
#include "matrix_shift_multithread.h"

class Timer
{
private:
	using clock_t = std::chrono::high_resolution_clock;
	using second_t = std::chrono::duration<double, std::ratio<1> >;

	std::chrono::time_point<clock_t> m_beg;

public:
	Timer() : m_beg(clock_t::now()) {}

	void reset() { m_beg = clock_t::now(); }

	double elapsed() const
	{
		return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
	}
};

void matrix_print(const std::vector<std::vector<int>>& matrix)
{
	for (auto a : matrix)
	{
		for (int b : a)
		{
			std::cout << b << "\t";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl << std::endl;
}

int main()
{
	size_t str = 1000;
	size_t col = 1000;

	matrixvec matrix(str);


	for (size_t i = 0; i < str; ++i) {
		for (size_t j = 0; j < col; ++j) {
			matrix[i].push_back(i * 10 + j);
		}
	}

	matrixvec matrix2(matrix);

	Timer t;
	double buf;

	shift_right_down_multithread_with_buf(matrix);

	buf = t.elapsed();

	std::cout << "shift_right_down_multithread_with_buf: " << buf << std::endl;

	t.reset();



	shift_down(matrix2);
	shift_right(matrix2);

	buf = t.elapsed();

	std::cout << "simple_shift_right_down_: " << buf << std::endl;

	t.reset();

	for (size_t i = 0; i < str; ++i)
	{
		for (size_t j = 0; j < col; ++j)
		{
			if (matrix[i][j] != matrix2[i][j]) {
				std::cout << "ERROR" << std::endl;
				return 0;
			}
		}
	}
	return 0;
}