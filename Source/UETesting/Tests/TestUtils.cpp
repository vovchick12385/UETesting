#if (WITH_DEV_AUTOMATION_TESTS || WITH_PERF_AUTOMATION_TESTS)
#include "TestUtils.h"

namespace TPS
{
	namespace Test
	{
		UWorld* GetTestGameWorld()
		{
	
			const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
			for ( const FWorldContext& Context : WorldContexts )
			{
				if ((Context.WorldType == EWorldType::PIE || Context.WorldType == EWorldType::Game) && Context.World() != nullptr)
				{
					return Context.World();
				}
			}

			return nullptr;
		}

		void CallFuncByNameWithParams(UObject* Object, const FString& FuncName, const TArray<FString>& Params)
		{
			if(!Object)
				return;
			FString Command = FString::Printf(TEXT("%s"), *FuncName);
			for(const auto& Param:Params)
			{
				Command.Append(" ").Append(Param);
			}
			FOutputDeviceNull OutputDeviceNull;
			Object->CallFunctionByNameWithArguments(*Command, OutputDeviceNull, nullptr, true);
		}
	}
}

#endif

