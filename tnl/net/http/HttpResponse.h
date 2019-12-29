#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_

#include <map>
#include <string>

#include "tnl/net/Buffer.h"

namespace tnl
{
namespace net
{

class HttpResponse
{
public:
    enum HttpStatusCode
    {
        Unknown,
        Code200Ok = 200,
        Code301MovedPermanently = 301,
        Code400BadRequest = 400,
        Code404NotFound = 404,
    };

    HttpResponse(bool close) :
        mStatusCode(Unknown),
        mCloseConnection(close)
    {

    }

    void setStatusCode(HttpStatusCode code)
    { mStatusCode = code; }

    void setStatusMessage(const std::string& message)
    { mStatusMessage = message; }

    void setCloseConnection(bool on)
    { mCloseConnection = on; }

    bool closeConnection() const
    { return mCloseConnection; }

    void setContentType(const std::string& contentType)
    { addHeader("Content-Type", contentType); }

    void addHeader(const std::string& key, const std::string& value)
    { mHeaders[key] = value; }

    void setBody(const std::string& body)
    { mBody = body; }

    void appendToBuffer(Buffer* output) const;

private:
    std::map<std::string, std::string> mHeaders;
    HttpStatusCode mStatusCode;
    std::string mStatusMessage;
    bool mCloseConnection;
    std::string mBody;
};

}
}

#endif // _HTTP_RESPONSE_H_