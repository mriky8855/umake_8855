#include "Pawns/Enemy/Enemy_1.h"


AEnemy_1::AEnemy_1()
{
    AIType = EAIType::Enemy;
    TeamID = ETeamID::Enemy;
}

void AEnemy_1::BeginPlay()
{
	Super::BeginPlay();

	AIType = EAIType::Enemy;
}