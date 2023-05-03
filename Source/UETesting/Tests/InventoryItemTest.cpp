


#if (WITH_DEV_AUTOMATION_TESTS || WITH_PERF_AUTOMATION_TESTS)
#include "InventoryItemTest.h"
#include "UETesting/Items/TPSInventoryItem.h"
#include "MISC/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "TestUtils.h"
#include "Components/TextRenderComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../MyProjectCharacter.h"
#include "UETesting/Components/TPSInventoryComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTestActorCanNotBeCreatedItem, "TPSGame.Items.Inventory.CppActorCanNotBeCreated",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority);


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTestBlueprintShouldBeSetupCorrectly, "TPSGame.Items.Inventory.BlueprintShouldBeSetupCorrectly",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryDataShouldBeSetupCorrectly, "TPSGame.Items.Inventory.InventoryDataShouldBeSetupCorrectly",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryCanBeTaken, "TPSGame.Items.Inventory.InventoryCanBeTaken",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority);

///World'/Game/TestingMap.TestingMap'
namespace{

	const char* InventoryItemBPName = "/Script/Engine.Blueprint'/Game/MyTPSInventoryItem.MyTPSInventoryItem'";
	const char* InventoryItemTestBPName = "Blueprint'/Game/Tests/BP_Test_TPSInventoryComponent.BP_Test_TPSInventoryComponent'";

	
}

using namespace TPS::Test;

bool FTestActorCanNotBeCreatedItem::RunTest(const FString& Parameters)
{
	const FString ExpectedWarningMessage = FString::Printf(TEXT("SpawnActor failed because class %s is abstract"), *ATPSInventoryItem::StaticClass()->GetName());
	AddExpectedError(ExpectedWarningMessage, EAutomationExpectedErrorFlags::Exact);
	LevelScope("/Game/Tests/EmptyTestLevel");
	
	UWorld* World = GetTestGameWorld();
	if(!TestNotNull("Inventory world exist",World))
		return false;

	const FTransform InitialTransform(FVector(1000.f));
	const ATPSInventoryItem* InvItem = World->SpawnActor<ATPSInventoryItem>(ATPSInventoryItem::StaticClass(), InitialTransform);
	if(!TestNull("Inventory item exist", InvItem))
		return false;
	
	return true;
}

bool FTestBlueprintShouldBeSetupCorrectly::RunTest(const FString& Parameters)
{
	LevelScope("/Game/Tests/EmptyTestLevel");
	
	UWorld* World = GetTestGameWorld();
	if(!TestNotNull("Inventory world exist",World))
		return false;

	
	
	const FTransform InitialTransform(FVector(1000.f));
	const ATPSInventoryItem* InvItem = CreateBlueprint<ATPSInventoryItem>(World, InventoryItemBPName, InitialTransform);
	if(!TestNotNull("Inventory item exist", InvItem))
		return false;

	const auto CollisionComp = InvItem->FindComponentByClass<USphereComponent>();

	if(!TestNotNull("Collision exist", CollisionComp))
		return false;

	TestTrueExpr(CollisionComp->GetUnscaledSphereRadius() >= 30.0f);
	TestTrueExpr(CollisionComp->GetCollisionEnabled() == ECollisionEnabled::QueryOnly);
	TestTrueExpr(CollisionComp->GetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic) == ECollisionResponse::ECR_Overlap);
	TestTrueExpr(CollisionComp->GetGenerateOverlapEvents());
	TestTrueExpr(InvItem->GetRootComponent() == CollisionComp);
	ENUM_LOOP_START(ECollisionChannel, EElem)
	if(EElem != ECollisionChannel::ECC_OverlapAll_Deprecated)
		TestTrueExpr(CollisionComp->GetCollisionResponseToChannel(EElem) == ECollisionResponse::ECR_Overlap);
	ENUM_LOOP_END

	const auto TextRendererComp = InvItem->FindComponentByClass<UTextRenderComponent>();
	if(!TestNotNull("TextRendererComp exist", TextRendererComp))
		return false;

	const auto StaticMeshComp = InvItem->FindComponentByClass<UStaticMeshComponent>();
	if(!TestNotNull(" StaticMeshComp exist",  StaticMeshComp))
		return false;
	TestTrueExpr( StaticMeshComp->GetCollisionEnabled() == ECollisionEnabled::NoCollision);
	return true;
}

