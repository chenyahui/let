add_library(
        let
        tcp_server.h
        tcp_server.cpp
        acceptor.h
        acceptor.cpp
        event_loop.cpp
        event_loop.h
        event_executor_group.h
        event_executor_group.cpp
        tcp_connection.h
        tcp_connection.cpp
        buffer.h
        callback.h
        connector.h
        connector.cpp
        tcp_client.h
        tcp_client.cpp
        ip_addr.h
        ip_addr.cpp
        socket.h
        socket.cpp
        utility.h
        utility.cpp
        logger.h
        logger.cpp
        noncopyable.h
        callback.h
        threaded_executor.h
        threaded_executor.cpp
        event_executor.h
)

target_link_libraries(let event pthread)