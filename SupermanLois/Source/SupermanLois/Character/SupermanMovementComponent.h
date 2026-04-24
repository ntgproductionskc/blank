#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SupermanMovementComponent.generated.h"

UCLASS()
class SUPERMANLOIS_API USupermanMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	USupermanMovementComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	virtual float GetMaxSpeed() const override;

	// Atmospheric re-entry — surface friction simulation at very high speeds
	UPROPERTY(EditDefaultsOnly, Category="Flight")
	float AtmosphericDragCoefficient = 0.002f;

	// Minimum speed to stay aloft (hover threshold)
	UPROPERTY(EditDefaultsOnly, Category="Flight")
	float HoverMinSpeed = 30.f;

	// At boost, tilt character forward
	UPROPERTY(EditDefaultsOnly, Category="Flight")
	float MaxBoostTiltDegrees = 35.f;

	UPROPERTY(BlueprintReadOnly)
	float CurrentAirSpeed = 0.f; // km/h

	UPROPERTY(BlueprintReadOnly)
	bool bApproachingSonicBarrier = false;

protected:
	virtual void PhysFalling(float deltaTime, int32 Iterations) override;

private:
	void ApplyAtmosphericDrag(float DeltaTime);
	float CalculateTerminalVelocityResistance() const;
};
