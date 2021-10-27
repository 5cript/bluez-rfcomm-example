#include "rfcomm_server.hpp"

#include "bluez/connection_manager.hpp"
#include "bluez/reactor.hpp"

#include <dbus-glue/dbus_interface.hpp>
#include <dbus-glue/bindings/variant_helpers.hpp>
#include <dbus-glue-system/bluez/bluez.hpp>
#include <dbus-glue/interface_builder.hpp>

#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <bluetooth/rfcomm.h>

#include <cstdio>
#include <cstdint>
#include <atomic>
#include <thread>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <unordered_map>

bdaddr_t bdaddr_any = { 0 };
bdaddr_t bdaddr_local = {{0, 0, 0, 0xff, 0xff, 0xff}};

using namespace std::string_literals;

namespace Bluetooth
{
//#####################################################################################################################
    struct ServerImpl
    {
        ServerImpl(Server const* parent, ServerSpawnInfo const& info, DBusGlue::dbus* bus);
        ~ServerImpl();

        /**
         * @brief registerService Registers the service with bluez
         */
        void registerService();

        Server const* parent;
        DBusGlue::dbus* bus;
        std::shared_ptr <Bluetooth::ConnectionManager> handler;
        Reactor reactor;
        ServerSpawnInfo info;
    };
//---------------------------------------------------------------------------------------------------------------------
    ServerImpl::ServerImpl(Server const* parent, ServerSpawnInfo const& info, DBusGlue::dbus* bus)
        : parent{parent}
        , bus{bus}
        , handler{}
        , reactor{}
        , info{info}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    void ServerImpl::registerService()
    {
        using namespace DBusGlue;
        using namespace ExposeHelpers;

        DBusGlue::construct_interface <ConnectionManager> (
            handler.get(),
            exposable_method_factory{} <<
                name("Release") <<
                flags(SD_BUS_VTABLE_UNPRIVILEGED) <<
                as(&ConnectionManager::Release),
            exposable_method_factory{} <<
                name("NewConnection") <<
                parameter("path") <<
                parameter("fd") <<
                parameter("properties") <<
                flags(SD_BUS_VTABLE_UNPRIVILEGED) <<
                as(&ConnectionManager::NewConnection),
            exposable_method_factory{} <<
                name("RequestDisconnection") <<
                parameter("path") <<
                flags(SD_BUS_VTABLE_UNPRIVILEGED) <<
                as(&ConnectionManager::RequestDisconnection)
        );

        int res = 0;
        res = bus->expose_interface(handler);
        if (res < 0)
            throw std::runtime_error("could not register interface: "s + strerror(-res));

        if (info.busInstanceName)
        {
            res = sd_bus_request_name
            (
                bus->handle(),
                info.busInstanceName.value().c_str(),
                SD_BUS_NAME_ALLOW_REPLACEMENT | SD_BUS_NAME_REPLACE_EXISTING
            );
            if (res < 0)
                throw std::runtime_error("could not give myself a name: "s + strerror(-res));
        }

        auto profileMan = create_interface <BlueZ::org::bluez::ProfileManager>
        (
            *bus,
            "org.bluez",
            "/org/bluez",
            "org.bluez.ProfileManager1"
        );

        // fill option map
        variant_dictionary <std::unordered_map> options;
        variant_store(*bus, options["Name"], info.name);
        variant_store(*bus, options["Role"], "server"s);
        variant_store(*bus, options["Channel"], static_cast <uint16_t> (info.channel));
        variant_store(*bus, options["AutoConnect"], info.autoConnect);

        // register profile
        std::string uuid = "00001101-0000-1000-8000-00805f9b34fb";
        if (info.uuidOverride)
            uuid = info.uuidOverride.value();
        profileMan.RegisterProfile(object_path{info.path}, uuid, options);
    }
//---------------------------------------------------------------------------------------------------------------------
    ServerImpl::~ServerImpl()
    {
    }
//#####################################################################################################################
    Server::Server(DBusGlue::dbus* bus, ServerSpawnInfo const& info)
        : impl_{new ServerImpl(this, info, bus)}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    Server::~Server()
    {
        stop();
    }
//---------------------------------------------------------------------------------------------------------------------
    void Server::start
    (
        std::function <void(std::string const& device, int)> on_connect,
        std::function <void(int, std::string const& data)> on_receive,
        std::function <void(int)> on_disconnect,
        std::chrono::milliseconds pollTimeout)
    {
        impl_->handler.reset(new ConnectionManager(
            &impl_->reactor,
            "org.bluez.Profile1",
            impl_->info.path,
            on_connect,
            on_receive,
            on_disconnect
        ));

        impl_->reactor.start(pollTimeout);
        impl_->registerService();
    }
//---------------------------------------------------------------------------------------------------------------------
    void Server::stop()
    {
        impl_->reactor.stop();

        using namespace DBusGlue;
        auto profileMan = create_interface <BlueZ::org::bluez::ProfileManager>
        (
            *impl_->bus,
            "org.bluez",
            "/org/bluez",
            "org.bluez.ProfileManager1"
        );

        profileMan.UnregisterProfile(object_path{impl_->info.path});
    }
//#####################################################################################################################
}
