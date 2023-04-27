
#if(WITH_DEV_AUTOMATION_TESTS || WITH_PERF_AUTOMATION_TESTS)

#include "InventoryTest.h"
#include "TestUtils.h"
#include "CoreMinimal.h"
#include "UETesting/Components/TPSInventoryComponent.h"


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FComponentTest, "TPSGame.Components.Inventory.ComponentCouldBeCreated",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FItemScoreShouldBeZerosDefaultTest, "TPSGame.Components.Inventory.ScoreShouldBeZerosDefault",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNegativeItemShouldBeAdded, "TPSGame.Components.Inventory.NegativeItemShouldBeAdded",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPositiveItemShouldBeAdded, "TPSGame.Components.Inventory.PositiveItemShouldBeAdded",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPositiveItemShouldNotBeAdded, "TPSGame.Components.Inventory.PositiveItemShouldNotBeAdded",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority);

namespace
{
	class UTPSInventoryComponentTestable: public UTPSInventoryComponent
	{
	public:
		void SetLimits(const TMap<EInventoryItemType, int32>& Limits)
		{
			InventoryLimits = Limits;
		}
	};
}
bool FComponentTest::RunTest(const FString& Parameters)
{
	const UTPSInventoryComponent* InvComponetn = NewObject<UTPSInventoryComponent>();
	TestNotNull("Test InventoryComponent",InvComponetn);
	return true;
}

bool FItemScoreShouldBeZerosDefaultTest::RunTest(const FString& Parameters)
{
	const UTPSInventoryComponent* InvComponetn = NewObject<UTPSInventoryComponent>();
	if(!TestNotNull("Test InventoryComponent",InvComponetn))
		return false;
	TestTrueExpr(InvComponetn->GetInventoryAmountByType(EInventoryItemType::CONE) == 0);
	TestTrueExpr(InvComponetn->GetInventoryAmountByType(EInventoryItemType::CUBE) == 0);
	TestTrueExpr(InvComponetn->GetInventoryAmountByType(EInventoryItemType::SPHERE) == 0);
	TestTrueExpr(InvComponetn->GetInventoryAmountByType(EInventoryItemType::CYLINDER) == 0);

	ENUM_LOOP_START(EInventoryItemType, EnumElem)
		TestTrueExpr(InvComponetn->GetInventoryAmountByType(EnumElem) == 0);
	ENUM_LOOP_END
	
	return true;
}

bool FNegativeItemShouldBeAdded::RunTest(const FString& Parameters)
{
	UTPSInventoryComponentTestable* InvComponetn = NewObject<UTPSInventoryComponentTestable>();
	if(!TestNotNull("Test InventoryComponent",InvComponetn))
		return false;
	const int32 NegativeScoreAmount = -35;
	TMap<EInventoryItemType, int32> InvLimits;
	const int32 ScoreLimit = 100;
	const int32 InitializeScoreAmount = 10;
	ENUM_LOOP_START(EInventoryItemType, EnumElem)
		InvLimits.Add(EnumElem, ScoreLimit);
	ENUM_LOOP_END
	InvComponetn->SetLimits(InvLimits);
	
	ENUM_LOOP_START(EInventoryItemType, EnumElem)
		TestTrueExpr(InvComponetn->TryToAddItem({EnumElem,InitializeScoreAmount}));
		TestTrueExpr(InvComponetn->GetInventoryAmountByType(EnumElem) == InitializeScoreAmount);
		TestTrueExpr(InvComponetn->TryToAddItem({EnumElem,NegativeScoreAmount}));
		TestTrueExpr(InvComponetn->GetInventoryAmountByType(EnumElem) == InitializeScoreAmount);
	ENUM_LOOP_END
	return true;
}

bool FPositiveItemShouldBeAdded::RunTest(const FString& Parameters)
{
	UTPSInventoryComponentTestable* InvComponetn = NewObject<UTPSInventoryComponentTestable>();
	if(!TestNotNull("Test InventoryComponent",InvComponetn))
		return false;
	
	TMap<EInventoryItemType, int32> InvLimits;
	
	const int32 ScoreLimit = 100;
	ENUM_LOOP_START(EInventoryItemType, EnumElem)
		InvLimits.Add(EnumElem, ScoreLimit);
	ENUM_LOOP_END
	InvComponetn->SetLimits(InvLimits);
	
	ENUM_LOOP_START(EInventoryItemType, EnumElem)
	for(int32 i =0;i<InvLimits[EnumElem];++i)
	{
		TestTrueExpr(InvComponetn->TryToAddItem({EnumElem,1}));
		TestTrueExpr(InvComponetn->GetInventoryAmountByType(EnumElem) == i+1);
	}
	ENUM_LOOP_END
	return true;
}

bool FPositiveItemShouldNotBeAdded::RunTest(const FString& Parameters)
{
	UTPSInventoryComponentTestable* InvComponetn = NewObject<UTPSInventoryComponentTestable>();
	if(!TestNotNull("Test InventoryComponent",InvComponetn))
		return false;
	
	TMap<EInventoryItemType, int32> InvLimits;
	
	const int32 ScoreLimit = 100;
	InvLimits.Add(EInventoryItemType::SPHERE, ScoreLimit);
	InvComponetn->SetLimits(InvLimits);

	TestTrueExpr(InvComponetn->TryToAddItem({EInventoryItemType::SPHERE,10}));
	TestTrueExpr(InvComponetn->GetInventoryAmountByType(EInventoryItemType::SPHERE) == 10);

	TestTrueExpr(InvComponetn->TryToAddItem({EInventoryItemType::SPHERE,ScoreLimit+1}));
	TestTrueExpr(InvComponetn->GetInventoryAmountByType(EInventoryItemType::SPHERE) == 10);
	
	return true;
}



#endif