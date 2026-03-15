#include "Characters/AICombatComponent/AICombatComponent.h"
#include "Characters/H2Character.h"
#include "Components/CombatComponent.h"

UAICombatComponent::UAICombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAICombatComponent::EquipDagger()
{
	AH2Character* OwnerChar = Cast<AH2Character>(GetOwner());

	if(!OwnerChar)
		return;

	if(!OwnerChar->bIsEquipped)
	{
		OwnerChar->ToggleEquip();
	}
}

void UAICombatComponent::Attack()
{
	AH2Character* OwnerChar = Cast<AH2Character>(GetOwner());

	if(!OwnerChar)
		return;

	OwnerChar->PrimaryAttack();
}

void UAICombatComponent::Unequip()
{
	AH2Character* OwnerChar = Cast<AH2Character>(GetOwner());

	if(!OwnerChar)
		return;

	if(OwnerChar->bIsEquipped)
	{
		OwnerChar->ToggleEquip();
	}
}