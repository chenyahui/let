#include <iostream>
#include <let/tcp_server.h>
#include <let/logger.h>

using namespace let;

void log(int severity, const char *msg)
{
    LOG_DEBUG << msg;
}

int main()
{
    event_enable_debug_logging(EVENT_DBG_ALL);
    event_enable_debug_mode();
    event_set_log_callback(log);

    ServerOptions options;

    TcpServer server(options, IpAddress(8079));

    server.setMessageCallback([](TcpConnectionPtr conn) {
        auto buffer = conn->inputBuffer()->pullUp();
        std::cout << "hi! " << buffer << "\n";
        conn->send(buffer);
    });

    server.run();

    return 0;
}