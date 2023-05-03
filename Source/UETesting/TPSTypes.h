#pragma once

#include "CoreMinimal.h"
#include "TPSTypes.generated.h"

UENUM(BlueprintType)
enum class EInventoryItemType: uint8
{
	SPHERE =0 UMETA(DisplayName = "My cool Sphere"),
	CUBE UMETA(DisplayName = "My cool Cube"),
	CYLINDER,
	CONE
};

USTRUCT(BlueprintType)
struct FInventoryData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EInventoryItemType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
	int32 Score;

	FString ToString() const
	{
		return FString::Printf(TEXT("(Type=%i,Score=%i)"),Type,Score);
	}
};