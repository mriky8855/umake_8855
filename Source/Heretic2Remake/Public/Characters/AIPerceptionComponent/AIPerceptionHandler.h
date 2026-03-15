#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "AIPerceptionHandler.generated.h"

class UAIPerceptionComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HERETIC2REMAKE_API UAIPerceptionHandler : public UActorComponent
{
	GENERATED_BODY()

public:

	UAIPerceptionHandler();

protected:

	virtual void BeginPlay() override;

private:

	UAIPerceptionComponent* Perception;

	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);
};