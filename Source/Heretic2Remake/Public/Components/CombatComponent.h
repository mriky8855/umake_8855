#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AWeapon;
class AH2Character;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HERETIC2REMAKE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void SetWeapon(AWeapon* NewWeapon);
	void HandlePrimaryAttack(AH2Character* Character);

private:
	void PerformMeleeAttack(AH2Character* Character);
	void PerformShootingAttack(AH2Character* Character);

private:
	UPROPERTY()
	AWeapon* CurrentWeapon;
};