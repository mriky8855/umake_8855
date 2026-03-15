#pragma once

#include "CoreMinimal.h"
#include "Characters/H2Character.h"
#include "H2AICharacter.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAIStateMachineComponent;
class UAICombatComponent;
class UAIPerceptionHandler;

UENUM(BlueprintType)
enum class EAIType : uint8
{
	Enemy,
	Friendly,
	Neutral
};



UCLASS()
class HERETIC2REMAKE_API AH2AICharacter : public AH2Character
{
	GENERATED_BODY()

public:

	AH2AICharacter();

protected:

	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAIType AIType;

	/* PERCEPTION */

	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* Perception;

	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;

	/* AI SYSTEMS */

	UPROPERTY(VisibleAnywhere)
	UAIStateMachineComponent* StateMachine;

	UPROPERTY(VisibleAnywhere)
	UAICombatComponent* AICombat;

	UPROPERTY(VisibleAnywhere)
	UAIPerceptionHandler* PerceptionHandler;

	/* AI Characteristics */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Traits")
	bool bCanSprint = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Traits")
	float SprintSpeed = 650.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Traits")
	float CombatSpeed = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Traits")
	float SprintDistance = 800.f;


};