﻿


#if WITH_AUTOMATION_TESTS 

#include "TPSCharacterTests.h"
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "TestUtils.h"
#include "UETesting/MyProjectCharacter.h"
#include "Engine/World.h"
#include "../TPSTypes.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"


DEFINE_LOG_CATEGORY_STATIC(LogTPSCharacterTests, All, All);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHealthMightBeChangedWithDamage, "TPSGame.Character.HealthMightBeChangedWithDamage",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLatentCommandSimpleWait, "TPSGame.LatentCommand.SimpleWait",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLatentCommandSimpleLog, "TPSGame.LatentCommand.SimpleLog",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLatentCommandOpenCloseMap, "TPSGame.LatentCommand.OpenCloseMap",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCharacterCanBeKilled, "TPSGame.Character.CharacterCanBeKilled",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAutoHealShouldRestoreHealth, "TPSGame.Character.AutoHealShouldRestoreHealth",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority);

using namespace TPS::Test;

namespace{

	const char* CharacterBPName = "Blueprint'/Game/Tests/BP_TestProjectCharacter_Child.BP_TestProjectCharacter_Child'";
}

bool FHealthMightBeChangedWithDamage::RunTest(const FString& Parameters)
{
	LevelScope("/Game/Tests/EmptyTestLevel");
	UWorld* World = GetTestGameWorld();
	if(!TestNotNull("Inventory world exist",World))
		return false;

	const FTransform InitialTransform(FVector(0.0f, -240.0f, 110.0f));
	AMyProjectCharacter* Character = CreateBlueprintDeferred<AMyProjectCharacter>(World, CharacterBPName, InitialTransform);
	if(!TestNotNull("Character exist", Character))
		return false;
	
	FHealthData HealthData;
	HealthData.MaxHealth = 1000.0f;

	CallFuncByNameWithParams(Character, "SetHealthData",{HealthData.ToString()});
	Character->FinishSpawning(InitialTransform);

	TestEqual("Health is full", Character->GetHealthPercent(), 1.0f);
	const float DamageAmount = 10.0f;
	Character->TakeDamage(DamageAmount, FDamageEvent{}, nullptr, nullptr);
	TestEqual("Health was decreased", Character->GetHealthPercent(), 1.0f - DamageAmount/HealthData.MaxHealth);
	return true;
}

bool FLatentCommandSimpleWait::RunTest(const FString& Parameters)
{
	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(3.0f));
	return true;
}

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FTPSLatentLogCommand, FString, LogMessage);
bool FTPSLatentLogCommand::Update()
{
	UE_LOG(LogTPSCharacterTests, Display, TEXT("%s"), *LogMessage);
	return true;
}

bool FLatentCommandSimpleLog::RunTest(const FString& Parameters)
{
	UE_LOG(LogTPSCharacterTests, Display, TEXT("Log 1"));
	ADD_LATENT_AUTOMATION_COMMAND(FTPSLatentLogCommand("Latent log 1"));
	UE_LOG(LogTPSCharacterTests, Display, TEXT("Log 2"));
	ADD_LATENT_AUTOMATION_COMMAND(FTPSLatentLogCommand("Latent log 2"));
	UE_LOG(LogTPSCharacterTests, Display, TEXT("Log 3"));
	return true;
}

bool FLatentCommandOpenCloseMap::RunTest(const FString& Parameters)
{
	const auto Level = LevelScope("/Game/Tests/EmptyTestLevel");
	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(3.0f));
	return true;
}

DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FCharacterDestroyedLatentCommand, AMyProjectCharacter*, Character, float, Lifespan);
bool FCharacterDestroyedLatentCommand::Update()
{
	const double NewTime = FPlatformTime::Seconds();
	if(NewTime - StartTime >= Lifespan)
	{
		if(IsValid(Character))
		{
			UE_LOG(LogTPSCharacterTests, Error, TEXT(" Character wasn't destroyed"));
		}
		return true;
	}
	return false;
}


