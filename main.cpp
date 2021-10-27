#include "bluetooth/rfcomm_server.hpp"
#include "bluetooth/bluez/adapter.hpp"
#include "bluetooth/bluez/device.hpp"

#include <dbus-glue/bindings/busy_loop.hpp>

#include <iostream>

int main()
{
    using namespace Bluetooth;
    using namespace std::string_literals;

    auto bus = DBusMock::open_system_bus();

    ServerSpawnInfo info
    {
        1,
        "/bla/bluetooth",
        "SerialServer",
        true,
        "de.blabla.cpp"s
    };

    Server server{&bus, info};
    Bluetooth::Adapter driver(&bus);

    driver.alias("device-3.0T");
    std::cout << "power status: " << driver.power() << "\n";
    std::cout << "discover timeout: " << driver.discoverTimeout() << "\n";

    // for discovery and paring, do this someplace else:
    /*
        driver.discoverable(true);
        driver.pairable(true);
    */

    auto phone = driver.getPairedByName("HUAWEI P20");

    phone.api().Trusted = true;

    server.start(
        // on connect:
        [](std::string const& path, int fd){
            std::cout << "new connection " << path << " with fd " << fd << "\n";
        },
        // on data:
        [](int fd, std::string const& data){
            std::cout << "Data recieved from " << fd << ": " << data << "\n";
            ::write(fd, "ok", 2);
        },
        // on disconnect:
        [](int fd) {
            std::cout << fd << " disconnected\n";
        }
    );

    DBusMock::make_busy_loop(&bus);

    std::cin.get();

    return 0;
}
