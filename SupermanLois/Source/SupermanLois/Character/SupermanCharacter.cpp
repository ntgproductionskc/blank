#include "SupermanCharacter.h"
#include "SupermanMovementComponent.h"
#include "SupermanPowerComponent.h"
#include "../Powers/HeatVisionAbility.h"
#include "../Powers/FreezeBreathAbility.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"

ASupermanCharacter::ASupermanCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USupermanMovementComponent>(
		ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch   = false;
	bUseControllerRotationYaw     = false;
	bUseControllerRotationRoll    = false;

	// Camera rig
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength           = 350.f;
	CameraBoom->bUsePawnControlRotation   = true;
	CameraBoom->bEnableCameraLag          = true;
	CameraBoom->CameraLagSpeed            = 8.f;
	CameraBoom->CameraLagMaxDistance      = 200.f;
	CameraBoom->bEnableCameraRotationLag  = true;
	CameraBoom->CameraRotationLagSpeed    = 6.f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->FieldOfView = 90.f;

	// Power component
	PowerComponent = CreateDefaultSubobject<USupermanPowerComponent>(TEXT("PowerComponent"));

	// Ability system
	AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));

	// VFX — attached to eye sockets (set socket names in BP)
	HeatVisionLeftEye  = CreateDefaultSubobject<UNiagaraComponent>(TEXT("HeatVisionL"));
	HeatVisionRightEye = CreateDefaultSubobject<UNiagaraComponent>(TEXT("HeatVisionR"));
	HeatVisionLeftEye->SetupAttachment(GetMesh(), TEXT("EyeSocket_L"));
	HeatVisionRightEye->SetupAttachment(GetMesh(), TEXT("EyeSocket_R"));
	HeatVisionLeftEye->SetAutoActivate(false);
	HeatVisionRightEye->SetAutoActivate(false);

	FreezeBreathNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FreezeBreath"));
	FreezeBreathNiagara->SetupAttachment(GetMesh(), TEXT("MouthSocket"));
	FreezeBreathNiagara->SetAutoActivate(false);

	FlightTrailNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FlightTrail"));
	FlightTrailNiagara->SetupAttachment(RootComponent);
	FlightTrailNiagara->SetAutoActivate(false);

	// Cape
	CapeMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Cape"));
	CapeMesh->SetupAttachment(GetMesh(), TEXT("CapeSocket"));

	// Movement defaults
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate              = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed              = 600.f;
	GetCharacterMovement()->JumpZVelocity             = 1200.f;
	GetCharacterMovement()->GravityScale              = 1.75f;
}

void ASupermanCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystem)
		AbilitySystem->InitAbilityActorInfo(this, this);
}

void ASupermanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateFlightPhysics(DeltaTime);
	UpdateHeatVision(DeltaTime);
	UpdateFreezeBreath(DeltaTime);
	UpdateSolarEnergyRegen(DeltaTime);
	UpdateCapePhysics(DeltaTime);

	if (SonicBoomCooldown > 0.f) SonicBoomCooldown -= DeltaTime;
	PreviousSpeed = GetVelocity().Size();
}

UAbilitySystemComponent* ASupermanCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

// ── State ──────────────────────────────────────────────────────────────────────

void ASupermanCharacter::SetSupermanState(ESupermanState NewState)
{
	if (SupermanState == NewState) return;
	SupermanState = NewState;
	OnStateChanged.Broadcast(NewState);
}

// ── Movement ───────────────────────────────────────────────────────────────────

