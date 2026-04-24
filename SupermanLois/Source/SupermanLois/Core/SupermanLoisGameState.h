#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SupermanLoisGameState.generated.h"

USTRUCT(BlueprintType)
struct FWorldState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) float TimeOfDay         = 8.f;    // 0–24 hour
	UPROPERTY(BlueprintReadWrite) float WeatherIntensity  = 0.f;
	UPROPERTY(BlueprintReadWrite) bool  bIsRaining        = false;
	UPROPERTY(BlueprintReadWrite) bool  bIsStorming       = false;
	UPROPERTY(BlueprintReadWrite) int32 ActiveEmergencies = 0;
	UPROPERTY(BlueprintReadWrite) int32 CiviliansRescued  = 0;
	UPROPERTY(BlueprintReadWrite) int32 CiviliansLost     = 0;
	UPROPERTY(BlueprintReadWrite) float PlayerReputation  = 50.f;  // 0–100
};

UCLASS()
class SUPERMANLOIS_API ASupermanLoisGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category="World")
	FWorldState WorldState;

	UPROPERTY(BlueprintReadWrite, Category="Story")
	int32 CurrentStoryChapter = 1;

	UPROPERTY(BlueprintReadWrite, Category="Story")
	TArray<FName> CompletedMissions;

	UPROPERTY(BlueprintReadWrite, Category="Story")
	TMap<FName, bool> StoryFlags; // global narrative state flags

	UFUNCTION(BlueprintCallable, Category="Story")
	void SetStoryFlag(FName Flag, bool Value) { StoryFlags.FindOrAdd(Flag) = Value; }

	UFUNCTION(BlueprintPure, Category="Story")
	bool GetStoryFlag(FName Flag) const
	{
		const bool* Val = StoryFlags.Find(Flag);
		return Val ? *Val : false;
	}
};
