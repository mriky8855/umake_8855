#include "H2Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
#include "Components/CombatComponent.h"
#include "Blueprint/UserWidget.h"
#include "Weapons/Weapon.h"
#include "Animation/AnimInstance.h"
#include "Engine/World.h"

AH2Character::AH2Character()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->MaxWalkSpeed = 450.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 350.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

	bIsEquipped = false;
	bIsAiming = false;
	bIsEquipping = false;

	CurrentWeapon = nullptr;
	CurrentWeaponIndex = 0;

	DefaultFOV = 90.f;
	ADSFOV = 65.f;
	FOVInterpSpeed = 12.f;
}

void AH2Character::BeginPlay()
{
	Super::BeginPlay();
	DefaultFOV = FollowCamera->FieldOfView;

	DefaultSocketOffset = CameraBoom->SocketOffset;
	TargetSocketOffset = DefaultSocketOffset;

	if (CrosshairWidgetClass)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			CrosshairWidget = CreateWidget<UUserWidget>(PC, CrosshairWidgetClass);
		}
		if (CrosshairWidget)
		{
			CrosshairWidget->AddToViewport();
			if (APlayerController* PC = Cast<APlayerController>(GetController()))
			{
				int32 SizeX, SizeY;
				PC->GetViewportSize(SizeX, SizeY);

				FVector2D Center(SizeX, SizeY);
				FVector2D FinalPos = Center + CrosshairScreenOffset;

				CrosshairWidget->SetPositionInViewport(FinalPos, false);
			}
			CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Controller: %s"), *GetNameSafe(GetController()));
	UE_LOG(LogTemp, Warning, TEXT("CrosshairWidgetClass: %s"), *GetNameSafe(CrosshairWidgetClass));
	UE_LOG(LogTemp, Warning, TEXT("CrosshairWidget Created: %s"), CrosshairWidget ? TEXT("YES") : TEXT("NO"));
}

void AH2Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleMovementMode();
	HandleCameraEffects(DeltaTime);

}


void AH2Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}

	if (UEnhancedInputComponent* EnhancedInput =
		Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AH2Character::Move);
		EnhancedInput->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AH2Character::Look);
		EnhancedInput->BindAction(IA_PrimaryAttack, ETriggerEvent::Started, this, &AH2Character::PrimaryAttackInput);
		EnhancedInput->BindAction(IA_Equip, ETriggerEvent::Started, this, &AH2Character::EquipInput);
		EnhancedInput->BindAction(IA_Aim, ETriggerEvent::Started, this, &AH2Character::AimInput);
		EnhancedInput->BindAction(IA_Aim, ETriggerEvent::Completed, this, &AH2Character::StopAim);
	}
}

void AH2Character::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Forward, MovementVector.Y);
		AddMovementInput(Right, MovementVector.X);
	}
}

void AH2Character::Look(const FInputActionValue& Value)
{
	FVector2D LookAxis = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxis.X);
	AddControllerPitchInput(LookAxis.Y);

	if (bIsAiming)
	{
		FRotator ControlRot = GetControlRotation();
		FRotator NewRot(0.f, ControlRot.Yaw, 0.f);
		SetActorRotation(NewRot);
	}
}

void AH2Character::PrimaryAttackInput(const FInputActionValue& Value)
{
	PrimaryAttack();
}

void AH2Character::EquipInput(const FInputActionValue& Value)
{
	ToggleEquip();
}

void AH2Character::AimInput(const FInputActionValue& Value)
{
	StartAim();
}

void AH2Character::HandleMovementMode()
{
	float BaseSpeed = 450.f;

	if (bIsAiming)
	{
		// ADS MODE
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;

		BaseSpeed = 250.f;
	}
	else if (bIsEquipped)
	{
		// Equipped but not ADS
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;

		BaseSpeed = 300.f;
	}
	else
	{
		// Exploration
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;

		BaseSpeed = 450.f;
	}

	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
}

void AH2Character::HandleCameraEffects(float DeltaTime)
{
	float TargetFOV = bIsAiming ? ADSFOV : DefaultFOV;

	float NewFOV = FMath::FInterpTo(
		FollowCamera->FieldOfView,
		TargetFOV,
		DeltaTime,
		FOVInterpSpeed
	);

	FollowCamera->SetFieldOfView(NewFOV);

	// Shoulder shift
	if (bIsAiming)
	{
		TargetSocketOffset = FVector(0.f, 50.f, 10.f); // Right shoulder
	}
	else
	{
		TargetSocketOffset = DefaultSocketOffset;
	}

	CameraBoom->SocketOffset = FMath::VInterpTo(
		CameraBoom->SocketOffset,
		TargetSocketOffset,
		DeltaTime,
		8.f
	);
}

void AH2Character::MoveForward(float Value)
{
	if (Controller && Value != 0.f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AH2Character::MoveRight(float Value)
{
	if (Controller && Value != 0.f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AH2Character::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AH2Character::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AH2Character::StartAim()
{
	if (bIsEquipped && CurrentWeapon && CurrentWeapon->IsShooting())
	{
		bIsAiming = true;

		if (CrosshairWidget)
			CrosshairWidget->SetVisibility(ESlateVisibility::Visible);
	}

	UE_LOG(LogTemp, Warning, TEXT("StartAim Called"));
	UE_LOG(LogTemp, Warning, TEXT("bIsEquipped: %d"), bIsEquipped);
	UE_LOG(LogTemp, Warning, TEXT("CurrentWeapon: %s"), *GetNameSafe(CurrentWeapon));
}

void AH2Character::StopAim()
{
	bIsAiming = false;

	if (CrosshairWidget)
		CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
}

void AH2Character::ToggleEquip()
{
	if (bIsEquipping) return;

	UAnimInstance* Anim = GetMesh()->GetAnimInstance();
	if (!Anim) return;

	bIsEquipping = true;

	if (!bIsEquipped)
	{
		SpawnWeapon(CurrentWeaponIndex);

		if (EquipMontage)
		{
			Anim->Montage_Play(EquipMontage);
		}

		bIsEquipped = true;
	}
	else
	{
		if (UnequipMontage)
		{
			Anim->Montage_Play(UnequipMontage);
		}

		DespawnCurrentWeapon();
		bIsEquipped = false;

		CurrentWeaponIndex++;
		if (CurrentWeaponIndex >= WeaponClasses.Num())
			CurrentWeaponIndex = 0;
	}

	bIsEquipping = false;
}

void AH2Character::SpawnWeapon(int32 Index)
{
	if (!WeaponClasses.IsValidIndex(Index)) return;

	FActorSpawnParameters Params;
	Params.Owner = this;

	AWeapon* NewWeapon =
		GetWorld()->SpawnActor<AWeapon>(WeaponClasses[Index], Params);

	if (!NewWeapon) return;

	NewWeapon->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		TEXT("WeaponSocket")
	);

	CurrentWeapon = NewWeapon;
	CombatComponent->SetWeapon(NewWeapon);

	if (NewWeapon->IsMelee())
	{
		bIsAiming = false;
	}
}

void AH2Character::DespawnCurrentWeapon()
{
	if (!CurrentWeapon) return;

	CurrentWeapon->Destroy();
	CurrentWeapon = nullptr;
}

void AH2Character::PrimaryAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("PrimaryAttack Called"));

	if (CombatComponent)
	{
		CombatComponent->HandlePrimaryAttack(this);
	}
}