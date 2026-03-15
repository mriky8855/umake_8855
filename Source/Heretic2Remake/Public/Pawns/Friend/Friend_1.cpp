#include "Pawns/Friend/Friend_1.h"


AFriend_1::AFriend_1()
{
    AIType = EAIType::Friendly;
    TeamID = ETeamID::Friendly;
}

void AFriend_1::BeginPlay()
{
	Super::BeginPlay();

	AIType = EAIType::Friendly;
}