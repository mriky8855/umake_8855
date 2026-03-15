#include "Characters/AIStateMachine/AIStateMachineComponent.h"

#include "Characters/AICharacter/H2AICharacter.h"
#include "Characters/AICombatComponent/AICombatComponent.h"

#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"

UAIStateMachineComponent::UAIStateMachineComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAIStateMachineComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerAI = Cast<AH2AICharacter>(GetOwner());

	CurrentState = EAIState::Idle;

	TargetActor = nullptr;
}

void UAIStateMachineComponent::SetTarget(AActor* Actor)
{
	if (!Actor) return;

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

void UAIStateMachineComponent::UpdateCombatFocus()
{
	if (!OwnerAI || !TargetActor) return;

	FVector Direction = TargetActor->GetActorLocation() - OwnerAI->GetActorLocation();

	Direction.Z = 0;

	FRotator TargetRotation = Direction.Rotation();

	FRotator NewRotation = FMath::RInterpTo(
		OwnerAI->GetActorRotation(),
		TargetRotation,
		GetWorld()->GetDeltaSeconds(),
		10.f
	);

	OwnerAI->SetActorRotation(NewRotation);
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

	/* Always face combat target */
	if (TargetActor)
	{
		UpdateCombatFocus();
	}

	switch (CurrentState)
	{

	/* ----------------------------------------------------- */
	case EAIState::Idle:

		break;

	/* ----------------------------------------------------- */
	case EAIState::Detect:

		OwnerAI->AICombat->EquipDagger();

		if (OwnerAI->GetCharacterMovement())
		{
			OwnerAI->GetCharacterMovement()->MaxWalkSpeed = OwnerAI->CombatSpeed;
		}

		CurrentState = EAIState::Follow;

		break;

	/* ----------------------------------------------------- */
	case EAIState::Equip:

		OwnerAI->AICombat->EquipDagger();

		if (OwnerAI->GetCharacterMovement())
		{
			OwnerAI->GetCharacterMovement()->MaxWalkSpeed = OwnerAI->CombatSpeed;
		}

		CurrentState = EAIState::Follow;

		break;

	/* ----------------------------------------------------- */
	case EAIState::Follow:

		if (!TargetActor) return;

	{
		float Distance = FVector::Dist(
			OwnerAI->GetActorLocation(),
			TargetActor->GetActorLocation());

		/* Too far → sprint */
		if (OwnerAI->bCanSprint && Distance > OwnerAI->SprintDistance)
		{
			CurrentState = EAIState::Unequip;
			break;
		}

		AIController->MoveToActor(TargetActor, AcceptanceRadius);

		if (Distance <= AttackRange)
		{
			CurrentState = EAIState::Attack;
		}
	}

	break;

	/* ----------------------------------------------------- */
	case EAIState::Unequip:

		OwnerAI->AICombat->UnequipWeapon();

		if (OwnerAI->GetCharacterMovement())
		{
			OwnerAI->GetCharacterMovement()->MaxWalkSpeed = OwnerAI->SprintSpeed;
		}

		CurrentState = EAIState::Sprint;

		break;

	/* ----------------------------------------------------- */
	case EAIState::Sprint:

		if (!TargetActor) return;

	{
		AIController->MoveToActor(TargetActor, AcceptanceRadius);

		float Distance = FVector::Dist(
			OwnerAI->GetActorLocation(),
			TargetActor->GetActorLocation());

		if (Distance <= OwnerAI->SprintDistance)
		{
			CurrentState = EAIState::Equip;
		}
	}

	break;

	/* ----------------------------------------------------- */
	case EAIState::Attack:

	{
		float Time = GetWorld()->GetTimeSeconds();

		if (Time - LastAttackTime >= AttackCooldown)
		{
			OwnerAI->AICombat->Attack();

			LastAttackTime = Time;

			CurrentState = EAIState::Wait;
		}
	}

	break;

	/* ----------------------------------------------------- */
	case EAIState::Wait:

		if (!TargetActor) return;

	{
		float Distance = FVector::Dist(
			OwnerAI->GetActorLocation(),
			TargetActor->GetActorLocation());

		if (Distance > AttackRange)
		{
			CurrentState = EAIState::Follow;
		}
		else
		{
			CurrentState = EAIState::Attack;
		}
	}

	break;

	/* ----------------------------------------------------- */
	default:
		break;
	}
}