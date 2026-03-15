#include "Characters/AIStateMachine/AIStateMachineComponent.h"
#include "Characters/AICharacter/H2AICharacter.h"
#include "Characters/AICombatComponent/AICombatComponent.h"

#include "AIController.h"

UAIStateMachineComponent::UAIStateMachineComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAIStateMachineComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerAI = Cast<AH2AICharacter>(GetOwner());

	CurrentState = EAIState::Idle;
}

void UAIStateMachineComponent::SetTarget(AActor* Actor)
{
	TargetActor = Actor;

	CurrentState = EAIState::Detect;
}

void UAIStateMachineComponent::ClearTarget(AActor* Actor)
{
	if (TargetActor == Actor)
	{
		TargetActor = nullptr;

		CurrentState = EAIState::Idle;
	}
}

void UAIStateMachineComponent::RotateTowardsTarget()
{
	if (!OwnerAI || !TargetActor) return;

	AAIController* AIController = Cast<AAIController>(OwnerAI->GetController());
	if (!AIController) return;

	FVector Direction = TargetActor->GetActorLocation() - OwnerAI->GetActorLocation();
	Direction.Z = 0;

	FRotator TargetRotation = Direction.Rotation();

	FRotator NewRotation = FMath::RInterpTo(
		AIController->GetControlRotation(),
		TargetRotation,
		GetWorld()->GetDeltaSeconds(),
		8.f
	);

	AIController->SetControlRotation(NewRotation);
}

void UAIStateMachineComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!OwnerAI) return;

	AAIController* AIController = Cast<AAIController>(OwnerAI->GetController());
	if (!AIController) return;

	switch (CurrentState)
	{

	case EAIState::Idle:
		break;

	case EAIState::Detect:

		OwnerAI->AICombat->EquipDagger();

		CurrentState = EAIState::Follow;

		break;

	case EAIState::Follow:

		if (!TargetActor) return;

		RotateTowardsTarget();

		AIController->MoveToActor(TargetActor, AcceptanceRadius);

		if (FVector::Dist(
			OwnerAI->GetActorLocation(),
			TargetActor->GetActorLocation()) <= AttackRange)
		{
			CurrentState = EAIState::Attack;
		}

		break;

	case EAIState::Attack:

	{
		float Time = GetWorld()->GetTimeSeconds();

		if (Time - LastAttackTime >= AttackCooldown)
		{
			RotateTowardsTarget();

			OwnerAI->AICombat->Attack();

			LastAttackTime = Time;

			CurrentState = EAIState::Wait;
		}

		break;
	}

	case EAIState::Wait:

		if (TargetActor)
		{
			RotateTowardsTarget();
		}

		if (FVector::Dist(
			OwnerAI->GetActorLocation(),
			TargetActor->GetActorLocation()) > AttackRange)
		{
			CurrentState = EAIState::Follow;
		}
		else
		{
			CurrentState = EAIState::Attack;
		}

		break;

	default:
		break;
	}
}