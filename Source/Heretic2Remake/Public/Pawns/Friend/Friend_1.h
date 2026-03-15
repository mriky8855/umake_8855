#pragma once

#include "CoreMinimal.h"
#include "Characters/AICharacter/H2AICharacter.h"
#include "Friend_1.generated.h"

UCLASS()
class HERETIC2REMAKE_API AFriend_1 : public AH2AICharacter
{
	GENERATED_BODY()

public:

	AFriend_1();

protected:

	virtual void BeginPlay() override;
};