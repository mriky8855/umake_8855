#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Characters/AICombatSlots/CombatSlotManager.h"
#include "Kismet/GameplayStatics.h"
#include "AIStateMachineComponent.generated.h"


class AH2AICharacter;

class ACombatSlotManager;

UENUM(BlueprintType)
enum class EAIState : uint8
{
    Idle,
    Detect,
    Equip,
    Follow,
    Sprint,
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
		FActorComponentTickFunction* ThisTickFunction) override;

private:

	AH2AICharacter* OwnerAI;

	EAIState CurrentState;

	AActor* TargetActor;

public:

	void SetTarget(AActor* Actor);
	void ClearTarget(AActor* Actor);

    void UpdateCombatFocus();

	/* Blueprint Tunables */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float AttackRange = 120.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float AcceptanceRadius = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float AttackCooldown = 1.5f;

	ACombatSlotManager* SlotManager = nullptr;

	bool bHasCombatSlot = false;

	int32 CombatSlotIndex = -1;

private:

	float LastAttackTime = -100.f;

	void RotateTowardsTarget();

	
};