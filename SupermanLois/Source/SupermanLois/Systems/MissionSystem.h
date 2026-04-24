#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "MissionSystem.generated.h"

class ASupermanLoisGameMode;

UENUM(BlueprintType)
enum class EMissionType : uint8
{
	Story,      // main narrative arc
	Side,       // optional
	Emergency,  // tied to EmergencySystem
	Daily,      // resets each in-game day
	Companion   // involves Lois / Jordan / Jonathan
};

UENUM(BlueprintType)
enum class EMissionStatus : uint8
{
	Locked,
	Available,
	Active,
	Completed,
	Failed
};

UENUM(BlueprintType)
enum class EObjectiveType : uint8
{
	ReachLocation,
	DefeatEnemy,
	RescueCivilian,
	ProtectTarget,
	DestroyObject,
	EscortTarget,
	SurviveWaves,
	UseAbility,
	CollectItem,
	TimeLimit
};

USTRUCT(BlueprintType)
struct FMissionObjective
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) FText            Description;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) EObjectiveType   Type            = EObjectiveType::ReachLocation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) int32            RequiredCount   = 1;
	UPROPERTY(BlueprintReadOnly)                    int32            CurrentCount    = 0;
	UPROPERTY(BlueprintReadOnly)                    bool             bCompleted      = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) bool             bOptional       = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) FVector          TargetLocation  = FVector::ZeroVector;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) FName            TargetTag;
};

USTRUCT(BlueprintType)
struct FMissionData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) FName                    MissionID;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) FText                    Title;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) FText                    Description;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) EMissionType             Type            = EMissionType::Story;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) int32                    Chapter         = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) TArray<FMissionObjective> Objectives;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) TArray<FName>            PrerequisiteMissions;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) float                    ReputationReward = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) FName                    NextMissionID;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) bool                     bHasCutscene    = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) FName                    StartCutsceneID;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) FName                    EndCutsceneID;
};

UCLASS()
class SUPERMANLOIS_API UMissionSystem : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(ASupermanLoisGameMode* GameMode);

	UFUNCTION(BlueprintCallable, Category="Missions")
	void StartMission(FName MissionID);

	UFUNCTION(BlueprintCallable, Category="Missions")
	void UpdateObjective(FName MissionID, EObjectiveType ObjectiveType, int32 CountDelta = 1);

	UFUNCTION(BlueprintCallable, Category="Missions")
	void CompleteMission(FName MissionID, bool bSuccess);

	UFUNCTION(BlueprintPure, Category="Missions")
	FMissionData* GetActiveMission() const;

	UFUNCTION(BlueprintPure, Category="Missions")
	TArray<FMissionData*> GetAvailableMissions() const;

	UFUNCTION(BlueprintPure, Category="Missions")
	bool IsMissionComplete(FName MissionID) const;

	void Tick(float DeltaTime);

	// Delegates
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnMissionStarted, const FMissionData&);
	FOnMissionStarted OnMissionStarted;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnObjectiveUpdated, FName, const FMissionObjective&);
	FOnObjectiveUpdated OnObjectiveUpdated;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMissionCompleted, FName, bool);
	FOnMissionCompleted OnMissionCompleted;

private:
	UPROPERTY()
	ASupermanLoisGameMode* OwningGameMode;

	TArray<FMissionData>     AllMissions;
	TMap<FName, EMissionStatus> MissionStatusMap;
	FName                    ActiveMissionID;

	void LoadMissionData();
	void PopulateStoryMissions();
	void CheckObjectiveCompletion(FName MissionID);
	bool ArePrerequisitesMet(const FMissionData& Mission) const;
};
