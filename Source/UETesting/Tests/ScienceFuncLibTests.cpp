

#if(WITH_DEV_AUTOMATION_TESTS || WITH_PERF_AUTOMATION_TESTS)
#include "ScienceFuncLibTests.h"
#include "UETesting/Science/ScienceFuncLib.h"
#include "CoreMinimal.h"


#include "TestUtils.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFibonacciSimple, "TPSGame.Science.FibonacciSimple",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFibonacciStress, "TPSGame.Science.Fibonacci",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::StressFilter | EAutomationTestFlags::LowPriority);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFibonacciLogHasErrors, "TPSGame.Science.FibonacciLogHasErrors",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority);


bool FFibonacciSimple::RunTest(const FString& Parameters)
{
	AddInfo("Fibonacci simple testing");
	
	TestTrueExpr(UScienceFuncLib::Fibonacci(0) == 0);
	TestTrueExpr(UScienceFuncLib::Fibonacci(1) == 1);
	TestTrueExpr(UScienceFuncLib::Fibonacci(2) == 1);
	TestTrueExpr(UScienceFuncLib::Fibonacci(3) == 2);

	

	TArray<TPS::Test::TestPayload<int32, int32>> TestData{{0,0}, {1,1}, {2,1}, {3,2}, {4,3},{5,5}};

	for(const auto& c: TestData)
	{
		//TestTrueExpr(UScienceFuncLib::Fibonacci(c.TestValue) == c.ExpectedValue);
		const FString InfoString = FString::Printf(TEXT("Test value: %i, expected value: %i"), c.TestValue, c.ExpectedValue);
		TestEqual(InfoString,UScienceFuncLib::Fibonacci(c.TestValue) , c.ExpectedValue );
	}
	return true;
}

bool FFibonacciStress::RunTest(const FString& Parameters)
{

	AddInfo("Fibonacci stress testing");
	/*for(int32 i=2;i<40;++i)
	{
		TestTrueExpr(UScienceFuncLib::Fibonacci(i) == UScienceFuncLib::Fibonacci( i-1) +UScienceFuncLib::Fibonacci( i-2));
	}*/
	int32 PrevPrevValue = 0;
	int PrevValue = 1;
	for(int32 i=2;i<40;++i)
	{
		const int32 CurrentValue = UScienceFuncLib::Fibonacci(i);
		TestTrueExpr(CurrentValue == PrevValue + PrevPrevValue);
		PrevPrevValue = PrevValue;
		PrevValue = CurrentValue;
	}
	return true;
}

bool FFibonacciLogHasErrors::RunTest(const FString& Parameters)
{

	AddInfo("Fibonacci errors testing");
	AddExpectedError("Invalid input for Fibonacci", EAutomationExpectedErrorFlags::Contains, 1);
	UScienceFuncLib::Fibonacci(-10);
	return true;
}


#endif