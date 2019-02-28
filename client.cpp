#include <let/tcp_client.h>
#include <let/callback.h>
#include <let/tcp_conn.h>

#include <iostream>

using namespace let;

int main()
{
    EventLoop loop;
    IpAddress addr("127.0.0.1", 8080);
    auto client = TcpClient(&loop, addr);

    client.setConnectionCallback([](TcpConnectionPtr conn) {
        std::cout << "已连接";
        conn->send("hello");
    });
    client.connect();

    loop.loop();
    
    return 0;
}