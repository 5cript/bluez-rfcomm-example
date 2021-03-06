#pragma once

#include <dbus-mockery/bindings/bus.hpp>
#include <dbus-mockery/bindings/object_path.hpp>
#include <dbus-mockery-system/bluez/device.hpp>

#include <memory>

namespace Bluetooth
{
    class Device
    {
    public:
        using device_type = DBusMock::Mocks::interface_mock <BlueZ::org::bluez::hci::dev::Device>;

        Device(DBusMock::dbus* bus, DBusMock::object_path const& path);
        ~Device();

        device_type& api();

    private:
        struct Implementation;

        std::unique_ptr <Implementation> impl_;
    };
}
