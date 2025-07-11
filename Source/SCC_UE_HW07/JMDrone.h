
#pragma once

#include "CoreMinimal.h"
#include "JMPawn.h"
#include "JMDrone.generated.h"

/**
 * 
 */
UCLASS()
class SCC_UE_HW07_API AJMDrone : public AJMPawn
{
	GENERATED_BODY()
public:
	AJMDrone();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone|Movement")
	float RollSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone|Movement")
	float AirControlRatio;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone|Movement")
	float GravityAccel;

	FVector Velocity;
	bool bIsOnGround;


	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void Tick(float DeltaTime) override;

	void Move(const FInputActionValue& Value) override;
	void Look(const FInputActionValue& Value) override;
	void Roll(const FInputActionValue& Value);
	bool CheckGrounded() const;

private:
	
};
