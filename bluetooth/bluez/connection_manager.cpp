#include "connection_manager.hpp"

#include <iostream>
#include <unistd.h>
#include <fcntl.h>

using namespace std::chrono_literals;

namespace Bluetooth
{
    ConnectionManager::ConnectionManager
    (
        Reactor* reactor,
        std::string serviceName,
        std::string path,
        std::function<void(const std::string &, int)> on_connect,
        std::function<void(int, std::string const& data)> on_receive,
        std::function <void(int)> on_disconnect
    )
        : reactor_{reactor}
        , serviceName_{std::move(serviceName)}
        , path_{std::move(path)}
        , on_connect{std::move(on_connect)}
        , on_receive{std::move(on_receive)}
        , on_disconnect{std::move(on_disconnect)}
    {
        selfPid_ = ::getpid();
    }
    void ConnectionManager::Release()
    {
        reactor_->stop();
    }
    void CloseConnection(int fd)
    {
        ::close(fd);
    }
    void ConnectionManager::NewConnection
    (
        DBusMock::object_path const& device,
        DBusMock::file_descriptor fd,
        DBusMock::variant_dictionary <std::unordered_map> const& fd_properties
    )
    {
        fd_ = dup(fd.operator int());
        fcntl(fd_, F_SETOWN, selfPid_);
        reactor_->add(fd_, [this](std::string const& data){
            on_receive(fd_, data);
        }, on_disconnect);
        on_connect(device.string(), fd_);
    }
    void ConnectionManager::RequestDisconnection(DBusMock::object_path const& device)
    {
        reactor_->stop();
    }
}
