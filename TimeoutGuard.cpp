#pragma once

#include "stdafx.h"
#include "TimeoutGuard.h"

namespace utility
{
	TimeoutGuard::TimeoutGuard(
		clock::duration timeout,
		std::function<void( void )> alarm,
		unsigned short sleep_divisor
	)
		: timeout( timeout )
		, alarm( alarm )
		, naptime( timeout / sleep_divisor )
	{
		idle.store( true );
		live.store( true );

		timer_thread = std::thread( std::bind( &TimeoutGuard::guard, this ) );
	}

	TimeoutGuard::~TimeoutGuard()
	{
		live.store( false );
		timer_thread.join();
	}

	void TimeoutGuard::guard()
	{
		while ( live.load() )
		{
			if ( !idle.load() )
			{
				auto now = clock::now();

				if ((now - touched.load()) > timeout)
				{
					idle.store( true );
					alarm();
				}
			}

			std::this_thread::sleep_for( naptime );
		};
	}

	void TimeoutGuard::operator()()
	{
		touch();
		idle.store( false );
	}

	void TimeoutGuard::touch()
	{
		touched.store( clock::now() );
	}
}
