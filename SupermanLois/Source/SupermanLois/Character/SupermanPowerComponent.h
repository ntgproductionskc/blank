#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SupermanPowerComponent.generated.h"

class ASupermanCharacter;
class UAnimMontage;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SUPERMANLOIS_API USupermanPowerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USupermanPowerComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

public:
	// ── Attack montages (assigned in Blueprint) ────────────────────────────────
	UPROPERTY(EditDefaultsOnly, Category="Animations")
	UAnimMontage* LightAttackMontage_Ground;

	UPROPERTY(EditDefaultsOnly, Category="Animations")
	UAnimMontage* LightAttackMontage_Air;

	UPROPERTY(EditDefaultsOnly, Category="Animations")
	UAnimMontage* HeavyAttackMontage_Ground;

	UPROPERTY(EditDefaultsOnly, Category="Animations")
	UAnimMontage* HeavyAttackMontage_Air;

	UPROPERTY(EditDefaultsOnly, Category="Animations")
	UAnimMontage* FinisherMontage;

	UPROPERTY(EditDefaultsOnly, Category="Animations")
	UAnimMontage* GrabMontage;

	// ── Combat config ─────────────────────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float LightAttackDamage = 150.f;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float HeavyAttackDamage = 400.f;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float AttackRadius      = 120.f;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float AttackRange       = 220.f;

	// ── Combo system ──────────────────────────────────────────────────────────
	UPROPERTY(BlueprintReadOnly, Category="Combat")
	int32 ComboCount = 0;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	int32 MaxCombo = 5;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float ComboWindow = 1.2f;   // seconds to chain next attack

	// ── Interface ─────────────────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category="Combat")
	void TriggerLightAttack();

	UFUNCTION(BlueprintCallable, Category="Combat")
	void TriggerHeavyAttack();

	UFUNCTION(BlueprintCallable, Category="Combat")
	void PerformAttackHitDetection(bool bIsHeavy);

	UFUNCTION(BlueprintCallable, Category="Combat")
	void TriggerFinisher(AActor* Target);

	// ── Delegates ─────────────────────────────────────────────────────────────
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnComboHit, int32, ComboCount, float, Damage);
	UPROPERTY(BlueprintAssignable) FOnComboHit OnComboHit;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnComboReset);
	UPROPERTY(BlueprintAssignable) FOnComboReset OnComboReset;

private:
	UPROPERTY()
	ASupermanCharacter* OwnerSuperman;

	float ComboResetTimer = 0.f;
	bool  bAttacking      = false;

	void ResetCombo();
};
