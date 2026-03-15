#include "Components/InventoryComponent.h"
#include "Weapons/Weapon.h"

void UInventoryComponent::AddWeapon(AWeapon* Weapon)
{
	if (!Weapon) return;
	Weapons.Add(Weapon);
}

void UInventoryComponent::EquipWeapon(AWeapon* Weapon)
{
	if (!Weapon) return;
	EquippedWeapon = Weapon;
}