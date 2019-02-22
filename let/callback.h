//
// Created by yahuichen on 2019/1/25.
//

#ifndef LET_CALLBACK_H
#define LET_CALLBACK_H

#include <functional>
#include <memory>

namespace let
{
class TcpConnection;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

using MessageCallback = std::function<void(TcpConnectionPtr)>;
using ConnectionCallback = std::function<void(TcpConnectionPtr)>;

} // namespace let
#endif //LET_CALLBACK_H
