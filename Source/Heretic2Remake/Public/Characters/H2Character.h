#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Components/CombatComponent.h"
#include "InputAction.h"

#include "H2Character.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UCombatComponent;
class UInventoryComponent;
class UAnimMontage;
class AWeapon;
class UUserWidget;

UENUM(BlueprintType)
enum class ETeamID : uint8
{
	Player = 0,
	Friendly = 0,
	Enemy = 1,
	Neutral = 2
};

UCLASS()
class HERETIC2REMAKE_API AH2Character : public ACharacter
{
	GENERATED_BODY()

public:
	AH2Character();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void HandleMovementMode();
	void HandleCameraEffects(float DeltaTime);

public:

	FORCEINLINE FVector2D GetCrosshairOffset() const { return CrosshairScreenOffset; }

	// ===============================
	// BASIC MOVEMENT
	// ===============================

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);

	// ===============================
	// COMBAT INPUT
	// ===============================

	void ToggleEquip();
	void StartAim();
	void StopAim();
	void PrimaryAttack();

	// ===============================
	// WEAPON SPAWN
	// ===============================

	void SpawnWeapon(int32 Index);
	void DespawnCurrentWeapon();

	// ===============================
	// ENHANCED INPUT
	// ===============================

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void PrimaryAttackInput(const FInputActionValue& Value);
	void EquipInput(const FInputActionValue& Value);
	void AimInput(const FInputActionValue& Value);

	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE UCombatComponent* GetCombatComponent() const
	{
		return CombatComponent;
	}

public:

	// ===============================
	// STATE FLAGS
	// ===============================

	UPROPERTY(BlueprintReadOnly, Category="Combat")
	bool bIsEquipped;

	UPROPERTY(BlueprintReadOnly, Category="Combat")
	bool bIsAiming;

	// ===============================
	// WEAPON SYSTEM
	// ===============================

	UPROPERTY(EditAnywhere, Category="Weapons")
	TArray<TSubclassOf<AWeapon>> WeaponClasses;

	UPROPERTY()
	AWeapon* CurrentWeapon;

	int32 CurrentWeaponIndex;

	// ===============================
	// UI SYSTEM
	// ===============================

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UUserWidget> CrosshairWidgetClass;

	UPROPERTY()
	UUserWidget* CrosshairWidget;

	// ===============================
	// CAMERA SETTINGS
	// ===============================

	UPROPERTY(EditAnywhere, Category="Camera|ADS")
	float ADSFOV;

	UPROPERTY(EditAnywhere, Category="Camera|ADS")
	float FOVInterpSpeed;

	UPROPERTY(EditAnywhere, Category="Camera|ADS")
	FVector ADSRightShoulderOffset;

	UPROPERTY(EditAnywhere, Category="Camera|ADS")
	float ADSCameraInterpSpeed;

	// ===============================
	// TEAM SETTINGS
	// ===============================
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Team")
	ETeamID TeamID = ETeamID::Neutral;

private:

	// ===============================
	// COMPONENTS
	// ===============================

	UPROPERTY()
	USpringArmComponent* CameraBoom;

	UPROPERTY()
	UCameraComponent* FollowCamera;

	UPROPERTY()
	UCombatComponent* CombatComponent;

	UPROPERTY()
	UInventoryComponent* InventoryComponent;


	// ===============================
	// MONTAGES
	// ===============================

	UPROPERTY(EditAnywhere, Category="Combat")
	UAnimMontage* EquipMontage;

	UPROPERTY(EditAnywhere, Category="Combat")
	UAnimMontage* UnequipMontage;

	// ===============================
	// INPUT MAPPING
	// ===============================

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputMappingContext* PlayerMappingContext;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_Move;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_Look;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_PrimaryAttack;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_Equip;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_Aim;

	// ===============================
	// INTERNAL STATE
	// ===============================

	bool bIsEquipping;

	float DefaultFOV;

	float CameraSwayTime;

	// ===============================
	// FOOT IK SYSTEM (UNCHANGED)
	// ===============================

	float LeftFootOffset;
	float RightFootOffset;
	float PelvisOffset;

	FRotator LeftFootRotation;
	FRotator RightFootRotation;

	// --- ADS Camera Offset ---
	FVector TargetSocketOffset;
	FVector DefaultSocketOffset;

	// --- Aim Rotation ---
	FRotator TargetControlRotation;

	// --- Crosshair Offset ---
	UPROPERTY(EditAnywhere, Category="UI")
	FVector2D CrosshairScreenOffset = FVector2D(0.f, 0.f);

	void UpdateFootIK(float DeltaTime);
	void TraceFoot(FName SocketName, float& OutOffset, FRotator& OutRotation);

public:

	float GetLeftFootOffset() const { return LeftFootOffset; }
	float GetRightFootOffset() const { return RightFootOffset; }
	float GetPelvisOffset() const { return PelvisOffset; }

	FRotator GetLeftFootRotation() const { return LeftFootRotation; }
	FRotator GetRightFootRotation() const { return RightFootRotation; }
};