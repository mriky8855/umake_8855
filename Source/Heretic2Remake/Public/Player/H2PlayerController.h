#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "H2PlayerController.generated.h"

UCLASS()
class HERETIC2REMAKE_API AH2PlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
};