void ASupermanCharacter::MoveInput(FVector2D AxisValue)
{
	if (SupermanState == ESupermanState::Cutscene || SupermanState == ESupermanState::Stunned) return;

	const FRotator ControlRot = GetControlRotation();

	if (bIsFlying || bIsHovering)
	{
		// 6-DOF flight: forward = camera forward, right = camera right
		const FVector Forward = UKismetMathLibrary::GetForwardVector(FRotator(ControlRot.Pitch, ControlRot.Yaw, 0.f));
		const FVector Right   = UKismetMathLibrary::GetRightVector(FRotator(0.f, ControlRot.Yaw, 0.f));

		TargetFlightVelocity  = Forward * AxisValue.Y + Right * AxisValue.X;
		TargetFlightVelocity  = TargetFlightVelocity.GetSafeNormal();
	}
	else
	{
		// Ground movement
		const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);
		AddMovementInput(FRotationMatrix(YawRot).GetUnitAxis(EAxis::X), AxisValue.Y);
		AddMovementInput(FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y), AxisValue.X);
	}
}

void ASupermanCharacter::SuperJump()
{
	if (IsWeakenedByKryptonite()) return;

	// Override jump with massive leap
	LaunchCharacter(FVector(0.f, 0.f, 2800.f), false, true);
	SetSupermanState(ESupermanState::Jumping);

	// Transition to flying at apex
	FTimerHandle JumpFlightTimer;
	GetWorldTimerManager().SetTimer(JumpFlightTimer, [this]()
	{
		if (GetVelocity().Z < 0.f) StartFlying();
	}, 0.4f, false);
}

void ASupermanCharacter::StartFlying()
{
	if (IsWeakenedByKryptonite()) return;

	bIsFlying  = true;
	bIsHovering = false;

	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->MaxFlySpeed = Stats.FlightSpeed;
	GetCharacterMovement()->BrakingDecelerationFlying = 1500.f;

	FlightTrailNiagara->Activate(true);
	SetSupermanState(ESupermanState::Flying);
}

void ASupermanCharacter::HoverInPlace()
{
	bIsHovering = true;
	bIsFlying   = false;

	GetCharacterMovement()->Velocity     = FVector::ZeroVector;
	GetCharacterMovement()->MaxFlySpeed  = 50.f;
	FlightTrailNiagara->Deactivate();
	SetSupermanState(ESupermanState::Hovering);
}

void ASupermanCharacter::Land()
{
	bIsFlying   = false;
	bIsHovering = false;
	bIsBoostFlying = false;
	CurrentBoostBlend = 0.f;

	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	FlightTrailNiagara->Deactivate();

	ApplyLandingImpact();
	SetSupermanState(ESupermanState::Grounded);
}

void ASupermanCharacter::ActivateFlightBoost(bool bActive)
{
	if (!bIsFlying || IsWeakenedByKryptonite()) return;

	bIsBoostFlying = bActive;

	if (bActive)
	{
		GetCharacterMovement()->MaxFlySpeed = Stats.FlightSpeed * Stats.BoostMultiplier;
		SetSupermanState(ESupermanState::FlyingBoost);

		if (SonicBoomCooldown <= 0.f) TriggerSonicBoom();
	}
	else
	{
		GetCharacterMovement()->MaxFlySpeed = Stats.FlightSpeed;
		SetSupermanState(ESupermanState::Flying);
	}
}

void ASupermanCharacter::UpdateFlightPhysics(float DeltaTime)
{
	if (!bIsFlying) return;

	const float PowerCost = bIsBoostFlying ? 2.f * DeltaTime : 0.5f * DeltaTime;
	Stats.SolarEnergy = FMath::Max(0.f, Stats.SolarEnergy - PowerCost);

	// Smoothly orient body to flight direction
	if (!GetVelocity().IsNearlyZero(10.f))
	{
		FRotator DesiredRot = GetVelocity().ToOrientationRotator();
		if (bIsBoostFlying)
			DesiredRot.Pitch = FMath::ClampAngle(DesiredRot.Pitch, -30.f, 15.f);

		SetActorRotation(FMath::RInterpTo(GetActorRotation(), DesiredRot, DeltaTime, 5.f));
	}

	// Move toward target velocity
	if (!TargetFlightVelocity.IsNearlyZero())
	{
		const float Speed = bIsBoostFlying
			? Stats.FlightSpeed * Stats.BoostMultiplier
			: Stats.FlightSpeed;

		AddMovementInput(TargetFlightVelocity, 1.f);
		TargetFlightVelocity = FVector::ZeroVector;
	}

	// Boost blend for camera FOV (done in BP camera update graph)
	CurrentBoostBlend = FMath::FInterpTo(CurrentBoostBlend, bIsBoostFlying ? 1.f : 0.f, DeltaTime, 4.f);

	// Auto-land when out of energy
	if (Stats.SolarEnergy <= 0.f) Land();
}

