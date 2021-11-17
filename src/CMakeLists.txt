cmake_minimum_required(VERSION 3.16)

project(baikalbeat VERSION 1.9)

add_executable(baikalbeat baikalbeat.cc)

add_subdirectory(${CMAKE_SOURCE_DIR}/cppkafka ${CMAKE_SOURCE_DIR}/cppkafka/build)

target_include_directories(baikalbeat
    PUBLIC ${CMAKE_SOURCE_DIR}/cppkafka/include
)

target_link_directories(baikalbeat
    PUBLIC ${CMAKE_SOURCE_DIR}/cppkafka/build/src/lib
)

target_link_libraries(baikalbeat
	rdkafka
	cppkafka
	pthread
	boost_program_options
	boost_system
	boost_thread
)
