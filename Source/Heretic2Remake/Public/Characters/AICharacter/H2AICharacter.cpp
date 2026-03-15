#include "Characters/AICharacter/H2AICharacter.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

#include "Characters/AIPerceptionComponent/AIPerceptionHandler.h"
#include "Characters/AIStateMachine/AIStateMachineComponent.h"
#include "Characters/AICombatComponent/AICombatComponent.h"

AH2AICharacter::AH2AICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	StateMachine = CreateDefaultSubobject<UAIStateMachineComponent>("StateMachine");

	AICombat = CreateDefaultSubobject<UAICombatComponent>("AICombat");

	PerceptionHandler = CreateDefaultSubobject<UAIPerceptionHandler>("PerceptionHandler");

	Perception = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerception");

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("SightConfig");

	SightConfig->SightRadius = 2000.f;
	SightConfig->LoseSightRadius = 2300.f;
	SightConfig->PeripheralVisionAngleDegrees = 70.f;

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	Perception->ConfigureSense(*SightConfig);
	Perception->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AH2AICharacter::BeginPlay()
{
	Super::BeginPlay();
}