#include "SupermanMovementComponent.h"
#include "GameFramework/Character.h"

USupermanMovementComponent::USupermanMovementComponent()
{
	// Base flying defaults — overridden dynamically
	MaxFlySpeed                  = 2000.f;
	BrakingDecelerationFlying    = 2000.f;
	AirControl                   = 1.f;
	GravityScale                 = 1.75f;
	MaxAcceleration              = 4096.f;
	BrakingFrictionFactor        = 0.f;
}

void USupermanMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CurrentAirSpeed = Velocity.Size() * 0.036f; // cm/s to km/h

	if (IsFlying())
		ApplyAtmosphericDrag(DeltaTime);

	// Warn at ~75% of sonic boom threshold (~Mach 0.75)
	bApproachingSonicBarrier = (CurrentAirSpeed > 900.f && CurrentAirSpeed < 1200.f);
}

float USupermanMovementComponent::GetMaxSpeed() const
{
	return MaxFlySpeed;
}

void USupermanMovementComponent::ApplyAtmosphericDrag(float DeltaTime)
{
	const float Speed = Velocity.Size();
	if (Speed < 100.f) return;

	// Quadratic drag — negligible at low speed, significant above Mach
	const float DragForce = AtmosphericDragCoefficient * Speed * Speed;
	const float DragDelta = FMath::Min(DragForce * DeltaTime, Speed * 0.1f);

	Velocity -= Velocity.GetSafeNormal() * DragDelta;
}

float USupermanMovementComponent::CalculateTerminalVelocityResistance() const
{
	// Returns a 0-1 multiplier that caps speed near terminal velocity (for realism)
	const float SpeedFraction = CurrentAirSpeed / 35000.f; // max ~Mach 30
	return 1.f - FMath::Clamp(SpeedFraction * SpeedFraction, 0.f, 0.95f);
}

void USupermanMovementComponent::PhysFalling(float deltaTime, int32 Iterations)
{
	// Keep gravity during fall unless player activates flight
	Super::PhysFalling(deltaTime, Iterations);
}
