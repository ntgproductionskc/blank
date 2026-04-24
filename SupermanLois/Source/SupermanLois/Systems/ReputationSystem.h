#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ReputationSystem.generated.h"

class ASupermanLoisGameMode;

UENUM(BlueprintType)
enum class EReputationTier : uint8
{
	Unknown,        // 0–20
	Vigilante,      // 20–40
	Hero,           // 40–60  (default start)
	Champion,       // 60–80
	Legend          // 80–100
};

USTRUCT(BlueprintType)
struct FReputationEvent
{
	GENERATED_BODY()
	UPROPERTY() FText   Description;
	UPROPERTY() float   Delta;
	UPROPERTY() float   Timestamp;
};

UCLASS()
class SUPERMANLOIS_API UReputationSystem : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(ASupermanLoisGameMode* GameMode);

	UFUNCTION(BlueprintCallable, Category="Reputation")
	void AddReputation(float Delta, const FText& Reason);

	UFUNCTION(BlueprintPure, Category="Reputation")
	float GetReputation() const { return CurrentReputation; }

	UFUNCTION(BlueprintPure, Category="Reputation")
	EReputationTier GetTier() const;

	UFUNCTION(BlueprintPure, Category="Reputation")
	FText GetTierDisplayName() const;

	UFUNCTION(BlueprintPure, Category="Reputation")
	float GetTierProgress() const; // 0–1 within current tier

	// Affects dialogue, NPC behavior, enemy aggression
	UFUNCTION(BlueprintPure, Category="Reputation")
	bool IsPubliclyTrusted() const { return CurrentReputation >= 60.f; }

	// Reputation events
	static constexpr float REP_CivilianRescued      = +2.f;
	static constexpr float REP_EmergencyResolved     = +5.f;
	static constexpr float REP_MissionComplete       = +10.f;
	static constexpr float REP_PropertyDamage        = -1.f;
	static constexpr float REP_CivilianHurt          = -5.f;
	static constexpr float REP_CivilianKilled        = -15.f;
	static constexpr float REP_VillainDefeated       = +8.f;
	static constexpr float REP_EmergencyFailed       = -8.f;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnReputationChanged, float, EReputationTier);
	FOnReputationChanged OnReputationChanged;

private:
	UPROPERTY()
	ASupermanLoisGameMode* OwningGameMode;

	float CurrentReputation = 50.f;
	TArray<FReputationEvent> RecentEvents;
};
