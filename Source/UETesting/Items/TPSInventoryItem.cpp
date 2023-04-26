#include "TPSInventoryItem.h"
#include "GameFramework/Pawn.h"
#include "Components/SphereComponent.h"
#include "UETesting/Components/TPSInventoryComponent.h"


ATPSInventoryItem::ATPSInventoryItem()
{
 	
	PrimaryActorTick.bCanEverTick = false;
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(CollisionComponent);
	CollisionComponent->SetSphereRadius(30.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	CollisionComponent->SetGenerateOverlapEvents(true);
}

void ATPSInventoryItem::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if(const auto Pawn = Cast<APawn>(OtherActor))
	{
		if(const auto InvComp = Pawn->FindComponentByClass<UTPSInventoryComponent>())
		{
			if(InvComp->TryToAddItem(InventoryData))
				Destroy();
		}
		
	}
}



