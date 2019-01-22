#include <event2/listener.h>

namespace let {
    class TcpServer {
    public:

    private:
        evconnlistener *listener_;
    };
}