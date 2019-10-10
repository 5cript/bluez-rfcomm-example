#include "reactor.hpp"

#include <stdexcept>

#include <unistd.h>
#include <sys/epoll.h>
#include <iostream>

namespace Bluetooth
{
    Reactor::Reactor()
        : epollFd_{-1}
        , poller_{}
        , running_{false}
        , cbs_{}
        , discon_cbs_{}
    {
        epollFd_ = epoll_create1(0);
        if (epollFd_ == -1)
            throw std::runtime_error("cannot create epoll reactor");
    }
    void Reactor::add
    (
        int fd,
        std::function<void(const std::string &)> const& cb,
        std::function<void(int)> const& disconnect_cb
    )
    {
        epoll_event event;

        event.events = EPOLLIN;
        event.data.fd = fd;

        if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event))
            throw std::runtime_error("cannot add file descriptor epoll reactor");

        std::lock_guard <std::mutex> guard{cbProtect_};
        cbs_[fd] = cb;
        discon_cbs_[fd] = disconnect_cb;
    }
    void Reactor::stop()
    {
        running_.store(false);
        if (poller_.joinable())
        {
            poller_.join();
            if (epollFd_ != -1)
                close(epollFd_);
            epollFd_ = -1;
        }
    }
    void Reactor::start(std::chrono::milliseconds pollWaitTimeout)
    {
        stop();
        poller_ = std::thread{[this, pollWaitTimeout]() {
            running_.store(true);
            struct epoll_event events[4];
            constexpr int read_buffer_count = 25;
            char read_buffer[read_buffer_count];
            while(running_.load())
            {
                int readyCount = epoll_wait(epollFd_, events, 4, static_cast <int> (pollWaitTimeout.count()));
                if (readyCount == -1)
                {
                }
                for (int i = 0; i < readyCount; ++i)
                {
                    auto descriptor = events[i].data.fd;
                    auto amount = read(descriptor, read_buffer, read_buffer_count);
                    {
                        std::lock_guard <std::mutex> guard{cbProtect_};
                        if (amount == -1)
                        {
                            auto f = discon_cbs_[descriptor];
                            if (f)
                                f(descriptor);

                            close(descriptor);
                            cbs_.erase(descriptor);
                            discon_cbs_.erase(descriptor);
                        }
                        else
                        {
                            auto f = cbs_[descriptor];
                            if (f)
                                f(std::string(read_buffer, static_cast <std::size_t> (amount)));
                        }
                    }
                }
            }
        }};
    }
    Reactor::~Reactor()
    {
        stop();
    }
}
