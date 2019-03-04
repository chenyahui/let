#include <let/tcp_client.h>
#include <let/callback.h>
#include <let/tcp_conn.h>
#include <let/logger.h>

#include <iostream>

using namespace let;

int main()
{
    EventLoop loop;
    IpAddress addr("127.0.0.1", 8079);
    auto client = TcpClient(&loop, addr);

    client.setConnectionCallback([](TcpConnectionPtr conn) {
        LOG_DEBUG << "连接成功";
        conn->send("hello");
    });

    client.setMessageCallback([](TcpConnectionPtr conn) {
        auto msg = conn->inBuffer()->retrieveAllAsString();
        std::cout << msg << std::endl;
    });

    client.connect();
    loop.loop();

    return 0;
}