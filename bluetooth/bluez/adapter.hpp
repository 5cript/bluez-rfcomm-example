#pragma once

#include <dbus-glue/bindings/bus.hpp>
#include "device.hpp"

#include <memory>
#include <string>

namespace Bluetooth
{
    class Adapter
    {
    private:
        struct Implementation;

    public:
        Adapter(DBusMock::dbus* bus, std::string const& hci = "hci0", const std::string& adapter = "Adapter1");
        ~Adapter();

        /**
         * @brief power Set power on/off
         * @param on
         */
        void power(bool on);

        /**
         * @brief power Is powered on?
         */
        bool power() const;

        /**
         * @brief discoverable Is discoverable?
         */
        bool discoverable() const;

        /**
         * @brief discoverable Set device discoverable or not
         * @param isDiscoverable
         */
        void discoverable(bool isDiscoverable);

        /**
         * @brief discoverTimeout The discore time frame.
         * @return The current set discover timeframe.
         */
        uint32_t discoverTimeout() const;

        /**
         * @brief disocoverTimeout Set discover timeout.
         * @param timeout
         */
        void disocoverTimeout(uint32_t timeout);

        /**
         * @brief alias Bluetooth device alias
         * @return
         */
        std::string alias() const;

        /**
         * @brief alias Set device alias.
         * @param alias
         */
        void alias(std::string const& alias);

        /**
         * @brief pairable Is device pairable?
         * @return
         */
        bool pairable() const;

        /**
         * @brief pairable Make device pairable (or not)
         * @param isPairable
         */
        void pairable(bool isPairable);

        /**
         * @brief discoverTimeout The discore time frame.
         * @return The current set discover timeframe.
         */
        uint32_t pairableTimeout() const;

        /**
         * @brief disocoverTimeout Set discover timeout.
         * @param timeout
         */
        void pairableTimeout(uint32_t timeout);

        /**
         * @brief pairedDevices Retrieve a list of paired devices
         * @return
         */
        std::vector <DBusMock::object_path> pairedDevices() const;

        /**
         * @brief getPairedByName Retrieve a device by name that is paired.
         * @param name
         * @return
         */
        Device getPairedByName(std::string const& name);

    private:
        std::unique_ptr <Implementation> impl_;
    };
}
