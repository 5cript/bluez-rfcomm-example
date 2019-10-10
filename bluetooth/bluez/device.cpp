#include "device.hpp"

#include <dbus-mockery/dbus_interface.hpp>

using namespace DBusMock;

namespace Bluetooth
{
//#####################################################################################################################
    struct Device::Implementation
    {
        using device_type = DBusMock::Mocks::interface_mock <BlueZ::org::bluez::hci::dev::Device>;

        DBusMock::object_path path;
        device_type device;

        Implementation(DBusMock::dbus* bus, DBusMock::object_path const& path)
            : path{path}
            , device{[bus, &path]() -> auto {
                return create_interface <device_type::interface_type>
                (
                    *bus,
                    "org.bluez",
                    path.string(),
                    "org.bluez.Device1"
                );
            }()}
        {
        }
    };
//#####################################################################################################################
    Device::Device(DBusMock::dbus* bus, DBusMock::object_path const& path)
        : impl_{new Implementation(bus, path)}
    {

    }
//---------------------------------------------------------------------------------------------------------------------
    Device::~Device() = default;
//---------------------------------------------------------------------------------------------------------------------
    Device::device_type& Device::api()
    {
        return impl_->device;
    }
//#####################################################################################################################
}
