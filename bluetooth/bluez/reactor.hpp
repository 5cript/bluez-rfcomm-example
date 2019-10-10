#pragma once

#include <thread>
#include <chrono>
#include <atomic>
#include <string>
#include <functional>
#include <unordered_map>
#include <mutex>

namespace Bluetooth
{
    class Reactor
	{
	public:
		Reactor();
		~Reactor();

		/**
		 * @brief add Adds file descriptor to poll.
		 * @param fd The file descriptor to poll on.
		 * @param cb A callback to call on after read.
		 */
		void add
		(
		    int fd,
		    std::function <void(std::string const&)> const& cb,
		    std::function <void(int)> const& disconnect_cb
		);

		/**
		 * @brief start	Start polling
		 */
		void start(std::chrono::milliseconds pollWaitTimeout = std::chrono::milliseconds{1000});

		/**
		 * @brief stop Stop polling.
		 */
		void stop();

		Reactor(Reactor const&) = delete;
		Reactor(Reactor&&) = default;
		Reactor& operator=(Reactor const&) = delete;
		Reactor& operator=(Reactor&&) = delete;

	private:
		int epollFd_;
		std::thread poller_;
		std::atomic_bool running_;
		std::mutex cbProtect_;
		std::unordered_map <int, std::function <void(std::string const&)> > cbs_;
		std::unordered_map <int, std::function <void(int)> > discon_cbs_;
	};
}
