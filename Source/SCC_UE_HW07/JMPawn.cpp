#include "JMPawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "JMPlayerController.h"

AJMPawn::AJMPawn() : CameraLength(500.0f), MoveSpeed(400.0f), LookSpeed(100.0f), CameraFOV(120.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);			// 정석, 안정적인 루트 세팅
	CapsuleComp->SetSimulatePhysics(false);	// Physics(충돌) 비활성화

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(CapsuleComp);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(CapsuleComp);
	SpringArmComp->TargetArmLength = CameraLength;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;
	CameraComp->SetFieldOfView(CameraFOV);
}

void AJMPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AJMPlayerController* PlayerController = Cast<AJMPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&AJMPawn::Move
				);
			}
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&AJMPawn::Look
				);
			}
		}
	}
}

/** Controller 회전을 기준으로 Move. 
*   ActorLocalOffset으로 조정하다보면 회전각이 한 프레임 늦게 반영되는 것인지
*   이동 각도가 틀어지는 문제가 있음 */
void AJMPawn::Move(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();
	if (Input.IsNearlyZero()) return;
	if (!Controller)	return;

	FRotator ControlRot = Controller->GetControlRotation();
	FRotator YawRot(0, ControlRot.Yaw, 0);		// Yaw 만 사용한 평면이동

	FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
	FVector MoveDir = Forward * Input.X + Right * Input.Y;
	AddActorWorldOffset(MoveDir * MoveSpeed * GetWorld()->GetDeltaSeconds(), true);	// true : Sweep 활성화
}

/* Controller 각을 변경하고, pawn을 Controller 각에 동기화하는 방식 */
void AJMPawn::Look(const FInputActionValue& Value)
{
	FVector2D Input = Value.Get<FVector2D>();
	if (Input.IsNearlyZero()) return;
	if (!Controller) return;

	float Yaw = Input.X * LookSpeed * GetWorld()->GetDeltaSeconds();
	float Pitch = Input.Y * LookSpeed * GetWorld()->GetDeltaSeconds();

	FRotator ControlRot = Controller->GetControlRotation();
	ControlRot.Yaw += Yaw;
	ControlRot.Pitch = FMath::Clamp(ControlRot.Pitch + Pitch, -80.f, 80.f);
	Controller->SetControlRotation(ControlRot);		// Controller Rotation 을 변경

	FRotator PawnRot = GetActorRotation();
	PawnRot.Yaw = ControlRot.Yaw;
	SetActorRotation(PawnRot);	// Controller Rotation 에 Pawn Rotation을 동기화
}

