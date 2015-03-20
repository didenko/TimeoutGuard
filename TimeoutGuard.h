#pragma once

namespace utility
{
	typedef std::chrono::system_clock clock;

	struct TimeoutGuard
	{
		TimeoutGuard(
			clock::duration timeout,
			std::function<void( void )> alarm,
			unsigned short sleep_divisor = 3
		);
		~TimeoutGuard();

		void operator()();
		void touch();

	private:

		void guard();

		clock::duration timeout;
		clock::duration naptime;
		std::function<void( void )> alarm;

		std::atomic_bool idle;
		std::atomic_bool live;

		std::atomic<clock::time_point> touched;

		std::thread timer_thread;
	};
}

