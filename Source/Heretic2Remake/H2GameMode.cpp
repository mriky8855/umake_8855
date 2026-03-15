#include "H2GameMode.h"
#include "Player/H2PlayerController.h"
#include "Characters/H2Character.h"

AH2GameMode::AH2GameMode()
{
	PlayerControllerClass = AH2PlayerController::StaticClass();
	DefaultPawnClass = AH2Character::StaticClass();
}