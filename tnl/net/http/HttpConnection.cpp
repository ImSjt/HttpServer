#include <assert.h>
#include <algorithm>

#include "tnl/base/Logger.h"
#include "tnl/net/http/HttpConnection.h"

using namespace tnl;
using namespace tnl::net;

HttpConnection::HttpConnection(EventLoop* loop, const std::string& name, int sockfd,
                    const InetAddress& localAddr, const InetAddress& peerAddr) :
    TcpConnection(loop, name, sockfd, localAddr, peerAddr),
    mState(ExpectRequestLine)
{

}

HttpConnection::~HttpConnection()
{
    LOG_TRACE("Http Connection release");
}

void HttpConnection::handleConnection()
{
    LOG_TRACE("HTTP:%s -> %s is %s", localAddress().toIpPort().c_str(),
                                peerAddress().toIpPort().c_str(),
                                (connected() ? "UP" : "DOWN"));
}

void HttpConnection::handleBytes(Buffer* buf)
{
    if (!parseRequest(buf))
    {
        send("HTTP/1.1 400 Bad Request\r\n\r\n");
        shutdown();
    }

    if (parseFinish())
    {
        handleRequest(mHttpRequest);
        reset();
    }
}

// GET / HTTP/1.1
bool HttpConnection::processRequestLine(const char* begin, const char* end)
{
    const char* start = begin;
    const char* space = std::find(begin, end, ' ');
    bool succeed = false;

    if (space != end && mHttpRequest.setMethod(start, space))
    {
        start = space + 1;
        space = std::find(start, end, ' ');

        if (space != end)
        {
            const char* question = std::find(start, space, '?');
            if (question != space)
            {
                mHttpRequest.setPath(start, question);
                mHttpRequest.setQuery(question, space);
            }
            else
            {
                mHttpRequest.setPath(start, space);
            }
        
            start = space+1;

            succeed = end-start==8 && std::equal(start, end-1, "HTTP/1.");
            if (succeed)
            {
                if (*(end-1) == '1')
                {
                    mHttpRequest.setVersion(HttpRequest::Http11);
                }
                else if (*(end-1) == '0')
                {
                    mHttpRequest.setVersion(HttpRequest::Http10);
                }
                else
                {
                    succeed = false;
                }
            }
        }        
    }

    return succeed;
}

bool HttpConnection::parseRequest(Buffer* buf)
{
    bool continueToParse = true;
    bool ok;

    while (continueToParse)
    {
        switch (mState)
        {
        case ExpectRequestLine:
        {
            const char* crlf = buf->findCRLF();
            if (crlf)
            {
                ok = processRequestLine(buf->peek(), crlf);
                if (ok)
                {
                    mState = ExpectHeaders;
                }
                else
                {
                    continueToParse = false;
                }
                buf->retrieveUntil(crlf+2);
            }
            else
            {
                continueToParse = false;
            }
            break;
        }
        case ExpectHeaders:
        {
            const char* crlf = buf->findCRLF();
            if (crlf)
            {
                const char* colon = std::find(buf->peek(), crlf, ':');
                if (colon != crlf)
                {
                    mHttpRequest.addHeader(buf->peek(), colon, crlf);
                }
                else
                {
                    mState = GotAll;
                    continueToParse = false;
                }

                buf->retrieveUntil(crlf+2);
            }
            else
            {
                continueToParse = false;
            }
            
            break;
        }
        case ExpectBody:
        {

            break;
        }
        }
    }

    return ok;
}

void HttpConnection::handleRequest(const HttpRequest& httpRequest)
{
    const std::string& connection = httpRequest.getHeader("Connection");

    bool close = connection == "close" ||
        (httpRequest.getVersion() == HttpRequest::Http10 && connection != "Keep-Alive");

    HttpResponse httpResponse(close);

    assert(mHttpCallback);
    mHttpCallback(httpRequest, httpResponse);

    handleRespond(httpResponse);
}

void HttpConnection::handleRespond(const HttpResponse& httpResponse)
{
    Buffer buf;
    httpResponse.appendToBuffer(&buf);
    send(&buf);
    if (httpResponse.closeConnection())
    {
        shutdown();
    }
}