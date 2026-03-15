#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIStateMachineComponent.generated.h"

class AH2AICharacter;

UENUM(BlueprintType)
enum class EAIState : uint8
{
    Idle,
    Detect,
    Equip,
    Follow,
    Attack,
    Wait,
    Unequip
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HERETIC2REMAKE_API UAIStateMachineComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    UAIStateMachineComponent();

protected:

    virtual void BeginPlay() override;
    virtual void TickComponent(
        float DeltaTime,
        ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction
    ) override;

private:

    AH2AICharacter* OwnerAI;

    EAIState CurrentState;

    AActor* TargetActor;

    float AttackRange;

public:

    void SetTarget(AActor* Actor);
    void ClearTarget(AActor* Actor);

};