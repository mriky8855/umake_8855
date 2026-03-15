#include "Characters/AICombatSlots/CombatSlotManager.h"

ACombatSlotManager::ACombatSlotManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACombatSlotManager::BeginPlay()
{
	Super::BeginPlay();
}

int32 ACombatSlotManager::RequestSlot(AActor* Target, AActor* Attacker)
{
	if (!Target || !Attacker)
		return -1;

	TArray<AActor*>& Slots = ActiveSlots.FindOrAdd(Target);

	if (Slots.Num() >= MaxAttackers)
		return -1;

	int32 SlotIndex = Slots.Num();

	if (!Slots.Contains(Attacker))
	{
		Slots.Add(Attacker);
	}

	return SlotIndex;
}

void ACombatSlotManager::ReleaseSlot(AActor* Target, AActor* Attacker)
{
	if (!Target || !Attacker)
		return;

	TArray<AActor*>* Slots = ActiveSlots.Find(Target);

	if (!Slots)
		return;

	Slots->Remove(Attacker);

	if (Slots->Num() == 0)
	{
		ActiveSlots.Remove(Target);
	}
}

FVector ACombatSlotManager::GetOrbitLocation(AActor* Target, int32 SlotIndex, float Radius)
{
	if (!Target)
		return FVector::ZeroVector;

	FVector Center = Target->GetActorLocation();

	float AngleStep = 360.f / MaxAttackers;
	float Angle = AngleStep * SlotIndex;

	/* use target rotation */
	FRotator TargetRot = Target->GetActorRotation();

	FVector Forward = TargetRot.Vector();
	FVector Right = FRotationMatrix(TargetRot).GetUnitAxis(EAxis::Y);

	float Rad = FMath::DegreesToRadians(Angle);

	FVector Offset =
		(Forward * FMath::Cos(Rad) + Right * FMath::Sin(Rad)) * Radius;

	return Center + Offset;
}