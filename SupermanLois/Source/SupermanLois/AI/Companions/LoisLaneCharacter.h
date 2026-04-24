#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LoisLaneCharacter.generated.h"

class ASupermanCharacter;

UENUM(BlueprintType)
enum class ELoisState : uint8
{
	Idle,
	Investigating,    // following a lead
	InDanger,         // needs rescue
	Rescued,
	FollowingClark,
	AtDailyPlanet,
	Captured,
	CallingForHelp
};

UCLASS()
class SUPERMANLOIS_API ALoisLaneCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ALoisLaneCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

public:
	UPROPERTY(BlueprintReadOnly) ELoisState LoisState = ELoisState::AtDailyPlanet;
	UPROPERTY(EditDefaultsOnly)  float      Health    = 100.f;

	// Dialogue systems
	UPROPERTY(EditDefaultsOnly, Category="Dialogue")
	TArray<USoundBase*> DangerCalloutLines;

	UPROPERTY(EditDefaultsOnly, Category="Dialogue")
	TArray<USoundBase*> RescuedThankYouLines;

	UPROPERTY(EditDefaultsOnly, Category="Dialogue")
	TArray<USoundBase*> ClarkCommentLines;   // witty remarks to Clark

	// Lois carries a camera — snapshots become news articles in the game world
	UFUNCTION(BlueprintCallable, Category="Lois")
	void TakePhoto(FVector Subject);

	UFUNCTION(BlueprintCallable, Category="Lois")
	void InvestigateLead(FVector Location, FText LeadDescription);

	UFUNCTION(BlueprintCallable, Category="Lois")
	void CallForSuperman();

	UFUNCTION(BlueprintCallable, Category="Lois")
	void SetCaptured(bool bCaptured);

	UFUNCTION(BlueprintCallable, Category="Lois")
	void OnRescuedBySuperman();

	// Mission helper — returns how much of the current lead she's uncovered (0–1)
	UFUNCTION(BlueprintPure, Category="Lois")
	float GetInvestigationProgress() const { return InvestigationProgress; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoisInDanger);
	UPROPERTY(BlueprintAssignable) FOnLoisInDanger OnLoisInDanger;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoisPhotoTaken, FVector, PhotoLocation);
	UPROPERTY(BlueprintAssignable) FOnLoisPhotoTaken OnLoisPhotoTaken;

private:
	float InvestigationProgress  = 0.f;
	float InvestigationTarget    = 0.f;
	bool  bIsCaptured            = false;
	FVector CurrentLeadLocation  = FVector::ZeroVector;

	void UpdateInvestigation(float DeltaTime);
	void LookAround();
	void PlayVoiceLine(const TArray<USoundBase*>& Lines);
};
