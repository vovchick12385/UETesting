#include "SandBoxTests.h"
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMathMaxInt, "TPSGame.Math.MaxInt",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMathSqrt, "TPSGame.Math.Sqrt",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority);

bool FMathMaxInt::RunTest(const FString& Parametr)
{
	AddInfo("Max [int] func testing");
	TestTrue("2 different positive numbers", FMath::Max(13,25)==25);
	// 13,25
	// 25, 25
	// 0, 123
	// 0,0
	//-234, 0
	// -543, -233
	// -9, -9
	//-5, 1
	TestEqual("2 equal positive numbers", FMath::Max(25,25) ,25);
	TestTrueExpr(FMath::Max(0,123) == 123);
	
	TestTrue("2 different positive numbers", FMath::Max(0,0)==0);
	
	TestTrue("2 different positive numbers", FMath::Max(-234,0)==0);
	TestTrue("2 different positive numbers", FMath::Max(-543,-233)==-233);
	TestTrue("2 different positive numbers", FMath::Max(-9,-9)==-9);
	TestTrue("2 different positive numbers", FMath::Max(1,-5)==1);
	
	return true;
}

bool FMathSqrt::RunTest(const FString& Parameters)
{
	//sqrt(3) = 1.73205...
	AddInfo("Sqrt function testing");
	TestEqual("Sqrt(4) [0]", FMath::Sqrt(4.0f), 2.0f);
	TestEqual("Sqrt(3) [1]", FMath::Sqrt(3.0f), 1.73205f, 0.001f);	
}