#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "H2CharAnim.generated.h"

class AH2Character;

UCLASS()
class HERETIC2REMAKE_API UH2CharAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	UH2CharAnim();

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:

	// Movement
	UPROPERTY(BlueprintReadOnly)
	float Speed;

	UPROPERTY(BlueprintReadOnly)
	float Direction;

	UPROPERTY(BlueprintReadOnly)
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly)
	bool bIsEquipped;

	UPROPERTY(BlueprintReadOnly)
	bool bIsAiming;

	// IK
	UPROPERTY(BlueprintReadOnly)
	float LeftFootOffset;

	UPROPERTY(BlueprintReadOnly)
	float RightFootOffset;

	UPROPERTY(BlueprintReadOnly)
	float PelvisOffset;

	UPROPERTY(BlueprintReadOnly)
	FRotator LeftFootRotation;

	UPROPERTY(BlueprintReadOnly)
	FRotator RightFootRotation;

	UPROPERTY(BlueprintReadOnly)
	float AimPitch;

	UPROPERTY(BlueprintReadOnly)
	float AimYaw;

private:
	AH2Character* CharacterRef;

	void UpdateFootIK(float DeltaTime);
	void TraceFoot(FName SocketName, float& OutOffset, FRotator& OutRotation);

public:
	float GetLeftFootOffset() const { return LeftFootOffset; }
	float GetRightFootOffset() const { return RightFootOffset; }
	float GetPelvisOffset() const { return PelvisOffset; }

	FRotator GetLeftFootRotation() const { return LeftFootRotation; }
	FRotator GetRightFootRotation() const { return RightFootRotation; }
};