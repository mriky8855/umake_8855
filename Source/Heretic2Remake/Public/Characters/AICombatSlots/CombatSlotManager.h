#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatSlotManager.generated.h"

UCLASS()
class HERETIC2REMAKE_API ACombatSlotManager : public AActor
{
	GENERATED_BODY()

public:

	ACombatSlotManager();

protected:

	virtual void BeginPlay() override;

private:

	TMap<AActor*, TArray<AActor*>> ActiveSlots;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat Slots")
	int32 MaxAttackers = 2;

	int32 RequestSlot(AActor* Target, AActor* Attacker);

	void ReleaseSlot(AActor* Target, AActor* Attacker);

	FVector GetOrbitLocation(AActor* Target, int32 SlotIndex, float Radius);
};