#pragma once

#include "CoreMinimal.h"
#include "Characters/AICharacter/H2AICharacter.h"
#include "Enemy_1.generated.h"

UCLASS()
class HERETIC2REMAKE_API AEnemy_1 : public AH2AICharacter
{
	GENERATED_BODY()

public:

	AEnemy_1();

protected:

	virtual void BeginPlay() override;
};