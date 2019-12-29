#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "tnl/base/Logger.h"
#include "tnl/net/EventLoop.h"
#include "tnl/net/EPollPoller.h"

using namespace tnl;
using namespace tnl::net;

static_assert(EPOLLIN == POLLIN,        "epoll uses same flag values as poll");
static_assert(EPOLLPRI == POLLPRI,      "epoll uses same flag values as poll");
static_assert(EPOLLOUT == POLLOUT,      "epoll uses same flag values as poll");
static_assert(EPOLLRDHUP == POLLRDHUP,  "epoll uses same flag values as poll");
static_assert(EPOLLERR == POLLERR,      "epoll uses same flag values as poll");
static_assert(EPOLLHUP == POLLHUP,      "epoll uses same flag values as poll");

static const int New = -1;
static const int Added = 1;
static const int Deleted = 2;

EPollPoller::EPollPoller(EventLoop* loop) :
    Poller(loop),
    mEpollfd(::epoll_create1(EPOLL_CLOEXEC)),
    mEvents(mInitEventListSize)
{
    assert(mEpollfd > 0);
}

EPollPoller::~EPollPoller()
{
    ::close(mEpollfd);
}

int EPollPoller::poll(int timeoutMs, ChannelList& activeChannels)
{
    int numEvents = ::epoll_wait(mEpollfd,
                                &*mEvents.begin(),
                                static_cast<int>(mEvents.size()),
                                timeoutMs);
    LOG_TRACE("epoll return %d", numEvents);
    if (numEvents > 0)
    {
        fillActiveChannels(numEvents, activeChannels);
        if (static_cast<size_t>(numEvents) == mEvents.size())
        {
            mEvents.resize(mEvents.size()*2);
        }
    }
    else if (numEvents == 0)
    {
        
    }
    else
    {

    }

    return numEvents;
}

void EPollPoller::updateChannel(Channel* channel)
{
    assert(mLoop->isInLoopThread());

    const int index = channel->index();
    LOG_TRACE("channel index:%d", index);
    if (index == New || index == Deleted)
    {
        // a new one, add with EPOLL_CTL_ADD
        int fd = channel->fd();
        if (index == New)
        {
            assert(mChannels.find(fd) == mChannels.end());
            mChannels[fd] = channel;
        }
        else // index == kDeleted
        {
            assert(mChannels.find(fd) != mChannels.end());
            assert(mChannels[fd] == channel);
        }

        channel->setIndex(Added);
        update(EPOLL_CTL_ADD, channel);
    }
    else
    {
        // update existing one with EPOLL_CTL_MOD/DEL
        int fd = channel->fd();
        assert(mChannels.find(fd) != mChannels.end());
        assert(mChannels[fd] == channel);
        assert(index == Added);
        if (channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(Deleted);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPollPoller::removeChannel(Channel* channel)
{
    assert(mLoop->isInLoopThread());
    int fd = channel->fd();
  
    assert(mChannels.find(fd) != mChannels.end());
    assert(mChannels[fd] == channel);
    assert(channel->isNoneEvent());
    int index = channel->index();
    assert(index == Added || index == Deleted);
    size_t n = mChannels.erase(fd);

    assert(n == 1);

    if (index == Added)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(New);
}

void EPollPoller::fillActiveChannels(int num, ChannelList& activeChannels)
{
    assert(static_cast<size_t>(num) <= mEvents.size());
    for (int i = 0; i < num; ++i)
    {
        Channel* channel = static_cast<Channel*>(mEvents[i].data.ptr);

        channel->setREvents(mEvents[i].events);
        activeChannels.push_back(channel);
        LOG_TRACE("push channel to active channels");
    }
}

static const char* operationToStr(int operation)
{
    if (operation == EPOLL_CTL_DEL)
    {
        return "EPOLL_CTL_DEL";
    }
    else if (operation == EPOLL_CTL_ADD)
    {
        return "EPOLL_CTL_ADD";
    }
    else if (operation == EPOLL_CTL_MOD)
    {
        return "EPOLL_CTL_MOD";
    }
    else
    {
        return "ERROR OP";
    }
}

void EPollPoller::update(int operation, Channel* channel)
{
    struct epoll_event event;
    memset(&event, 0, sizeof event);
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();

    LOG_TRACE("%s fd(%d)", operationToStr(operation), fd);

    if (::epoll_ctl(mEpollfd, operation, fd, &event) < 0)
    {
        LOG_ERROR("epoll_ctl op = %s, fd = %d failure", operationToStr(operation), fd);
    }
}