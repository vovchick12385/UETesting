
#if(WITH_DEV_AUTOMATION_TESTS || WITH_PERF_AUTOMATION_TESTS)
#include "TestBattery.h"
#include "CoreMinimal.h"
#include "UETesting/Items/Battery.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTestBattery, "TPSTests.TestBattery",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority);
bool FTestBattery::RunTest(const FString& Parameters)
{
	using namespace TPS;
	AddInfo("Batery with default ctor");
	const Battery BatteyDefault;
	TestTrueExpr(FMath::IsNearlyEqual(BatteyDefault.GetPercent(), 1.0f));
	TestTrueExpr(BatteyDefault.GetColor()== FColor::Green);
	TestTrueExpr(BatteyDefault.ToString().Equals("100%"));

	AddInfo("Batery with custom ctor");
	const Battery BatteyObj1(1.0f);
	TestTrueExpr(FMath::IsNearlyEqual(BatteyObj1.GetPercent(), 1.0f));
	TestTrueExpr(BatteyObj1.GetColor()== FColor::Green);
	TestTrueExpr(BatteyObj1.ToString().Equals("100%"));

	AddInfo("Batery with custom ctor");
	const Battery BatteyObj2(0.46f);
	TestTrueExpr(FMath::IsNearlyEqual(BatteyObj2.GetPercent(), 0.46f));
	TestTrueExpr(BatteyObj2.GetColor()== FColor::Yellow);
	TestTrueExpr(BatteyObj2.ToString().Equals("46%"));

	const auto BatteryTestFunc = [this](float Percent, const FColor& color, const FString& PercentString)
	{
		const Battery BatteryObject(Percent);
		TestTrueExpr(FMath::IsNearlyEqual(BatteryObject.GetPercent(), FMath::Clamp(Percent, 0.0f, 1.0f),UE_KINDA_SMALL_NUMBER));
		TestTrueExpr(BatteryObject.GetColor() == color);
		TestTrueExpr(BatteryObject.ToString().Equals(PercentString));
	};

	BatteryTestFunc(1.0f, FColor::Green, "100%");
	BatteryTestFunc(0.46f, FColor::Yellow, "46%");
	BatteryTestFunc(0.2f, FColor::Red, "20%");
	BatteryTestFunc(300.0f, FColor::Green, "100%");
	BatteryTestFunc(-1.0f, FColor::Red, "0%");

	AddInfo("Batter charge / uncharge");
	Battery BatteryObj3(0.6f);
	TestTrueExpr(FMath::IsNearlyEqual(BatteryObj3.GetPercent(), 0.6f,UE_KINDA_SMALL_NUMBER));
	BatteryObj3.Charge();
	TestTrueExpr(FMath::IsNearlyEqual(BatteryObj3.GetPercent(), 0.7f,UE_KINDA_SMALL_NUMBER));
	BatteryObj3.UnCharge();
	TestTrueExpr(FMath::IsNearlyEqual(BatteryObj3.GetPercent(), 0.6f,UE_KINDA_SMALL_NUMBER));


	AddInfo("Batter charge / uncharge corner cases");
	for(int32 i = 0; i <10;++i)
	{
		BatteryObj3.Charge();
	}
	TestTrueExpr(FMath::IsNearlyEqual(BatteryObj3.GetPercent(), 1.0f));
	for(int32 i = 0; i <10;++i)
	{
		BatteryObj3.UnCharge();
	}
	TestTrueExpr(FMath::IsNearlyEqual(BatteryObj3.GetPercent(), 0.0f));

	AddInfo("Battery comparison");
	Battery BatteryLow(0.3);
	Battery BatteryHigh(0.9f);

	TestTrueExpr(BatteryHigh >= BatteryLow);
	TestTrueExpr(FMath::Max(BatteryHigh, BatteryLow) == BatteryHigh);

	AddInfo("Battery in memory comparison");
	TestNotSame("Not the same batteries", BatteryHigh, BatteryLow);
	Battery BatteryHighDup(0.9f);
	TestTrueExpr(BatteryHigh == BatteryHighDup);
	TestNotSame("Not the same batteries", BatteryHigh, BatteryHighDup);
	const Battery& BatteryRef = BatteryHigh;
	TestSame("Same batteries", BatteryHigh, BatteryRef);
	return true;
}

#endif