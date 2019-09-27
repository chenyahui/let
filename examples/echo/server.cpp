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
    virtual void onConnected(TcpConnectionPtr)
    {
        LOG_DEBUG << "on connected";
    }

    virtual int splitMessage(TcpConnectionPtr, const char *data, size_t length)
    {
        LOG_DEBUG << "split message";
        return length;
    }

    virtual int onMessage(TcpConnectionPtr conn, const char *data, size_t length)
    {
        LOG_DEBUG << "onMessage:" << data;
        conn->write(data, length);
    }

    virtual int onDisconnected(TcpConnectionPtr)
    {
        LOG_DEBUG << "on disconnected";
    }

    virtual int onError(TcpConnectionPtr, int error)
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