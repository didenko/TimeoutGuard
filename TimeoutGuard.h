#pragma once

namespace utility
{
	/**
		The `clock` alias is for easy switching to `steady_clock` once Microsoft fixes it
	*/
	typedef std::chrono::system_clock clock;

	/**
		The `TimeoutGuard` class triggers the `alarm` callback from the `guard_thread`
		if `touch` was not called for at least the `timeout` duration.

		Because of the way the `guard_thread` sleeps, the actual detection may happen
		as late as after `timeout` + `naptime` duration. Hence it is possible that the alarm
		will not be called if the `TimeoutGuard` instance is touched within the
		'timeout` and `timeout` + `naptime` timeframe.

		If not provided, by default the `naptime` is same as `timeout`.

		The `TimeoutGuard` is not active after construction, whicn means, that the
		`guard_thread` will block until it is activated by calling the `watch` method.

		The `TimeoutGuard` class is not copyable and not moveable.
	*/
	class TimeoutGuard
	{
	public:
		TimeoutGuard(
			clock::duration timeout,
			std::function<void( void )> alarm,
			clock::duration naptime
		);

		TimeoutGuard(
			clock::duration timeout,
			std::function<void( void )> alarm
		);

		~TimeoutGuard();

		TimeoutGuard( const TimeoutGuard & ) = delete;
		TimeoutGuard & operator=(const TimeoutGuard & ) = delete;

		TimeoutGuard( TimeoutGuard && ) = delete;
		TimeoutGuard & operator=( TimeoutGuard && ) = delete;

		void watch();
		void touch();

	private:

		void guard();

		clock::duration timeout;
		clock::duration naptime;
		std::function<void( void )> alarm;

		std::atomic_bool idle;
		std::atomic_bool live;

		std::atomic<clock::time_point> touched;

		std::thread guard_thread;
		std::mutex guard_mutex;
		std::condition_variable wakeup;
	};
}
