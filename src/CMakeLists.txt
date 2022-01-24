cmake_minimum_required(VERSION 3.16)

project(baikalbeat VERSION 1.9)

add_executable(baikalbeat baikalbeat.cc)

add_subdirectory(${CMAKE_SOURCE_DIR}/../deps/cppkafka ${CMAKE_SOURCE_DIR}/../deps/cppkafka/build)

target_include_directories(baikalbeat
    PUBLIC ${CMAKE_SOURCE_DIR}/../deps/cppkafka/include
    PUBLIC ${CMAKE_SOURCE_DIR}/../deps/p7/Headers
)

target_link_directories(baikalbeat
    PUBLIC ${CMAKE_SOURCE_DIR}/../deps/cppkafka/build/src/lib
	PUBLIC ${CMAKE_SOURCE_DIR}/../deps/p7/build
)

target_link_libraries(baikalbeat
	rdkafka
	cppkafka
	pthread
        p7
	boost_program_options
	boost_system
	boost_thread
)
