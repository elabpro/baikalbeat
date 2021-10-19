cmake_minimum_required(VERSION 3.16)
project(baikalbeat VERSION 1.9)

#set(BUILD_SHARED_LIBS OFF)
#set(CMAKE_EXE_LINKER_FLAGS "-static")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    )

add_executable(baikalbeat
               baikalbeat.cc)

target_link_libraries(baikalbeat
                      cppkafka
                      rdkafka
                      pthread
                      boost_program_options
                      boost_system
                      boost_thread)

