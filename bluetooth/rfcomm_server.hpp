#pragma once

#include <dbus-mockery/bindings/bus.hpp>

#include <memory>
#include <string>
#include <cstdint>
#include <optional>
#include <chrono>

namespace Bluetooth
{
    // goes further than just members, also implements everything
    struct ServerImpl;

    struct ServerSpawnInfo
    {
        // rfcomm server channel
        uint8_t channel = 1;

        // path for dbus bluez.Profile1 interface
        std::string path;

        // A name for the profile to register.
        std::string name;

        // set auto connect property?
        bool autoConnect;

        // names the dbus instance, if provided, so it can be easily found in d-feet.
        // Warning! Only use if you allow it in a dbus config file!
        // Example config: /etc/dbus-1/system.d/WHATEVER.conf
        /* <!DOCTYPE busconfig PUBLIC
         *  "-//freedesktop//DTD D-BUS Bus Configuration 1.0//EN"
         *  "http://www.freedesktop.org/standards/dbus/1.0/busconfig.dtd">
         * <busconfig>
         *   <policy user="muster">
         *     <allow own="BUS_INTERFACE_NAME_STRING_HERE"/>
         *   </policy>
         *   <policy context="default">
         *     <allow send_destination="BUS_INTERFACE_NAME_STRING_HERE"/>
         *     <allow receive_sender="BUS_INTERFACE_NAME_STRING_HERE"/>
         *   </policy>
         * </busconfig>
         */
        std::optional <std::string> busInstanceName = {};

        // Change the uuid. WARNING This will break any client expection a "SerialPortServiceClass".
        std::optional <std::string> uuidOverride = {};
    };

    // interface only
    class Server
    {
    public:
        Server(DBusMock::dbus* bus, ServerSpawnInfo const& info);
        ~Server();

        /**
         * @brief start Starts the socket and registers the profile with bluez.
         */
        void start
        (
            std::function <void(std::string const& device, int)> on_connect,
            std::function <void(int, std::string const& data)> on_receive,
            std::function <void(int)> on_disconnect,
            std::chrono::milliseconds pollTimeout = std::chrono::milliseconds{1000}
        );

        /**
         * @brief stop Closes the socket and unregisters the profile.
         */
        void stop();

    private:
        std::unique_ptr <ServerImpl> impl_;
    };
}
