#ifndef MATRIX_SHIFT_MULTITHREAD_H_
#define MATRIX_SHIFT_MULTITHREAD_H_

#include <vector>
#include <thread>

using matrixvec = std::vector<std::vector<int>>;

void shift_down(matrixvec& matrix);

void shift_right(matrixvec& matrix);

void shift_right_down_multithread_with_buf(matrixvec& matrix,
	unsigned int threads = std::thread::hardware_concurrency());

//void shift_right_down_multithread(std::vector<std::vector<int>>& matrix,
//	unsigned int threads = std::thread::hardware_concurrency());

#endif //MATRIX_SHIFT_MULTITHREAD_H_