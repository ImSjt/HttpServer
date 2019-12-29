#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include "tnl/net/TcpServer.h"
#include "tnl/net/http/HttpConnection.h"

namespace tnl
{
namespace net
{

class HttpServer : public TcpServer
{
public:
    HttpServer(EventLoop* loop, const InetAddress& listenAddr,
                const std::string& name, HttpCallback httpCallback, bool reusePort = false);
    virtual ~HttpServer();

protected:
    virtual TcpConnection* createNewConnection(EventLoop* loop,
                    const std::string& name, int sockfd,
                    const InetAddress& localAddr, const InetAddress& peerAddr);

private:
    HttpCallback mHttpCallback;

};

} // namespace tnl
} // namespace tnl

#endif // _HTTP_SERVER_H_
