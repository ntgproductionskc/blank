#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CivilianCharacter.generated.h"

UENUM(BlueprintType)
enum class ECivilianBehavior : uint8
{
	WalkingAbout,      // normal city life
	PanickingFlee,     // running from danger
	CoweredInPlace,    // too scared to run
	WatchingSuperman,  // looking up in awe
	Trapped,           // needs rescue
	Injured,           // needs medical help
	Evacuating,        // being guided by police/paramedic
	Rescued            // carried by Superman or guided to safety
};

UCLASS()
class SUPERMANLOIS_API ACivilianCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACivilianCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

public:
	UPROPERTY(BlueprintReadOnly, Category="State")
	ECivilianBehavior CivilianBehavior = ECivilianBehavior::WalkingAbout;

	UPROPERTY(EditDefaultsOnly, Category="Config")
	float Health = 100.f;

	// Panic radius — if a villain or explosion is within this range, civilian flees
	UPROPERTY(EditDefaultsOnly, Category="Config")
	float PanicRadius = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category="Config")
	float RunSpeed = 500.f;

	UPROPERTY(EditDefaultsOnly, Category="Audio")
	TArray<USoundBase*> PanicLines;

	UPROPERTY(EditDefaultsOnly, Category="Audio")
	TArray<USoundBase*> AweLines; // "It's Superman!" etc.

	UFUNCTION(BlueprintCallable) void StartPanicking(FVector ThreatLocation);
	UFUNCTION(BlueprintCallable) void StopPanicking();
	UFUNCTION(BlueprintCallable) void WitnessSuperman(ACharacter* Superman);
	UFUNCTION(BlueprintCallable) void SetTrapped(bool bTrapped);
	UFUNCTION(BlueprintCallable) void OnRescued();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCivilianTrapped, ACivilianCharacter*, Civilian);
	UPROPERTY(BlueprintAssignable) FOnCivilianTrapped OnCivilianTrapped;

private:
	FVector FleeTarget = FVector::ZeroVector;
	float   PanicTimer = 0.f;
	bool    bIsTrapped = false;

	void PickFleeDirection(FVector ThreatLocation);
	void UpdatePanicBehavior(float DeltaTime);
	void CheckForNearbyThreats();
};
