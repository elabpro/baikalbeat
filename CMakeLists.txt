include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${ELASTICLIENT_INCLUDE_DIRS}
                    /opt/src/elasticlient/external/cpr/include
                    )

add_executable(baikalbeat
               baikalbeat.cc)

target_link_libraries(baikalbeat
                      ${ELASTICLIENT_LIBRARIES}
                      cppkafka
                      jsoncpp
                      elasticlient
                      cpr
                      pthread
                      boost_program_options
                      boost_system
                      boost_thread)