void ASupermanCharacter::TriggerSonicBoom()
{
	SonicBoomCooldown = 3.f;
	OnSonicBoomTriggered.Broadcast();

	// Radial damage shockwave — shatters glass, ragdolls nearby enemies
	UGameplayStatics::ApplyRadialDamage(
		GetWorld(), 500.f, GetActorLocation(), 800.f,
		UDamageType::StaticClass(), {}, this, GetController(), true);
}

void ASupermanCharacter::ApplyLandingImpact()
{
	// Crater shockwave on landing at high speed
	const float LandSpeed = PreviousSpeed;
	if (LandSpeed > 1000.f)
	{
		const float Radius = FMath::GetMappedRangeValueClamped(
			FVector2D(1000.f, 16000.f), FVector2D(300.f, 2000.f), LandSpeed);

		UGameplayStatics::ApplyRadialDamage(
			GetWorld(), LandSpeed * 0.05f, GetActorLocation(), Radius,
			UDamageType::StaticClass(), {}, this, GetController(), true);
	}
}

float ASupermanCharacter::GetCurrentFlightSpeed() const
{
	return GetVelocity().Size() * 0.036f; // cm/s → km/h
}

// ── Powers ─────────────────────────────────────────────────────────────────────

void ASupermanCharacter::StartHeatVision()
{
	if (Stats.SolarEnergy < 5.f || IsWeakenedByKryptonite()) return;

	bHeatVisionActive = true;
	ActivePower = EActivePower::HeatVision;
	HeatVisionLeftEye->Activate(true);
	HeatVisionRightEye->Activate(true);
	SetSupermanState(ESupermanState::UsingPower);
	OnPowerActivated.Broadcast(EActivePower::HeatVision, true);
}

void ASupermanCharacter::StopHeatVision()
{
	bHeatVisionActive = false;
	ActivePower = EActivePower::None;
	HeatVisionLeftEye->Deactivate();
	HeatVisionRightEye->Deactivate();

	SetSupermanState(bIsFlying ? ESupermanState::Flying : ESupermanState::Grounded);
	OnPowerActivated.Broadcast(EActivePower::HeatVision, false);
}

void ASupermanCharacter::UpdateHeatVision(float DeltaTime)
{
	if (!bHeatVisionActive) return;

	Stats.SolarEnergy = FMath::Max(0.f, Stats.SolarEnergy - 8.f * DeltaTime);
	HeatVisionTimer  += DeltaTime;

	// Raycast beam hit every tick
	FHitResult Hit;
	const FVector Start = FollowCamera->GetComponentLocation();
	const FVector End   = Start + FollowCamera->GetForwardVector() * 5000.f;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		// Apply burn damage to hit actor
		if (AActor* HitActor = Hit.GetActor())
		{
			const float BeamDamage = 50.f * DeltaTime * (1.f - Stats.KryptoniteExposure / 100.f);
			UGameplayStatics::ApplyPointDamage(
				HitActor, BeamDamage, GetActorForwardVector(),
				Hit, GetController(), this, UDamageType::StaticClass());
		}
	}

	if (Stats.SolarEnergy <= 0.f) StopHeatVision();
}

void ASupermanCharacter::StartFreezeBreath()
{
	if (Stats.SolarEnergy < 5.f || IsWeakenedByKryptonite()) return;

	bFreezeBreathActive = true;
	ActivePower = EActivePower::FreezeBreath;
	FreezeBreathNiagara->Activate(true);
	SetSupermanState(ESupermanState::UsingPower);
	OnPowerActivated.Broadcast(EActivePower::FreezeBreath, true);
}

