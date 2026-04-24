#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PoliceOfficerAI.generated.h"

UENUM(BlueprintType)
enum class EPoliceState : uint8
{
	Responding,       // driving/running to scene
	Securing,         // establishing perimeter
	Engaging,         // shooting at enemy
	Evacuating,       // moving civilians away
	RequestingBackup,
	MedicalAssist,
	Idle
};

UCLASS()
class SUPERMANLOIS_API APoliceOfficerAI : public ACharacter
{
	GENERATED_BODY()

public:
	APoliceOfficerAI();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

public:
	UPROPERTY(BlueprintReadOnly, Category="State")
	EPoliceState PoliceState = EPoliceState::Responding;

	UPROPERTY(EditDefaultsOnly, Category="Config")
	float Health             = 100.f;

	UPROPERTY(EditDefaultsOnly, Category="Config")
	float AttackDamage       = 15.f;    // standard 9mm — useless against powered villains

	UPROPERTY(EditDefaultsOnly, Category="Config")
	float ShootRange         = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category="Config")
	float ShootInterval      = 0.4f;

	// Dialogue lines triggered on Superman arrival
	UPROPERTY(EditDefaultsOnly, Category="Audio")
	TArray<USoundBase*> SupermanGreetingLines;

	UPROPERTY(EditDefaultsOnly, Category="Audio")
	TArray<USoundBase*> RadioChatterSounds;

	UFUNCTION(BlueprintCallable, Category="Behavior")
	void RespondToEmergency(FVector EmergencyLocation);

	UFUNCTION(BlueprintCallable, Category="Behavior")
	void EvacuateCivilian(AActor* Civilian);

	UFUNCTION(BlueprintCallable, Category="Behavior")
	void SetupPerimeter(FVector Center, float Radius);

	UFUNCTION(BlueprintCallable, Category="Behavior")
	void OnSupermanArrived();   // behavior changes when Superman shows up

	// Radio communication
	UFUNCTION(BlueprintCallable, Category="Radio")
	void BroadcastRadio(const FString& Message);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOfficerDown);
	UPROPERTY(BlueprintAssignable) FOnOfficerDown OnOfficerDown;

private:
	FVector EmergencyTarget       = FVector::ZeroVector;
	float   ShootCooldown         = 0.f;
	bool    bSupermanOnScene      = false;
	bool    bPerimeterEstablished = false;

	AActor* CurrentEngageTarget = nullptr;

	void AttemptShoot();
	void LookForThreats();
	void PlayRadioChatter();

	FTimerHandle RadioChatterTimer;
};
