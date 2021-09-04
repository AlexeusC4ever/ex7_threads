cmake_minimum_required(VERSION 3.5)

add_library(
	shift_thread 
	STATIC 
	matrix_shift_multithread.cpp
	matrix_shift_multithread.h
)
set_target_properties(
	shift_thread PROPERTIES
	CXX_STANDART 17
	CXX_STANDART_REQUIRED ON
)
