#include <iostream>
#include <let/tcp_server.h>
#include <let/logger.h>
#include <let/utility.h>

using namespace let;

void log(int severity, const char *msg)
{
    LOG_DEBUG << msg;
}

class MyHandler : public TcpHandler
{
public:
    ~MyHandler()
    {
        LOG_VERBOSE << "myhandler deconstruct";
    }

    void onConnected(TcpConnectionPtr) override
    {
        LOG_DEBUG << "on connected";
    }

    int splitMessage(TcpConnectionPtr, const char *data, size_t length) override
    {
        LOG_DEBUG << "split message";
        return static_cast<int>(length);
    }

    void onMessage(TcpConnectionPtr conn, const char *data, size_t length) override
    {
        LOG_DEBUG << "onMessage:" << data;
        conn->write(data, length);
    }

    void onDisconnected(TcpConnectionPtr) override
    {
        LOG_DEBUG << "on disconnected";
    }

    void onError(TcpConnectionPtr, int error) override
    {
        LOG_DEBUG << "on error";
    }
};

int main()
{
    EventLoopThreadPool pool(1);
    TcpServer server;

    server.setGroup(&pool);
    server.setHandlerFactory([]() -> TcpHandlerPtr {
        return std::make_shared<MyHandler>();
    });

    server.listen(IpAddress(8098));
    pool.start();

    bool quit = false;
    while (!quit)
    {
        let::sleep_ms(1000);
    }

    std::cout << "begin stop";
    server.gracefullyStop();
    pool.stop();
    return 0;
}