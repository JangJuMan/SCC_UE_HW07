#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "JMPawn.generated.h"

class UCapsuleComponent;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class SCC_UE_HW07_API AJMPawn : public APawn
{
	GENERATED_BODY()

public:
	AJMPawn();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pawn|Components")
	UCapsuleComponent* CapsuleComp;		// 충돌 컴포넌트(Capsule)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pawn|Components")
	USkeletalMeshComponent* MeshComp;	// Skeletal Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pawn|Components")
	USpringArmComponent* SpringArmComp;	// Spring Arm
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pawn|Components")
	UCameraComponent* CameraComp;		// Camera

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pawn|Camera")
	float CameraLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pawn|Camera")
	float CameraFOV;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pawn|Movement")
	float MoveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pawn|Movement")
	float LookSpeed;


	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Move(const FInputActionValue& Value);
	virtual void Look(const FInputActionValue& Value);


private:



};