bool FCharacterCanBeKilled::RunTest(const FString& Parameters)
{
	const auto Level = LevelScope("/Game/Tests/EmptyTestLevel");
	UWorld* World = GetTestGameWorld();
	if(!TestNotNull("Inventory world exist",World))
		return false;

	const FTransform InitialTransform(FVector(0.0f, -240.0f, 110.0f));
	AMyProjectCharacter* Character = CreateBlueprintDeferred<AMyProjectCharacter>(World, CharacterBPName, InitialTransform);
	if(!TestNotNull("Character exist", Character))
		return false;
	
	FHealthData HealthData;
	HealthData.MaxHealth = 1000.0f;
	HealthData.LifeSpan = 1.5f;

	CallFuncByNameWithParams(Character, "SetHealthData",{HealthData.ToString()});
	Character->FinishSpawning(InitialTransform);

	TestEqual("Health is full", Character->GetHealthPercent(), 1.0f);
	const float DamageAmount = 10.0f;
	Character->TakeDamage(DamageAmount, FDamageEvent{}, nullptr, nullptr);
	TestEqual("Health was decreased", Character->GetHealthPercent(), 1.0f - DamageAmount/HealthData.MaxHealth);

	const float KillingDamageAmount = HealthData.MaxHealth;
	Character->TakeDamage(KillingDamageAmount, FDamageEvent{}, nullptr, nullptr);
	TestEqual("Health is empty", Character->GetHealthPercent(), 0.0f);
	TestTrueExpr(Character->GetCharacterMovement()->MovementMode == MOVE_None);

	ENUM_LOOP_START(ECollisionChannel, EElem)
	if(EElem != ECC_OverlapAll_Deprecated)
	{
		TestTrueExpr(Character->GetCapsuleComponent()->GetCollisionResponseToChannel(EElem) == ECollisionResponse::ECR_Ignore);
	}
	ENUM_LOOP_END

	TestTrueExpr(Character->GetMesh()->GetCollisionEnabled() == ECollisionEnabled::QueryAndPhysics);
	TestTrueExpr(Character->GetMesh()->IsSimulatingPhysics());
	TestTrueExpr(FMath::IsNearlyEqual(Character->GetLifeSpan(), HealthData.LifeSpan));
	//ADD_LATENT_AUTOMATION_COMMAND(FCharacterDestroyedLatentCommand(Character,HealthData.LifeSpan));
	ADD_LATENT_AUTOMATION_COMMAND(FDelayedFunctionLatentCommand([Character]()
	{
		if(IsValid(Character))
		{
			UE_LOG(LogTPSCharacterTests, Error, TEXT(" Character wasn't destroyed"));
		}
	},HealthData.LifeSpan));
	return true;
}

DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FAutoHealCheck, AMyProjectCharacter*, Character, float, HealigDuration);
bool FAutoHealCheck::Update()
{
	const double NewTime = FPlatformTime::Seconds();
	if(NewTime - StartTime >= HealigDuration)
	{
		if(!FMath::IsNearlyEqual(Character->GetHealthPercent(),1.0f))
		{
			UE_LOG(LogTPSCharacterTests, Error, TEXT(" Character wasn't heal"));
		}
		return true;
	}
	return false;
}

bool FAutoHealShouldRestoreHealth::RunTest(const FString& Parameters)
{
	const auto Level = LevelScope("/Game/Tests/EmptyTestLevel");
	UWorld* World = GetTestGameWorld();
	if(!TestNotNull("Inventory world exist",World))
		return false;

	const FTransform InitialTransform(FVector(0.0f, -240.0f, 110.0f));
	AMyProjectCharacter* Character = CreateBlueprintDeferred<AMyProjectCharacter>(World, CharacterBPName, InitialTransform);
	if(!TestNotNull("Character exist", Character))
		return false;
	
	FHealthData HealthData;
	HealthData.MaxHealth = 200.0f;
	HealthData.HealModifier = 5.0f;
	HealthData.HealRate = 0.5f;

	CallFuncByNameWithParams(Character, "SetHealthData",{HealthData.ToString()});
	Character->FinishSpawning(InitialTransform);

	TestEqual("Health is full", Character->GetHealthPercent(), 1.0f);
	const float DamageAmount = 20.0f;
	Character->TakeDamage(DamageAmount, FDamageEvent{}, nullptr, nullptr);
	TestEqual("Health was decreased", Character->GetHealthPercent(), 1.0f - DamageAmount/HealthData.MaxHealth);

	const float HealthDiff = HealthData.MaxHealth*(1.0f - Character->GetHealthPercent());
	const float HealingDuration = HealthData.HealRate*HealthDiff/HealthData.HealModifier;
	//ADD_LATENT_AUTOMATION_COMMAND(FAutoHealCheck(Character, HealingDuration));
	ADD_LATENT_AUTOMATION_COMMAND(FDelayedFunctionLatentCommand([Character]()
	{
		if(!FMath::IsNearlyEqual(Character->GetHealthPercent(),1.0f))
		{
			UE_LOG(LogTPSCharacterTests, Error, TEXT(" Character wasn't heal"));
		}
	}, HealingDuration));
	return true;
}


#endif