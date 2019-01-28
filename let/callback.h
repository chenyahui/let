//
// Created by yahuichen on 2019/1/25.
//

#ifndef LET_CALLBACK_H
#define LET_CALLBACK_H

#include <functional>

namespace let
{
class TcpConnection;

using MessageCallback = std::function<void(TcpConnection *)>;

} // namespace let
#endif //LET_CALLBACK_H
