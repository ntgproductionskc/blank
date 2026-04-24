#pragma once

#include "CoreMinimal.h"
#include "BaseVillain.h"
#include "InverseSupermanCharacter.generated.h"

// The Inverse Superman (aka Bizarro-world Superman from the Inverse World)
// Tactical, intelligent — mirrors Superman's powers almost exactly
UCLASS()
class SUPERMANLOIS_API AInverseSupermanCharacter : public ABaseVillain
{
	GENERATED_BODY()

public:
	AInverseSupermanCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	virtual void UseSignatureAbility() override;
	virtual void StartAttackPhase() override;
	virtual void OnDefeated_Implementation() override;

	// Mirrors each of Superman's power abilities
	UFUNCTION(BlueprintCallable) void UseInverseHeatVision();
	UFUNCTION(BlueprintCallable) void UseInverseFreezeBreath();
	UFUNCTION(BlueprintCallable) void UseInverseFlightCharge();
	UFUNCTION(BlueprintCallable) void UseSolarFlare();           // unique — massive AoE energy burst
	UFUNCTION(BlueprintCallable) void CounterSupermanMove();     // reads and counters player attacks

	// Config
	UPROPERTY(EditDefaultsOnly) float SolarFlareRadius   = 1200.f;
	UPROPERTY(EditDefaultsOnly) float SolarFlareDamage   = 500.f;
	UPROPERTY(EditDefaultsOnly) float SolarFlareCooldown = 30.f;

	UPROPERTY(VisibleAnywhere) UNiagaraComponent* InverseHeatVisionComp;
	UPROPERTY(VisibleAnywhere) UNiagaraComponent* SolarFlareComp;

	// Tactical awareness
	UPROPERTY(BlueprintReadOnly)
	bool bIsMirroringPlayer = false;

private:
	float SolarFlareTimer    = 30.f;
	float AbilityRotateTimer = 0.f;
	int32 CurrentAbilityIdx  = 0;

	void SelectNextAbility();
};
