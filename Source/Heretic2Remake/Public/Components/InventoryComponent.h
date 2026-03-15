#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class AWeapon;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HERETIC2REMAKE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void AddWeapon(AWeapon* Weapon);
	void EquipWeapon(AWeapon* Weapon);

	UPROPERTY()
	TArray<AWeapon*> Weapons;

	UPROPERTY()
	AWeapon* EquippedWeapon;
};