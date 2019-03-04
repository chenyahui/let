# server
add_executable(let_sample_echo_server server.cpp)
target_link_libraries(let_sample_echo_server let)

# client
add_executable(let_sample_echo_client client.cpp)
target_link_libraries(let_sample_echo_client let)


