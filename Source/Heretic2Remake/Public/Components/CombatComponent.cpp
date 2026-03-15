#include "Components/CombatComponent.h"
#include "Weapons/Weapon.h"
#include "Characters/H2Character.h"
#include "Camera/CameraComponent.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

void UCombatComponent::SetWeapon(AWeapon* NewWeapon)
{
	CurrentWeapon = NewWeapon;
}

void UCombatComponent::HandlePrimaryAttack(AH2Character* Character)
{
	if (!Character || !CurrentWeapon) return;

	if (CurrentWeapon->IsMelee())
	{
		PerformMeleeAttack(Character);
	}
	else if (CurrentWeapon->IsShooting())
	{
		PerformShootingAttack(Character);
	}
}

void UCombatComponent::PerformMeleeAttack(AH2Character* Character)
{
	if (!Character || !CurrentWeapon) return;
	if (CurrentWeapon->AttackMontages.Num() == 0) return;

	// Play random melee montage
	int32 Index = FMath::RandRange(0, CurrentWeapon->AttackMontages.Num() - 1);
	UAnimMontage* SelectedMontage = CurrentWeapon->AttackMontages[Index];

	UAnimInstance* Anim = Character->GetMesh()->GetAnimInstance();
	if (Anim && SelectedMontage)
	{
		Anim->Montage_Play(SelectedMontage);
	}

	// Melee trace from chest height
	FVector Start = Character->GetActorLocation() + FVector(0.f, 0.f, 60.f);
	FVector Forward = Character->GetActorForwardVector();
	FVector End = Start + Forward * 150.f;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Pawn,
		Params
	);

	DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 2.f, 0, 3.f);

	if (bHit && Hit.GetActor())
	{
		DrawDebugSphere(GetWorld(), Hit.Location, 15.f, 16, FColor::Red, false, 2.f);
		UE_LOG(LogTemp, Warning, TEXT("Melee Hit: %s"), *Hit.GetActor()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Melee Miss"));
	}
}

void UCombatComponent::PerformShootingAttack(AH2Character* Character)
{
	if (!Character || !CurrentWeapon) return;
	if (!Character->bIsAiming) return;

	APlayerController* PC = Cast<APlayerController>(Character->GetController());
	if (!PC) return;

	UCameraComponent* Camera = Character->GetFollowCamera();
	if (!Camera) return;

	// Get viewport center + crosshair offset
	int32 SizeX, SizeY;
	PC->GetViewportSize(SizeX, SizeY);

	float ScreenX = SizeX * 0.5f;
	float ScreenY = SizeY * 0.5f;

	// Deproject screen position to world
	FVector WorldLocation;
	FVector WorldDirection;

	if (!PC->DeprojectScreenPositionToWorld(
		ScreenX,
		ScreenY,
		WorldLocation,
		WorldDirection))
	{
		return;
	}

	// -----------------------------
	// STEP 1: Camera Trace
	// -----------------------------
	FVector CameraStart = WorldLocation;
	FVector CameraEnd = CameraStart + WorldDirection * CurrentWeapon->TraceDistance;

	FHitResult CameraHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);

	bool bCameraHit = GetWorld()->LineTraceSingleByChannel(
		CameraHit,
		CameraStart,
		CameraEnd,
		ECC_Visibility,
		Params
	);

	FVector TargetPoint = bCameraHit ? CameraHit.Location : CameraEnd;

	// -----------------------------
	// STEP 2: Muzzle Trace
	// -----------------------------
	FVector MuzzleStart = CurrentWeapon->GetActorLocation();
	FVector ShootDirection = (TargetPoint - MuzzleStart).GetSafeNormal();
	FVector FinalEnd = MuzzleStart + ShootDirection * CurrentWeapon->TraceDistance;

	FHitResult FinalHit;

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		FinalHit,
		MuzzleStart,
		FinalEnd,
		ECC_Visibility,
		Params
	);

	DrawDebugLine(GetWorld(), MuzzleStart, FinalEnd, FColor::Green, false, 2.f, 0, 2.f);

	// -----------------------------
	// Impact + Decal
	// -----------------------------
	if (bHit)
	{
		DrawDebugSphere(GetWorld(), FinalHit.Location, 10.f, 12, FColor::Red, false, 2.f);

		if (CurrentWeapon->ImpactDecalMaterial)
		{
			FRotator DecalRotation = UKismetMathLibrary::MakeRotFromZ(FinalHit.Normal);

			UGameplayStatics::SpawnDecalAtLocation(
				GetWorld(),
				CurrentWeapon->ImpactDecalMaterial,
				FVector(10.f),
				FinalHit.Location,
				DecalRotation,
				5.f
			);
		}
	}
}