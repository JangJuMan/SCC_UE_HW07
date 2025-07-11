#include "JMGameMode.h"
#include "JMPawn.h"
#include "JMPlayerController.h"

AJMGameMode::AJMGameMode()
{
	DefaultPawnClass = AJMPawn::StaticClass();						// Default Pawn Class 지정 
	PlayerControllerClass = AJMPlayerController::StaticClass();		// Default Player Controller 지정
}
