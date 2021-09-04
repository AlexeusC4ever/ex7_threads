#include "matrix_shift_multithread.h"
#include <vector>
#include <thread>
#include <shared_mutex>
#include <chrono>

class Tmp_Str
{
private:
	std::vector<int> tmp_str;
	std::unique_lock<std::shared_mutex> mutex;

public:
	//using unique_lock to capture a mutex during initialization
	Tmp_Str(std::shared_mutex& mute) :mutex(mute) {}

	void set_string(std::vector<int>&& a)
	{
		//after receiving the boundary row, we open access to it for another thread
		tmp_str = std::move(a);
		mutex.unlock();
	}

	std::vector<int>& get_string()
	{
		//trying to access the boundary row, capturing a mutex for reading
		std::shared_lock<std::shared_mutex> read_lock(*mutex.mutex());
		return tmp_str;
	}
};

void shift_right_down_multithread_with_buf(matrixvec& matrix,
	unsigned int threads)
{
	const size_t str = matrix.size();
	const size_t col = matrix[0].size();

	if (threads > str) threads = str;

	std::vector<std::thread> thrvec(threads);			//the vector of threads
	std::vector<std::shared_mutex> mutvec(threads);		//the vector of mutexes
	std::vector<Tmp_Str> locks_strings;					//the vector of boundary rows

	for (size_t i = 0; i < threads; ++i)
	//immediate capture of mutexes so that threads can't access to data that has mot yet been recieved
	{
		locks_strings.emplace_back(mutvec[i]);
	}

	size_t str_in_small_thread = str / threads;	//assigning intervals to threads
	size_t big_threads_mount = str % threads;

	size_t start = 0;

	for (size_t count = 0; count < threads; ++count)
	{
		size_t end = start;
		if (big_threads_mount > 0) {			//assigning intervals to threads
			end += str_in_small_thread + 1;
			--big_threads_mount;
		}
		else
		{
			end += str_in_small_thread;
		}


		thrvec[count] = std::thread([start, end, count, &matrix, &locks_strings, &mutvec]
			{
				size_t number_of_prev_mutex = count == 0 ? mutvec.size() - 1 : count - 1;
				size_t col = matrix[0].size();

				std::vector<int> str_for_lock_strings(col);
				str_for_lock_strings[0] = matrix[end - 1][col - 1];
				for (size_t i = 1; i < col; ++i)
				{
					str_for_lock_strings[i] = matrix[end - 1][i - 1];
				}

				//we give to the vector a boundary row that was already been shifted
				locks_strings[count].set_string(std::move(str_for_lock_strings));

				for (size_t i = start; i < end - 1; ++i)	//shift one element to the right in all other rows
				{
					int tmp = matrix[i][col - 1];
					for (size_t j = col - 1; j > 0; --j)
					{
						matrix[i][j] = matrix[i][j - 1];
					}
					matrix[i][0] = tmp;
				}

				//shifting all the rows except the first in interval
				for (size_t i = end - 1; i > start; --i)
				{
					matrix[i] = matrix[i - 1];
				}

				//instead of the first row in our interval we set the boundary row obtained from previous thread
				matrix[start] = locks_strings[number_of_prev_mutex].get_string();

			});

		start = end;
	}

	for (size_t i = 0; i < threads; ++i)
	{
		thrvec[i].join();
	}
}


void shift_down(matrixvec& matrix)
{
	const size_t str = matrix.size();
	const size_t col = matrix[0].size();


	for (size_t j = 0; j < col; ++j)
	{
		int tmp = matrix[str - 1][j];
		for (size_t i = str - 1; i >= 1; --i)
		{
			matrix[i][j] = matrix[i - 1][j];
		}
		matrix[0][j] = tmp;
	}
}

void shift_right(matrixvec& matrix)
{
	const size_t str = matrix.size();
	const size_t col = matrix[0].size();


	for (size_t i = 0; i < str; ++i)
	{
		int tmp = matrix[i][col - 1];
		for (size_t j = col - 1; j > 0; --j)
		{
			matrix[i][j] = matrix[i][j - 1];
		}
		matrix[i][0] = tmp;
	}
}

//void shift_right_down_multithread(std::vector<std::vector<int>>& matrix,
//	unsigned int threads)
//{
//
//	const size_t str = matrix.size();
//	const size_t col = matrix[0].size();
//
//	if (threads > str) threads = str;
//
//	std::vector<std::thread> thrvec(threads);
//	std::vector<std::shared_mutex> mutvec(threads);
//	std::vector<std::unique_lock<std::shared_mutex>> locks;
//
//	for (size_t i = 0; i < threads; ++i)
//	{
//		locks.emplace_back(mutvec[i]);
//	}
//
//	size_t str_in_small_thread = str / threads;
//	size_t big_threads_mount = str % threads;
//
//	size_t start = 0;
//
//	std::vector<int> flags(threads, 0);
//
//	for (size_t count = 0; count < threads; ++count)
//	{
//		size_t end = start;
//		if (big_threads_mount > 0) {
//			end += str_in_small_thread + 1;
//			--big_threads_mount;
//		}
//		else
//		{
//			end += str_in_small_thread;
//		}
//
//
//		thrvec[count] = std::thread([start, end, count, &matrix, &locks, &mutvec, &flags]
//			{
//				size_t number_of_prev_mutex = count == 0 ? mutvec.size() - 1 : count - 1;
//				size_t number_of_boundary_str = count == 0 ? matrix.size() - 1 : start - 1;
//				size_t col = matrix[0].size();
//				size_t str = matrix.size();
//				for (size_t i = start; i < end; ++i)
//				{
//					int tmp = matrix[i][col - 1];
//					for (size_t j = col - 1; j > 0; --j)
//					{
//						matrix[i][j] = matrix[i][j - 1];
//					}
//					matrix[i][0] = tmp;
//				}
//				locks[count].unlock();
//
//				std::vector<int> tmp_for_bound_last;
//
//				if (end - start > 1)
//				{
//					tmp_for_bound_last = matrix[end - 2];
//
//					std::shared_lock<std::shared_mutex> reading_lock(mutvec[number_of_prev_mutex]);
//					std::vector<int> tmp_for_bound = std::move(matrix[number_of_boundary_str]);
//					flags[number_of_prev_mutex] = 1;
//
//
//					for (size_t i = end - 2; i > start; --i)
//					{
//						matrix[i] = std::move(matrix[i - 1]);
//					}
//
//					matrix[start] = std::move(tmp_for_bound);
//
//				}
//				else
//				{
//					std::shared_lock<std::shared_mutex> reading_lock(mutvec[number_of_prev_mutex]);
//					tmp_for_bound_last = std::move(matrix[number_of_boundary_str]);
//					flags[number_of_prev_mutex] = 1;
//				}
//
//				while (flags[count] != 1)
//					std::this_thread::sleep_for(std::chrono::milliseconds(10));
//
//				matrix[end - 1] = std::move(tmp_for_bound_last);
//
//			});
//
//		start = end;
//	}
//
//	for (size_t i = 0; i < threads; ++i)
//	{
//		thrvec[i].join();
//	}
//}
