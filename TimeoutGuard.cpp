#include "stdafx.h"
#include "TimeoutGuard.h"

namespace utility
{
	TimeoutGuard::TimeoutGuard(
		clock::duration timeout,
		std::function<void( void )> alarm,
		clock::duration naptime
	)
		: timeout( timeout )
		, alarm( alarm )
		, naptime( naptime )
	{
		guard_thread = std::thread( std::bind( &TimeoutGuard::guard, this ) );
	}

	TimeoutGuard::TimeoutGuard(
		clock::duration timeout,
		std::function<void( void )> alarm
	)
	: TimeoutGuard( timeout, alarm, timeout )
	{};

	TimeoutGuard::~TimeoutGuard()
	{
		live.store( false );
		wakeup.notify_all();
		guard_thread.join();
	}

	void TimeoutGuard::guard()
	{
		while ( live.load() )
		{
			if ( idle.load() )
			{
				// Sleep indefinitely until either told to become active or destruct
				std::unique_lock<std::mutex> live_lock( guard_mutex );
				wakeup.wait( live_lock, [this]() { return ! this->idle.load() || ! this->live.load(); } );
			};

			// quit the loop if destructing
			if ( ! live.load() ) break;

			// the actual timeout checking
			auto now = clock::now();

			if ( ( now - touched.load() ) > timeout )
			{
				idle.store( true );
				alarm();
				continue; // skip waiting for next timeout
			}

			{
				// sleep until next timeout check or destruction
				std::unique_lock<std::mutex> live_lock( guard_mutex );
				wakeup.wait_for( live_lock, naptime, [this](){ return ! this->live.load(); } );
			}
		};
	}

	void TimeoutGuard::watch()
	{
		touch();
		idle.store( false );
		wakeup.notify_all();
	}

	void TimeoutGuard::touch()
	{
		touched.store( clock::now() );
	}
}
