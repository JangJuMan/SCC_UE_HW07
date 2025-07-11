#include "JMDroneController.h"
#include "EnhancedInputSubsystems.h"

AJMDroneController::AJMDroneController() : 
	DroneInputMappingContext(nullptr),
	DroneMoveAction(nullptr),
	DroneLookAction(nullptr),
	DroneRollAction(nullptr)
{
}

void AJMDroneController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem
			= LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (DroneInputMappingContext)
			{
				Subsystem->AddMappingContext(DroneInputMappingContext, 0);	// 우선순위 0번
			}
		}
	}
}
