#pragma once

#include "reactor.hpp"

#include <dbus-mockery/bindings/exposable_interface.hpp>
#include <dbus-mockery/bindings/types.hpp>
#include <unordered_map>
#include <functional>

namespace Bluetooth
{
    /**
	 * @brief The Profile class
	 */
    class ConnectionManager : public DBusMock::exposable_interface
	{
	public:
		ConnectionManager
		(
		    Reactor* reactor,
		    std::string serviceName,
		    std::string path,
		    std::function <void(std::string const& device, int)> on_connect,
		    std::function <void(int, std::string const& data)> on_receive,
		    std::function <void(int)> on_disconnect
		);

		std::string path() const override
		{
			return path_;
		}
		std::string service() const override
		{
			return serviceName_;
		}

		void CloseConnection(int fd);

		void Release();
		void NewConnection
		(
		    DBusMock::object_path const& device,
		    DBusMock::file_descriptor fd,
		    DBusMock::variant_dictionary <std::unordered_map> const& fd_properties
		);
		void RequestDisconnection(DBusMock::object_path const& device);

	private:
		Reactor* reactor_;
		std::string serviceName_;
		std::string path_;
		int fd_;
		int selfPid_;
		std::function <void(std::string const& device, int)> on_connect;
		std::function <void(int, std::string const& data)> on_receive;
		std::function <void(int)> on_disconnect;
	};
}
