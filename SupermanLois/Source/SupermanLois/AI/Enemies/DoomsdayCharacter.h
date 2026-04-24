#pragma once

#include "CoreMinimal.h"
#include "BaseVillain.h"
#include "DoomsdayCharacter.generated.h"

// Doomsday — pure savage, adapts to damage taken, bone spikes, unstoppable charge
UCLASS()
class SUPERMANLOIS_API ADoomsdayCharacter : public ABaseVillain
{
	GENERATED_BODY()

public:
	ADoomsdayCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

public:
	virtual void UseSignatureAbility() override;
	virtual void OnDefeated_Implementation() override;

	UFUNCTION(BlueprintCallable, Category="Combat")
	void BoneSpikeSlam();       // ground pound, massive AoE

	UFUNCTION(BlueprintCallable, Category="Combat")
	void UnstoppableCharge();   // bulldoze through obstacles

	UFUNCTION(BlueprintCallable, Category="Combat")
	void AdaptArmor();          // immunity to damage type that just hit

	// Config
	UPROPERTY(EditDefaultsOnly, Category="Doomsday")
	float SlamRadius           = 900.f;

	UPROPERTY(EditDefaultsOnly, Category="Doomsday")
	float SlamDamage           = 300.f;

	UPROPERTY(EditDefaultsOnly, Category="Doomsday")
	float ChargeSpeed          = 4000.f;

	UPROPERTY(EditDefaultsOnly, Category="Doomsday")
	float ChargeDistance       = 3000.f;

	UPROPERTY(BlueprintReadOnly, Category="Doomsday")
	int32 AdaptationCount      = 0;  // how many times he's been hit hard

	// Resistances he gains via adaptation
	UPROPERTY(BlueprintReadOnly, Category="Doomsday")
	TArray<TSubclassOf<UDamageType>> AcquiredResistances;

private:
	bool  bIsCharging         = false;
	float ChargeTimer         = 0.f;
	float NextSlamCooldown    = 5.f;
	float NextChargeCooldown  = 8.f;
	FVector ChargeDirection   = FVector::ZeroVector;
};
