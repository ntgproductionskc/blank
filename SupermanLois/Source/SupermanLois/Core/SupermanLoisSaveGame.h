#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SupermanLoisSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FSaveWorldState
{
	GENERATED_BODY()
	UPROPERTY() float  TimeOfDay         = 8.f;
	UPROPERTY() bool   bIsRaining        = false;
	UPROPERTY() float  PlayerReputation  = 50.f;
	UPROPERTY() int32  CiviliansRescued  = 0;
};

UCLASS()
class SUPERMANLOIS_API USupermanLoisSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	static const FString SaveSlotName;
	static const int32   UserIndex = 0;

	UPROPERTY() FString          PlayerName          = TEXT("Clark Kent");
	UPROPERTY() int32            CurrentChapter      = 1;
	UPROPERTY() TArray<FName>    CompletedMissions;
	UPROPERTY() TMap<FName,bool> StoryFlags;
	UPROPERTY() FSaveWorldState  WorldState;
	UPROPERTY() FTransform       LastCheckpointTransform;
	UPROPERTY() float            TotalPlaytimeSeconds = 0.f;
	UPROPERTY() TMap<FName,int32> CollectiblesFound;
	UPROPERTY() float            BestSpeedRecord      = 0.f; // km/h record
};