void ASupermanCharacter::StopFreezeBreath()
{
	bFreezeBreathActive = false;
	ActivePower = EActivePower::None;
	FreezeBreathNiagara->Deactivate();

	SetSupermanState(bIsFlying ? ESupermanState::Flying : ESupermanState::Grounded);
	OnPowerActivated.Broadcast(EActivePower::FreezeBreath, false);
}

void ASupermanCharacter::UpdateFreezeBreath(float DeltaTime)
{
	if (!bFreezeBreathActive) return;

	Stats.SolarEnergy   = FMath::Max(0.f, Stats.SolarEnergy - 6.f * DeltaTime);
	FreezeBreathTimer  += DeltaTime;

	// Cone overlap — freeze all characters in front
	TArray<FOverlapResult> Overlaps;
	FCollisionShape Cone = FCollisionShape::MakeSphere(600.f);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->OverlapMultiByChannel(Overlaps, GetActorLocation() + GetActorForwardVector() * 300.f,
		FQuat::Identity, ECC_Pawn, Cone, Params))
	{
		for (const FOverlapResult& Overlap : Overlaps)
		{
			if (AActor* Target = Overlap.GetActor())
			{
				// Notify target of freeze (handled by their own components)
				Target->Tags.AddUnique(FName("Frozen"));
			}
		}
	}

	if (Stats.SolarEnergy <= 0.f) StopFreezeBreath();
}

void ASupermanCharacter::ToggleSuperSpeed()
{
	if (IsWeakenedByKryptonite()) return;

	bIsSuperSpeedActive = !bIsSuperSpeedActive;
	ActivePower = bIsSuperSpeedActive ? EActivePower::SuperSpeed : EActivePower::None;

	GetCharacterMovement()->MaxWalkSpeed = bIsSuperSpeedActive ? 6000.f : 600.f;
	OnPowerActivated.Broadcast(EActivePower::SuperSpeed, bIsSuperSpeedActive);
}

void ASupermanCharacter::ToggleXRayVision()
{
	bIsXRayVisionActive = !bIsXRayVisionActive;
	ActivePower = bIsXRayVisionActive ? EActivePower::XRayVision : EActivePower::None;
	OnPowerActivated.Broadcast(EActivePower::XRayVision, bIsXRayVisionActive);
}

void ASupermanCharacter::ToggleSuperHearing()
{
	bIsSuperHearingActive = !bIsSuperHearingActive;
	ActivePower = bIsSuperHearingActive ? EActivePower::SuperHearing : EActivePower::None;
	OnPowerActivated.Broadcast(EActivePower::SuperHearing, bIsSuperHearingActive);
}

void ASupermanCharacter::UpdateSolarEnergyRegen(float DeltaTime)
{
	// Passive regen when not using powers
	if (!bHeatVisionActive && !bFreezeBreathActive && !bIsBoostFlying)
	{
		const float RegenRate = bIsFlying ? 3.f : 5.f;
		Stats.SolarEnergy = FMath::Min(Stats.MaxSolarEnergy, Stats.SolarEnergy + RegenRate * DeltaTime);
	}

	// Kryptonite fades over time once away from source
	if (Stats.KryptoniteExposure > 0.f)
		Stats.KryptoniteExposure = FMath::Max(0.f, Stats.KryptoniteExposure - 1.f * DeltaTime);
}

void ASupermanCharacter::UpdateCapePhysics(float DeltaTime)
{
	// Cape responds to flight speed — blend between "hanging" and "blown back"
	if (CapeMesh)
	{
		const float SpeedNorm = FMath::Clamp(GetVelocity().Size() / (Stats.FlightSpeed * Stats.BoostMultiplier), 0.f, 1.f);
		CapeMesh->SetScalarParameterValueOnMaterials(TEXT("FlowStrength"), SpeedNorm);
	}
}

// ── Combat ─────────────────────────────────────────────────────────────────────

