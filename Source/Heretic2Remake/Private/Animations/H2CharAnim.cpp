#include "Animations/H2CharAnim.h"
#include "Characters/H2Character.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"
#include "Kismet/KismetMathLibrary.h"


UH2CharAnim::UH2CharAnim()
{
	Speed = 0.f;
	Direction = 0.f;
	bIsInAir = false;
	bIsEquipped = false;
	bIsAiming = false;

	LeftFootOffset = 0.f;
	RightFootOffset = 0.f;
	PelvisOffset = 0.f;

	CharacterRef = nullptr;
}

void UH2CharAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	APawn* Pawn = TryGetPawnOwner();
	if (Pawn)
	{
		CharacterRef = Cast<AH2Character>(Pawn);
	}
}

void UH2CharAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!CharacterRef)
	{
		APawn* Pawn = TryGetPawnOwner();
		if (Pawn)
			CharacterRef = Cast<AH2Character>(Pawn);
	}

	if (!CharacterRef)
		return;

	// Movement
	FVector Velocity = CharacterRef->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsEquipped = CharacterRef->bIsEquipped;
	bIsAiming = CharacterRef->bIsAiming;

	if (bIsAiming)
	{
		FRotator ControlRot = CharacterRef->GetControlRotation();
		ControlRot.Pitch = 0.f;
		ControlRot.Roll = 0.f;

		Direction = UKismetAnimationLibrary::CalculateDirection(
			Velocity,
			ControlRot
		);
	}
	else
	{
		Direction = 0.f;
	}

	FRotator ControlRot = CharacterRef->GetControlRotation();
	FRotator ActorRot = CharacterRef->GetActorRotation();

	FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(ControlRot, ActorRot);

	AimPitch = Delta.Pitch;
	AimYaw = Delta.Yaw;

	// IK values come from character
	LeftFootOffset  = CharacterRef->GetLeftFootOffset();
	RightFootOffset = CharacterRef->GetRightFootOffset();
	PelvisOffset    = CharacterRef->GetPelvisOffset();

	LeftFootRotation  = CharacterRef->GetLeftFootRotation();
	RightFootRotation = CharacterRef->GetRightFootRotation();
}