#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponCategory : uint8
{
	Melee,
	Shooting
};

UCLASS()
class HERETIC2REMAKE_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	EWeaponCategory WeaponCategory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, Category="Attack")
	TArray<UAnimMontage*> AttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	UMaterialInterface* ImpactDecalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	float TraceDistance;

	bool IsMelee() const { return WeaponCategory == EWeaponCategory::Melee; }
	bool IsShooting() const { return WeaponCategory == EWeaponCategory::Shooting; }
};