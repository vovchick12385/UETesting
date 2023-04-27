#include "TPSInventoryComponent.h"

UTPSInventoryComponent::UTPSInventoryComponent()
{
	
	PrimaryComponentTick.bCanEverTick = false;

	
}

void UTPSInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
#if !UE_BUILD_SHIPPING
	const auto InvItem = StaticEnum<EInventoryItemType>();
	for(int32 i =0; i< InvItem->NumEnums()-1; ++i)
	{
		const EInventoryItemType EnumElem = static_cast<EInventoryItemType>(i);
		const FString EnumElemName = UEnum::GetValueAsString(EnumElem);
		const auto LimitCheckCond = InventoryLimits.Contains(EnumElem) && InventoryLimits[EnumElem] >= 0;
		checkf(LimitCheckCond, TEXT("Limit for %s doesn't exist or less then zero"), *EnumElemName);
	}
#endif
}


bool UTPSInventoryComponent::TryToAddItem(const FInventoryData& InventoryData)
{
	if(InventoryData.Score < 0)
		return false;
	if(!Inventory.Contains(InventoryData.Type))
	{
		Inventory.Add(InventoryData.Type, 0);
	}
	const auto NextScore = Inventory[InventoryData.Type] + InventoryData.Score;
	Inventory[InventoryData.Type] = NextScore > InventoryLimits[InventoryData.Type] ? Inventory[InventoryData.Type] : NextScore;
	return true;
}

int32 UTPSInventoryComponent::GetInventoryAmountByType(EInventoryItemType Type) const
{
	return Inventory.Contains(Type) ? Inventory[Type] : 0;
}

