#include "tnl/net/http/HttpResponse.h"

using namespace tnl;
using namespace tnl::net;

void HttpResponse::appendToBuffer(Buffer* output) const
{
    char buf[32];
    snprintf(buf, sizeof buf, "HTTP/1.1 %d ", mStatusCode);

    output->append(buf, strlen(buf));
    output->append(mStatusMessage);
    output->append("\r\n");

    if (mCloseConnection)
    {
        output->append("Connection: close\r\n");
    }
    else
    {
        snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", mBody.size());
        output->append(buf);
        output->append("Connection: Keep-Alive\r\n");
    }

    for (const auto& header : mHeaders)
    {
        output->append(header.first);
        output->append(": ");
        output->append(header.second);
        output->append("\r\n");
    }

    output->append("\r\n");
    output->append(mBody);
}