#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FirefighterAI.generated.h"

UENUM(BlueprintType)
enum class EFirefighterState : uint8
{
	Responding,
	DeployingHose,
	FightingFire,
	RescuingCivilian,
	Retreating,
	StandingBy
};

UCLASS()
class SUPERMANLOIS_API AFirefighterAI : public ACharacter
{
	GENERATED_BODY()

public:
	AFirefighterAI();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

public:
	UPROPERTY(BlueprintReadOnly) EFirefighterState FFState = EFirefighterState::Responding;
	UPROPERTY(EditDefaultsOnly) float Health = 150.f;
	UPROPERTY(EditDefaultsOnly) float HoseRange = 600.f;
	UPROPERTY(EditDefaultsOnly) float FireSuppressRate = 10.f; // fire intensity reduced per second

	// Hose VFX
	UPROPERTY(VisibleAnywhere) UNiagaraComponent* HoseStreamNiagara;

	UFUNCTION(BlueprintCallable) void RespondToFire(FVector FireLocation);
	UFUNCTION(BlueprintCallable) void RescueCivilian(AActor* Civilian, FVector SafeZone);
	UFUNCTION(BlueprintCallable) void DeployHose();
	UFUNCTION(BlueprintCallable) void RetractHose();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFirefighterDown);
	UPROPERTY(BlueprintAssignable) FOnFirefighterDown OnFirefighterDown;

private:
	FVector FireTarget      = FVector::ZeroVector;
	float   HoseTimer       = 0.f;
	bool    bHoseDeployed   = false;
};
