#include <event2/listener.h>

#include "callback.h"
#include "acceptor.h"

namespace let {

    class TcpServer {
    public:
        void start();

        void setMessageCallback(const MessageCallback &messageCallback);

    private:
        Acceptor *acceptor_;
    };
} // namespace let