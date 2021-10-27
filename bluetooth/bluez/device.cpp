#include "device.hpp"

#include <dbus-glue/dbus_interface.hpp>

using namespace DBusGlue;

namespace Bluetooth
{
//#####################################################################################################################
    struct Device::Implementation
    {
        using device_type = DBusGlue::Mocks::interface_mock <BlueZ::org::bluez::hci::dev::Device>;

        DBusGlue::object_path path;
        device_type device;

        Implementation(DBusGlue::dbus* bus, DBusGlue::object_path const& path)
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
    Device::Device(DBusGlue::dbus* bus, DBusGlue::object_path const& path)
        : impl_{new Implementation(bus, path)}
    {

    }
//---------------------------------------------------------------------------------------------------------------------
    Device::~Device() = default;
//---------------------------------------------------------------------------------------------------------------------
    Device::Device(Device&&) = default;
//---------------------------------------------------------------------------------------------------------------------
    Device& Device::operator=(Device&&) = default;
//---------------------------------------------------------------------------------------------------------------------
    Device::device_type& Device::api()
    {
        return impl_->device;
    }
//#####################################################################################################################
}
