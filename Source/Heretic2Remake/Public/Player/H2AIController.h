#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "H2AIController.generated.h"

UCLASS()
class HERETIC2REMAKE_API AH2AIController : public AAIController
{
	GENERATED_BODY()

public:
	AH2AIController();

protected:
	virtual void BeginPlay() override;
};