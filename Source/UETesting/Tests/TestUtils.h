#pragma once
namespace TPS
{
	namespace Test
	{
		template<typename Type1, typename Type2>
		struct TestPayload
		{
			Type1 TestValue;
			Type2 ExpectedValue;
		};
	}
}