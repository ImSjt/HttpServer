#ifndef _HTTP_CONNECTION_H_
#define _HTTP_CONNECTION_H_
#include <iostream>
#include "tnl/net/TcpConnection.h"
#include "tnl/net/http/HttpRequest.h"
#include "tnl/net/http/HttpResponse.h"

namespace tnl
{
namespace net
{

using HttpCallback = std::function<void(const HttpRequest&, HttpResponse&)>;

class HttpConnection : public TcpConnection
{
public:
    HttpConnection(EventLoop* loop, const std::string& name, int sockfd,
                    const InetAddress& localAddr, const InetAddress& peerAddr);
    virtual ~HttpConnection();

    void setHttpCallback(const HttpCallback& httpCallback)
    {
        mHttpCallback = httpCallback;
    }

protected:
    virtual void handleConnection();    // 连接建立或者连接断开
    virtual void handleBytes(Buffer* buf);  // 处理请求
    // virtual void handleWriteComplete(); // 写完成
    // virtual void highWaterMark(size_t); // 超过警戒线时的写处理

private:
    enum HttpRequestParseState
    {
        ExpectRequestLine, // 请求行
        ExpectHeaders,     // 请求首部
        ExpectBody,        // 实体
        GotAll,            // 解析完毕
    };

    bool processRequestLine(const char* begin, const char* end);
    bool parseRequest(Buffer* buf);

    bool parseFinish()
    { return mState == GotAll; }

    void handleRequest(const HttpRequest& httpRequest);
    void handleRespond(const HttpResponse& httpResponse);

    void reset()
    {
        mState = ExpectRequestLine;
        HttpRequest request;
        mHttpRequest.swap(request);
    }

private:
    HttpRequestParseState mState;
    HttpRequest mHttpRequest;
    HttpCallback mHttpCallback;
};

} // namespace tnl
} // namespace tnl

#endif // _HTTP_CONNECTION_H_