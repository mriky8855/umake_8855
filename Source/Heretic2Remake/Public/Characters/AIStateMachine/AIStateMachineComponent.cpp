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

	if (!OwnerAI) return;

	/* Find Combat Slot Manager */
	SlotManager = Cast<ACombatSlotManager>(
		UGameplayStatics::GetActorOfClass(
			GetWorld(),
			ACombatSlotManager::StaticClass()
		)
	);

	/* Friendly AI follows player automatically */
	if (OwnerAI->TeamID == ETeamID::Friendly)
	{
		APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

		if (PlayerPawn)
		{
			TargetActor = PlayerPawn;

			CurrentState = EAIState::Follow;
		}
	}
}

void UAIStateMachineComponent::SetTarget(AActor* Actor)
{
	if (!Actor || !OwnerAI)
		return;

	AH2Character* OwnerChar = Cast<AH2Character>(OwnerAI);
	AH2Character* TargetChar = Cast<AH2Character>(Actor);

	if (!OwnerChar || !TargetChar)
		return;

	/* Ignore same team */
	if (OwnerChar->TeamID == TargetChar->TeamID)
		return;

	/* Friend only attacks enemies */
	if ((OwnerChar->TeamID == ETeamID::Friendly || OwnerChar->TeamID == ETeamID::Player) &&
		TargetChar->TeamID != ETeamID::Enemy)
	{
		return;
	}

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
	if (!OwnerAI || !TargetActor)
		return;

	FVector Direction =
		TargetActor->GetActorLocation() - OwnerAI->GetActorLocation();

	Direction.Z = 0;

	if (Direction.IsNearlyZero())
		return;

	FRotator TargetRotation = Direction.Rotation();

	OwnerAI->SetActorRotation(
		FMath::RInterpTo(
			OwnerAI->GetActorRotation(),
			TargetRotation,
			GetWorld()->GetDeltaSeconds(),
			12.f
		)
	);
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

	/* Always face target */
	if (TargetActor)
	{
		UpdateCombatFocus();
	}

	switch (CurrentState)
	{

	/* --------------------------- */
	case EAIState::Idle:

		break;

	/* --------------------------- */
	case EAIState::Detect:

		if (OwnerAI->AICombat)
		{
			OwnerAI->AICombat->EquipDagger();
		}

		if (OwnerAI->GetCharacterMovement())
		{
			OwnerAI->GetCharacterMovement()->MaxWalkSpeed = OwnerAI->CombatSpeed;
		}

		CurrentState = EAIState::Follow;

		break;

	/* --------------------------- */
	case EAIState::Equip:

		if (OwnerAI->AICombat)
		{
			OwnerAI->AICombat->EquipDagger();
		}

		if (OwnerAI->GetCharacterMovement())
		{
			OwnerAI->GetCharacterMovement()->MaxWalkSpeed = OwnerAI->CombatSpeed;
		}

		CurrentState = EAIState::Follow;

		break;

	/* --------------------------- */
	case EAIState::Follow:

		if (!TargetActor)
			return;

	{
		float Distance = FVector::Dist(
			OwnerAI->GetActorLocation(),
			TargetActor->GetActorLocation());

		/* Sprint if too far */
		if (OwnerAI->bCanSprint && Distance > OwnerAI->SprintDistance)
		{
			CurrentState = EAIState::Unequip;
			break;
		}

		/* Normal follow */
		AIController->MoveToActor(TargetActor, AcceptanceRadius);

		if (Distance <= AttackRange)
		{
			CurrentState = EAIState::Attack;
		}
	}

	break;

	/* --------------------------- */
	case EAIState::Unequip:

		/* Use character equip toggle system */
		if (OwnerAI->bIsEquipped)
		{
			OwnerAI->ToggleEquip();
		}

		if (OwnerAI->GetCharacterMovement())
		{
			OwnerAI->GetCharacterMovement()->MaxWalkSpeed = OwnerAI->SprintSpeed;
		}

		CurrentState = EAIState::Sprint;

		break;

	/* --------------------------- */
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

	/* --------------------------- */
	case EAIState::Attack:

	{
		if (!SlotManager)
			return;

		/* Request combat slot */
		if (!bHasCombatSlot)
		{
			CombatSlotIndex = SlotManager->RequestSlot(TargetActor, OwnerAI);

			if (CombatSlotIndex >= 0)
			{
				bHasCombatSlot = true;
			}
			else
			{
				CurrentState = EAIState::Wait;
				break;
			}
		}

		/* Move to orbit position */
		FVector OrbitLocation = SlotManager->GetOrbitLocation(TargetActor,CombatSlotIndex,OwnerAI->OrbitRadius);

		AIController->MoveToLocation(OrbitLocation);

		/* always face target */
		FVector Direction =
			TargetActor->GetActorLocation() - OwnerAI->GetActorLocation();

		Direction.Z = 0;

		if (!Direction.IsNearlyZero())
		{
			FRotator TargetRot = Direction.Rotation();
			OwnerAI->SetActorRotation(TargetRot);
		}

		float Time = GetWorld()->GetTimeSeconds();

		if (Time - LastAttackTime >= AttackCooldown)
		{
			if (OwnerAI->AICombat)
			{
				OwnerAI->AICombat->Attack();
			}

			LastAttackTime = Time;

			CurrentState = EAIState::Wait;
		}
	}

	break;

	/* --------------------------- */
	case EAIState::Wait:

		if (!TargetActor)
			return;

	{
		float Distance = FVector::Dist(
			OwnerAI->GetActorLocation(),
			TargetActor->GetActorLocation());

		/* Release slot after attack */
		if (bHasCombatSlot && SlotManager)
		{
			SlotManager->ReleaseSlot(TargetActor, OwnerAI);
			bHasCombatSlot = false;
		}

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

	/* --------------------------- */
	default:
		break;
	}
}