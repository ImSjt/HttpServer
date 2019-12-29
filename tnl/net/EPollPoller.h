#ifndef _EPOLL_POLLER_H_
#define _EPOLL_POLLER_H_

#include <vector>

#include "tnl/net/Poller.h"

struct epoll_event;

namespace tnl
{
namespace net
{

class EPollPoller : public Poller
{
public:
    EPollPoller(EventLoop* loop);
    virtual ~EPollPoller() override;

    virtual int poll(int timeoutMs, ChannelList& activeChannels) override;
    virtual void updateChannel(Channel* channel) override;
    virtual void removeChannel(Channel* channel) override;

private:
    void fillActiveChannels(int num, ChannelList& activeChannels);
    void update(int operation, Channel* channel);

private:
    using EventList = std::vector<struct epoll_event>;

    int mEpollfd;
    EventList mEvents;

    static const int mInitEventListSize = 16;
};

}
}

#endif // _EPOLL_POLLER_H_