#include "Characters/AIPerceptionComponent/AIPerceptionHandler.h"

#include "Characters/AICharacter/H2AICharacter.h"
#include "Characters/AIStateMachine/AIStateMachineComponent.h"

#include "Perception/AIPerceptionComponent.h"

UAIPerceptionHandler::UAIPerceptionHandler()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAIPerceptionHandler::BeginPlay()
{
	Super::BeginPlay();

	AH2AICharacter* OwnerAI = Cast<AH2AICharacter>(GetOwner());

	if(!OwnerAI)
		return;

	Perception = OwnerAI->Perception;

	if(!Perception)
		return;

	Perception->OnTargetPerceptionUpdated.AddDynamic(
		this,
		&UAIPerceptionHandler::OnTargetDetected
	);
}

void UAIPerceptionHandler::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	AH2AICharacter* OwnerAI = Cast<AH2AICharacter>(GetOwner());
	if (!OwnerAI) return;

	AH2AICharacter* TargetAI = Cast<AH2AICharacter>(Actor);

	if (!OwnerAI->StateMachine) return;

	// FRIENDLY AI: only attack enemies
	if (OwnerAI->AIType == EAIType::Friendly)
	{
		if (!TargetAI || TargetAI->AIType != EAIType::Enemy)
			return;
	}

	// ENEMY AI: priority handled in state machine, but allow player
	if (Stimulus.WasSuccessfullySensed())
	{
		OwnerAI->StateMachine->SetTarget(Actor);
	}
	else
	{
		OwnerAI->StateMachine->ClearTarget(Actor);
	}
}