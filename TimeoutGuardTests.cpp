#include "stdafx.h"
#include "CppUnitTest.h"

#include "TimeoutGuard.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace utility
{		
	TEST_CLASS( TimeoutGuardTest )
	{
	public:

		std::atomic_bool triggered{ false };

		void shoud_trigger()
		{
			triggered.store( true );
		}

		TEST_METHOD( TimeoutGuardExpiration )
		{
			TimeoutGuard tg{
				std::chrono::milliseconds{ 5 },
				std::bind( &TimeoutGuardTest::shoud_trigger, this )
			};

			triggered.store( false );
			tg.watch();
			std::this_thread::sleep_for( std::chrono::milliseconds{ 10 } );
			Assert::IsTrue( triggered.load(), L"Failed to call the timeout alarm on the first run", LINE_INFO() );

			triggered.store( false );
			tg.watch();
			std::this_thread::sleep_for( std::chrono::milliseconds{ 10 } );
			Assert::IsTrue( triggered.load(), L"Failed to call the timeout alarm on the second run", LINE_INFO() );
		}


		TEST_METHOD( TimeoutGuardNoAlarm )
		{
			TimeoutGuard tg{
				std::chrono::milliseconds{ 5 },
				std::bind( &TimeoutGuardTest::shoud_trigger, this )
			};

			triggered.store( false );
			tg.watch();
			std::this_thread::sleep_for( std::chrono::milliseconds{ 1 } );
			Assert::IsFalse( triggered.load(), L"Wrongly called the timeout alarm on the first run", LINE_INFO() );

			triggered.store( false );
			tg.watch();
			for (auto i = 0; i < 10; ++i)
			{
				std::this_thread::sleep_for( std::chrono::milliseconds{ 1 } );
				tg.touch();
			}
			Assert::IsFalse( triggered.load(), L"Wrongly called the timeout alarm on the second run", LINE_INFO() );
		}
	};
}
