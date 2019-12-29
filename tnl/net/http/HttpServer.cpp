#include "tnl/base/Logger.h"
#include "tnl/net/http/HttpServer.h"
#include "tnl/net/http/HttpConnection.h"

using namespace tnl;
using namespace tnl::net;

HttpServer::HttpServer(EventLoop* loop, const InetAddress& listenAddr,
            const std::string& name, HttpCallback httpCallback, bool reusePort) :
    TcpServer(loop, listenAddr, name, reusePort),
    mHttpCallback(std::move(httpCallback))
{

}

HttpServer::~HttpServer()
{

}

TcpConnection* HttpServer::createNewConnection(EventLoop* loop,
                const std::string& name, int sockfd,
                const InetAddress& localAddr, const InetAddress& peerAddr)
{
    HttpConnection* httpConnection = new HttpConnection(loop, name, sockfd, localAddr, peerAddr);
    httpConnection->setHttpCallback(mHttpCallback);
    
    return httpConnection;
}
