#include "JMDrone.h"
#include "JMDroneController.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"


AJMDrone::AJMDrone() : RollSpeed(100.0f), AirControlRatio(0.5f),
	GravityAccel(-9.8f), Velocity(FVector::ZeroVector), bIsOnGround(false)
{
	SpringArmComp->bUsePawnControlRotation = false;		// 드론에 spring arm 고정(컨트롤러 회전과 연동 X)
}

void AJMDrone::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AJMDroneController* DroneController = Cast<AJMDroneController>(GetController()))
		{
			if (DroneController->DroneMoveAction)
			{
				EnhancedInputComponent->BindAction(
					DroneController->DroneMoveAction,
					ETriggerEvent::Triggered,
					this,
					&AJMDrone::Move);
			}
			if (DroneController->DroneLookAction)
			{
				EnhancedInputComponent->BindAction(
					DroneController->DroneLookAction,
					ETriggerEvent::Triggered,
					this,
					&AJMDrone::Look);
			}
			if (DroneController->DroneRollAction)
			{
				EnhancedInputComponent->BindAction(
					DroneController->DroneRollAction,
					ETriggerEvent::Triggered,
					this,
					&AJMDrone::Roll);
			}
		}
	}
}

void AJMDrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bIsOnGround = CheckGrounded();

	if (!bIsOnGround)
	{
		Velocity.Z += GravityAccel * DeltaTime;		// 공중에 있을 때, 중력가속도 적용(점점 커짐, 자유낙하)
	}
	else
	{
		if (Velocity.Z < 0.0f)
		{
			Velocity.Z = 0.0f;	// 낙하중 + 지표면 착지시 낙하속도 0으로 초기화
		}
	}

	if (!Velocity.IsNearlyZero())
	{
		FHitResult Hit;
		AddActorWorldOffset(Velocity * DeltaTime, true, &Hit);	// Sweep 활성화로 Hit에 충돌정보 받아오기
		if (Hit.bBlockingHit && Velocity.Z < 0)
		{
			bIsOnGround = true;		// 낙하중 + 충돌정보 있을 때 물체위에 착지로 판단
			Velocity.Z = 0.0f;		// 낙하속도 0으로 초기화
		}
	}
}

void AJMDrone::Move(const FInputActionValue& Value)
{
	const FVector3d Input = Value.Get<FVector3d>();
	if (Input.IsNearlyZero()) return;

	float Speed = bIsOnGround ? MoveSpeed : MoveSpeed * AirControlRatio;	// 공중이동시 이동속도 감소(0.4배, AirControlRatio)
	AddActorLocalOffset(Input * Speed * GetWorld()->GetDeltaSeconds(), true);	// local 죄표 변경으로 이동
}

void AJMDrone::Look(const FInputActionValue& Value)
{
	FVector2D Input = Value.Get<FVector2D>();
	if (Input.IsNearlyZero()) return;

	FQuat CurrentQuat = GetActorQuat();		// Pitch 축 360도 회전을 위해 Quat 사용

	float DeltaYaw = Input.X * LookSpeed * GetWorld()->GetDeltaSeconds();
	float DeltaPitch = Input.Y * LookSpeed * GetWorld()->GetDeltaSeconds();

	FQuat YawQuat = FQuat(GetActorUpVector(), FMath::DegreesToRadians(DeltaYaw));
	FQuat PitchQuat = FQuat(GetActorRightVector(), FMath::DegreesToRadians(DeltaPitch));

	FQuat NewQuat = YawQuat * PitchQuat * CurrentQuat;
	SetActorRotation(NewQuat);		// 회전각을 드론에 적용

	SpringArmComp->SetWorldRotation(GetActorRotation());	// 드론의 회전각을 카메라에도 동기화
}

void AJMDrone::Roll(const FInputActionValue& Value)
{
	float Input = Value.Get<float>();
	if (FMath::IsNearlyZero(Input)) return;

	FRotator Rot = GetActorRotation();
	Rot.Roll += Input * RollSpeed * GetWorld()->GetDeltaSeconds();
	SetActorRotation(Rot);
}

/* 땅에 닿았는지 RayTrace */
bool AJMDrone::CheckGrounded() const
{
	FVector Start = GetActorLocation();		// 시작점
	FVector End = Start - FVector(0, 0, CapsuleComp->GetScaledCapsuleHalfHeight() + 2.0f);	// 끝점(캡슐 콜라이더 하단보다 2만큼 더 아래 위치)

	FHitResult Hit;		// 트레이스 결과 저장용

	FCollisionQueryParams Params;	// 쿼리 파라미터(추가 설정정보 전달용)
	Params.AddIgnoredActor(this);	// 자기자신 충돌 무시

	return GetWorld()->LineTraceSingleByChannel(
		Hit,				// 충돌 결과 반환
		Start,				// 시작점
		End,				// 끝점
		ECC_Visibility,		// 트레이스 채널(가시성)
		Params				// 쿼리 파라미터(자기자신 무시)
	);
}
