#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AICombatComponent.generated.h"

class UCombatComponent;
class AH2Character;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HERETIC2REMAKE_API UAICombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    UAICombatComponent();

    void EquipDagger();
    void Attack();
    void Unequip();

private:

    AH2Character* OwnerCharacter;
    UCombatComponent* CombatComponent;

};