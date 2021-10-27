#include "adapter.hpp"

#include <dbus-glue/dbus_interface.hpp>

#include <dbus-glue-system/bluez/hci.hpp>
#include <dbus-glue-system/bluez/bluez.hpp>
#include <dbus-glue-system/dbus/interfaces.hpp>

#include <type_traits>

using namespace DBusGlue;
using namespace std::string_literals;

namespace Bluetooth
{
//#####################################################################################################################
    struct Adapter::Implementation
    {
        using adapter_type = DBusGlue::Mocks::interface_mock <BlueZ::org::bluez::hci::Adapter>;
        using network_server_type = DBusGlue::Mocks::interface_mock <BlueZ::org::bluez::hci::NetworkServer>;

        DBusGlue::dbus* bus;
        std::string hci;
        adapter_type adapter;
        network_server_type server;

        Implementation(DBusGlue::dbus* bus, std::string const& hci, std::string const& adapter)
            : bus{bus}
            , hci{hci}
            , adapter{[bus, &hci, &adapter]() -> auto {
                return create_interface <adapter_type::interface_type>
                (
                    *bus,
                    "org.bluez",
                    "/org/bluez/"s + hci,
                    "org.bluez."s + adapter
                );
            }()}
            , server{[bus, &hci]() -> auto {
                return create_interface <network_server_type::interface_type>
                (
                    *bus,
                    "org.bluez",
                    "/org/bluez/"s + hci,
                    "org.bluez.NetworkServer1"s
                );
            }()}
        {}
    };
//#####################################################################################################################
    Adapter::Adapter(DBusGlue::dbus* bus, std::string const& hci, std::string const& adapter)
        : impl_{new Implementation(bus, hci, adapter)}
    {

    }
//---------------------------------------------------------------------------------------------------------------------
    Adapter::~Adapter() = default;
//---------------------------------------------------------------------------------------------------------------------
    void Adapter::power(bool on)
    {
        impl_->adapter.Powered = on;
    }
//---------------------------------------------------------------------------------------------------------------------
    bool Adapter::power() const
    {
        return impl_->adapter.Powered;
    }
//---------------------------------------------------------------------------------------------------------------------
    bool Adapter::discoverable() const
    {
        return impl_->adapter.Discoverable;
    }
//---------------------------------------------------------------------------------------------------------------------
    void Adapter::discoverable(bool isDiscoverable)
    {
        impl_->adapter.Discoverable = isDiscoverable;
    }
//---------------------------------------------------------------------------------------------------------------------
    uint32_t Adapter::discoverTimeout() const
    {
        return impl_->adapter.DiscoverableTimeout;
    }
//---------------------------------------------------------------------------------------------------------------------
    void Adapter::disocoverTimeout(uint32_t timeout)
    {
        impl_->adapter.DiscoverableTimeout = timeout;
    }
//---------------------------------------------------------------------------------------------------------------------
    std::string Adapter::alias() const
    {
        return impl_->adapter.Alias;
    }
//---------------------------------------------------------------------------------------------------------------------
    void Adapter::alias(std::string const& alias)
    {
        impl_->adapter.Alias = alias;
    }
//---------------------------------------------------------------------------------------------------------------------
    void Adapter::pairable(bool isPairable)
    {
        impl_->adapter.Pairable = isPairable;
    }
//---------------------------------------------------------------------------------------------------------------------
    bool Adapter::pairable() const
    {
        return impl_->adapter.Pairable;
    }
//---------------------------------------------------------------------------------------------------------------------
    uint32_t Adapter::pairableTimeout() const
    {
        return impl_->adapter.PairableTimeout;
    }
//---------------------------------------------------------------------------------------------------------------------
    void Adapter::pairableTimeout(uint32_t timeout)
    {
        impl_->adapter.PairableTimeout = timeout;
    }
//---------------------------------------------------------------------------------------------------------------------
    std::vector <DBusGlue::object_path> Adapter::pairedDevices() const
    {
        auto objectManager = create_interface <DBus::org::freedesktop::DBus::ObjectManager>
        (
            *impl_->bus,
            "org.bluez",
            "/",
            "org.freedesktop.DBus.ObjectManager"
        );

        auto objects = objectManager.GetManagedObjects();
        std::vector <DBusGlue::object_path> devices;
        for (auto const& [key, value] : objects)
        {
            auto keyString = key.string();
            if (keyString.find(impl_->hci) != std::string::npos)
            {
                auto front = "/org/bluez/"s + impl_->hci;
                auto extracted = keyString.substr(front.size(), keyString.size() - front.size());
                if
                (
                    "/org/bluez/"s + impl_->hci != keyString &&
                    extracted.size() == 22
                )
                {
                    devices.push_back(key);
                }
            }
        }
        return devices;
    }
//---------------------------------------------------------------------------------------------------------------------
    Device Adapter::getPairedByName(std::string const& name)
    {
        DBusGlue::object_path foundDev;
        auto devices = pairedDevices();
        for (auto const& device : devices)
        {
            std::cout << device << "\n";
            Device dev{impl_->bus, device};

            if (dev.api().Name == name)
                foundDev = device;
        }

        return {impl_->bus, foundDev};
    }
//#####################################################################################################################
}