bool FInventoryDataShouldBeSetupCorrectly::RunTest(const FString& Parameters)
{

	LevelScope("/Game/Tests/EmptyTestLevel");
	
	UWorld* World = GetTestGameWorld();
	if(!TestNotNull("Inventory world exist",World))
		return false;
	const FTransform InitialTransform(FVector(1000.f));
	ATPSInventoryItem* InvItem = CreateBlueprint<ATPSInventoryItem>(World, InventoryItemTestBPName, InitialTransform);
	if(!TestNotNull("Inventory item exist", InvItem))
		return false;
	FVector Vector(120.f,130.f,140.f);
	const int32 Score = 13;
	const FInventoryData InvData{EInventoryItemType::CYLINDER, Score};
	const FLinearColor Color = FLinearColor::Yellow;
	CallFuncByNameWithParams(InvItem,"SetInventoryData",
		{
			InvData.ToString(),
			Color.ToString()});
	const auto TextRendererComp = InvItem->FindComponentByClass<UTextRenderComponent>();
	if(!TestNotNull("TextRendererComp exist", TextRendererComp))
		return false;
	TestTrueExpr(TextRendererComp->Text.ToString().Equals(FString::FromInt(Score)));
	TestTrueExpr(TextRendererComp->TextRenderColor == Color.ToFColor(true));

	const auto StaticMeshComp = InvItem->FindComponentByClass<UStaticMeshComponent>();
	if(!TestNotNull(" StaticMeshComp exist",  StaticMeshComp))
		return false;
	const auto Material = StaticMeshComp->GetMaterial(0);
	if(!TestNotNull("Material exist",Material))
		return false;
	FLinearColor MaterialColor;
	Material->GetVectorParameterValue(FHashedMaterialParameterInfo{"Color"}, MaterialColor);
	TestTrueExpr(MaterialColor == Color);
	return true;
}


bool FInventoryCanBeTaken::RunTest(const FString& Parameters)
{
	LevelScope("/Game/Tests/EmptyTestLevel");
	
	UWorld* World = GetTestGameWorld();
	if(!TestNotNull("Inventory world exist",World))
		return false;
	const FTransform InitialTransform(FVector(1000.f));
	ATPSInventoryItem* InvItem = CreateBlueprint<ATPSInventoryItem>(World, InventoryItemTestBPName, InitialTransform);
	if(!TestNotNull("Inventory item exist", InvItem))
		return false;
	FVector Vector(120.f,130.f,140.f);
	const int32 Score = 13;
	const EInventoryItemType Type = EInventoryItemType::CYLINDER;
	const FInventoryData InvData{Type, Score};
	const FLinearColor Color = FLinearColor::Yellow;
	CallFuncByNameWithParams(InvItem,"SetInventoryData",
		{
			InvData.ToString(),
			Color.ToString()});

	TArray<AActor*> Pawns;
	UGameplayStatics::GetAllActorsOfClass(World, AMyProjectCharacter::StaticClass(), Pawns);
	if(!TestTrueExpr(Pawns.Num() == 1))
		return false;
	const auto Character = Cast<AMyProjectCharacter>(Pawns[0]);
	if(!TestNotNull("Character exist",Character))
		return false;
	const auto InvComp = Character->FindComponentByClass<UTPSInventoryComponent>();
	if(!TestNotNull("Inventory exist", InvComp))
		return false;
	TestTrueExpr(InvComp->GetInventoryAmountByType(Type)==0);

	//Character takes inventory item
	Character->SetActorLocation(InitialTransform.GetLocation());
	TestTrueExpr(InvComp->GetInventoryAmountByType(Type) == Score);
	TestTrueExpr(!IsValid(InvItem));

	TArray<AActor*> InvItems;
	UGameplayStatics::GetAllActorsOfClass(World, ATPSInventoryItem::StaticClass(), InvItems);
	TestTrueExpr(InvItems.IsEmpty());
	return true;
}


#endif