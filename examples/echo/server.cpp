#include <iostream>
#include <let/tcp_server.h>
#include <let/logger.h>
#include <let/utility.h>

#include <sstream>

using namespace let;

void log(int severity, const char *msg)
{
    LOG_DEBUG << msg;
}

class MyHandler : public TcpHandler {
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

        std::stringstream content;
        content << "HTTP/1.1 200 OK\r\n"
                << "Content-Type: text/plain\r\n"
                << "Content-Length: 6\r\n"
                << "Keep-Alive: true \r\n"
                << "\r\n"
                << "chen\r\n";

        std::string s = content.str();
        conn->write(s.data(), s.size());
    }

    void onDisconnected(TcpConnectionPtr) override
    {
        // LOG_DEBUG << "on disconnected";
    }

    void onError(TcpConnectionPtr, int error) override
    {
        // LOG_DEBUG << "on error";
    }
};

int main()
{
    EventLoopThreadPool pool(3);
    //    ThreadedExecutorPool thread_pool(2);

    TcpServer server;

    server.setGroup(&pool);
    server.setHandlerFactory([]() -> TcpHandlerPtr {
        return std::make_shared<MyHandler>();
    });
//    Logger::setLogLevel(ERROR);
    //    server.setExecutor(&thread_pool);

    server.listen(IpAddress(8098));
    pool.start();
    //    thread_pool.start();

    loop_util_ask_to_quit();

    LOG_INFO << "begin stop";
    server.gracefullyStop();
    pool.stop();
    return 0;
}