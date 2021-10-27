#pragma once

#include <dbus-glue/bindings/bus.hpp>
#include <dbus-glue/bindings/object_path.hpp>
#include <dbus-glue-system/bluez/device.hpp>

#include <memory>

namespace Bluetooth
{
    class Device
    {
    public:
        using device_type = DBusGlue::Mocks::interface_mock <BlueZ::org::bluez::hci::dev::Device>;

        Device(DBusGlue::dbus* bus, DBusGlue::object_path const& path);
        ~Device();
        Device(Device&&);
        Device(Device const&) = delete;
        Device& operator=(Device&&);
        Device& operator=(Device const&) = delete;

        device_type& api();

    private:
        struct Implementation;

        std::unique_ptr <Implementation> impl_;
    };
}