void ASupermanCharacter::LightAttack()
{
	if (SupermanState == ESupermanState::Stunned || SupermanState == ESupermanState::Cutscene) return;
	SetSupermanState(ESupermanState::InCombat);
	// Triggers anim montage + hit detection via AnimNotify
	PowerComponent->TriggerLightAttack();
}

void ASupermanCharacter::HeavyAttack()
{
	if (SupermanState == ESupermanState::Stunned || SupermanState == ESupermanState::Cutscene) return;
	SetSupermanState(ESupermanState::InCombat);
	PowerComponent->TriggerHeavyAttack();
}

void ASupermanCharacter::Dodge()
{
	if (SupermanState == ESupermanState::Stunned) return;

	const FVector DodgeDir = GetLastMovementInputVector().IsNearlyZero()
		? -GetActorForwardVector()
		: GetLastMovementInputVector();

	LaunchCharacter(DodgeDir * 1500.f, true, false);
}

void ASupermanCharacter::AttemptGrab()
{
	if (GrabbedActor) return; // already holding something

	FHitResult Hit;
	const FVector Start = GetActorLocation();
	const FVector End   = Start + GetActorForwardVector() * 500.f;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, Params))
	{
		GrabbedActor = Hit.GetActor();
		if (GrabbedActor)
		{
			// Attach to hand socket
			GrabbedActor->AttachToComponent(GetMesh(),
				FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				TEXT("GrabSocket_R"));

			SetSupermanState(ESupermanState::Grabbing);
		}
	}
}

void ASupermanCharacter::ThrowGrabbedObject()
{
	if (!GrabbedActor) return;

	GrabbedActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(GrabbedActor->GetRootComponent()))
	{
		Prim->SetSimulatePhysics(true);
		const FVector ThrowDir  = FollowCamera->GetForwardVector();
		const float   ThrowForce = bIsBoostFlying ? 500000.f : 150000.f;
		Prim->AddImpulse(ThrowDir * ThrowForce, NAME_None, true);
	}

	GrabbedActor = nullptr;
	SetSupermanState(bIsFlying ? ESupermanState::Flying : ESupermanState::Grounded);
}

// ── Damage / Kryptonite ────────────────────────────────────────────────────────

float ASupermanCharacter::TakeDamage(float Damage, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	// Superman is invulnerable unless weakened by kryptonite
	if (!IsWeakenedByKryptonite()) return 0.f;

	const float ScaledDamage = Damage * (Stats.KryptoniteExposure / 100.f);
	Stats.Health = FMath::Max(0.f, Stats.Health - ScaledDamage);

	return ScaledDamage;
}

void ASupermanCharacter::AddKryptoniteExposure(float Amount)
{
	Stats.KryptoniteExposure = FMath::Clamp(Stats.KryptoniteExposure + Amount, 0.f, 100.f);
	OnKryptoniteExposure.Broadcast(Stats.KryptoniteExposure);

	if (Stats.KryptoniteExposure > 75.f)
		SetSupermanState(ESupermanState::Stunned);
}

void ASupermanCharacter::RemoveKryptoniteExposure(float Amount)
{
	Stats.KryptoniteExposure = FMath::Max(0.f, Stats.KryptoniteExposure - Amount);
	OnKryptoniteExposure.Broadcast(Stats.KryptoniteExposure);

	if (SupermanState == ESupermanState::Stunned && Stats.KryptoniteExposure < 50.f)
		SetSupermanState(ESupermanState::Grounded);
}

// ── Stats helpers ──────────────────────────────────────────────────────────────

float ASupermanCharacter::GetHealthPercent() const
{
	return Stats.MaxHealth > 0.f ? Stats.Health / Stats.MaxHealth : 0.f;
}

float ASupermanCharacter::GetSolarEnergyPercent() const
{
	return Stats.MaxSolarEnergy > 0.f ? Stats.SolarEnergy / Stats.MaxSolarEnergy : 0.f;
}

void ASupermanCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
