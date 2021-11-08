cmake_minimum_required(VERSION 3.16)

project(baikalbeat VERSION 1.9)

add_executable(baikalbeat baikalbeat.cc)

target_include_directories(baikalbeat
  PUBLIC ${CMAKE_SOURCE_DIR}/cppkafka/include
)

target_link_libraries(baikalbeat
	${CMAKE_SOURCE_DIR}/cppkafka/build/src/lib/libcppkafka.so
	rdkafka
	pthread
	boost_program_options
	boost_system
	boost_thread
)
