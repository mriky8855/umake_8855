#include "Characters/AIStateMachine/AIStateMachineComponent.h"
#include "Characters/AICharacter/H2AICharacter.h"
#include "Characters/AICombatComponent/AICombatComponent.h"

#include "AIController.h"
#include "Kismet/GameplayStatics.h"

UAIStateMachineComponent::UAIStateMachineComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    AttackRange = 150.f;
}

void UAIStateMachineComponent::BeginPlay()
{
    Super::BeginPlay();

    OwnerAI = Cast<AH2AICharacter>(GetOwner());

    CurrentState = EAIState::Idle;

    TargetActor = nullptr;
}

void UAIStateMachineComponent::SetTarget(AActor* Actor)
{
    TargetActor = Actor;

    CurrentState = EAIState::Detect;
}

void UAIStateMachineComponent::ClearTarget(AActor* Actor)
{
    if(TargetActor == Actor)
    {
        TargetActor = nullptr;

        CurrentState = EAIState::Idle;
    }
}

void UAIStateMachineComponent::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if(!OwnerAI)
        return;

    AAIController* AIController = Cast<AAIController>(OwnerAI->GetController());

    if(!AIController)
        return;

    switch(CurrentState)
    {

    case EAIState::Idle:

        break;

    case EAIState::Detect:

        OwnerAI->AICombat->EquipDagger();

        CurrentState = EAIState::Follow;

        break;

    case EAIState::Follow:

        if(!TargetActor)
            return;

        AIController->MoveToActor(TargetActor, AttackRange);

        if(FVector::Dist(
            OwnerAI->GetActorLocation(),
            TargetActor->GetActorLocation()) < AttackRange)
        {
            CurrentState = EAIState::Attack;
        }

        break;

    case EAIState::Attack:

        OwnerAI->AICombat->Attack();

        CurrentState = EAIState::Wait;

        break;

    case EAIState::Wait:

        CurrentState = EAIState::Follow;

        break;

    default:
        break;

    }
}