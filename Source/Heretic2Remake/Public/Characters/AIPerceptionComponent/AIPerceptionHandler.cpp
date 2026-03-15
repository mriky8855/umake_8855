#include "Characters/AIPerceptionComponent/AIPerceptionHandler.h"

#include "Characters/AICharacter/H2AICharacter.h"
#include "Characters/H2Character.h"
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
	if (!OwnerAI)
		return;

	Perception = OwnerAI->Perception;
	if (!Perception)
		return;

	Perception->OnTargetPerceptionUpdated.AddDynamic(
		this,
		&UAIPerceptionHandler::OnTargetDetected
	);
}

void UAIPerceptionHandler::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	AH2AICharacter* OwnerAI = Cast<AH2AICharacter>(GetOwner());
	if (!OwnerAI)
		return;

	if (!OwnerAI->StateMachine)
		return;

	AH2Character* OwnerChar = Cast<AH2Character>(OwnerAI);
	AH2Character* TargetChar = Cast<AH2Character>(Actor);

	if (!OwnerChar || !TargetChar)
		return;

	/* ---------- TEAM FILTER ---------- */

	if (OwnerChar->TeamID == TargetChar->TeamID)
	{
		return;
	}

	/* ---------- PERCEPTION RESULT ---------- */

	if (Stimulus.WasSuccessfullySensed())
	{
		OwnerAI->StateMachine->SetTarget(Actor);
	}
	else
	{
		OwnerAI->StateMachine->ClearTarget(Actor);
	}
}