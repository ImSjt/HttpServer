#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include <assert.h>
#include <string>

#include "tnl/base/copyable.h"

namespace tnl
{
namespace net
{

class HttpRequest : copyable
{
public:
    enum Method
    {
        Invalid, Get, Post, Head, Put, Delete
    };

    enum Version
    {
        Unknown, Http10, Http11
    };

    HttpRequest() :
        mMethod(Invalid),
        mVersion(Unknown)
    {   }

    void setVersion(Version v)
    {
        mVersion = v;
    }

    Version getVersion() const
    {
        return mVersion;
    }

    bool setMethod(const char* start, const char* end)
    {
        assert(mMethod == Invalid);

        std::string m(start, end);
        if (m == "GET")
        {
            mMethod = Get;
        }
        else if (m == "POST")
        {
            mMethod = Post;
        }
        else if (m == "HEAD")
        {
            mMethod = Head;
        }
        else if (m == "PUT")
        {
            mMethod = Put;
        }
        else if (m == "DELETE")
        {
            mMethod = Delete;
        }
        else
        {
            mMethod = Invalid;
        }

        return mMethod != Invalid;
    }

    Method method() const
    { return mMethod; }

    const char* methodString() const
    {
        const char* result = "UNKNOWN";
        switch(mMethod)
        {
        case Get:
            result = "GET";
            break;
        case Post:
            result = "POST";
            break;
        case Head:
            result = "HEAD";
            break;
        case Put:
            result = "PUT";
            break;
        case Delete:
            result = "DELETE";
            break;
        default:
            break;
        }
        return result;
    }

    void setPath(const char* start, const char* end)
    {
        mPath.assign(start, end);
    }

    const std::string& path() const
    { return mPath; }
    
    void setQuery(const char* start, const char* end)
    {
        mQuery.assign(start, end);
    }

    const std::string& query() const
    {
        return mQuery;
    }

    // xxx: xxx
    void addHeader(const char* start, const char* colon, const char* end)
    {
        std::string field(start, colon);
        ++colon;

        while (colon < end && isspace(*colon))
        {
            ++colon;
        }

        std::string value(colon, end);
        while (!value.empty() && isspace(value[value.size()-1]))
        {
            value.resize(value.size()-1);
        }

        mHeaders[field] = value;
    }

    std::string getHeader(const std::string& field) const
    {
        std::string result;
        std::map<std::string, std::string>::const_iterator it = mHeaders.find(field);
        if (it != mHeaders.end())
        {
            result = it->second;
        }
        
        return result;
    }

    const std::map<std::string, std::string>& headers() const
    { return mHeaders; }

    void swap(HttpRequest& that)
    {
        std::swap(mMethod, that.mMethod);
        std::swap(mVersion, that.mVersion);
        mPath.swap(that.mPath);
        mQuery.swap(that.mQuery);
        mHeaders.swap(that.mHeaders);
    }

private:
    Method mMethod;
    Version mVersion;
    std::string mPath;
    std::string mQuery;
    std::map<std::string, std::string> mHeaders;
};

} // namespace net
} // namespace tnl

#endif // _HTTP_REQUEST_H_