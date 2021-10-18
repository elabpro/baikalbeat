cmake_minimum_required(VERSION 3.16)
project(baikalbeat)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${ELASTICLIENT_INCLUDE_DIRS}
                    /opt/src/elasticlient/external/cpr/include
                    )

add_executable(baikalbeat
               baikalbeat.cc)

target_link_libraries(baikalbeat
                      ${ELASTICLIENT_LIBRARIES}
                      cppkafka
                      pthread
                      boost_program_options
                      boost_system
                      boost_thread)

set(CMAKE_C_FLAGS
    "-fsanitize=leak -fno-omit-frame-pointer -g -O1"
    CACHE STRING "Flags used by the C compiler during builds."
    FORCE)

#set(CMAKE_CXX_FLAGS
#    "-fsanitize=leak -fno-omit-frame-pointer -g -O1"
#    CACHE STRING "Flags used by the C compiler during builds."
#    FORCE)
