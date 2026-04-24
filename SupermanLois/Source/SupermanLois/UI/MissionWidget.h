#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Systems/MissionSystem.h"
#include "MissionWidget.generated.h"

class UVerticalBox;
class UTextBlock;
class UProgressBar;

UCLASS()
class SUPERMANLOIS_API UMissionWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget)) UTextBlock*   MissionTitleText;
	UPROPERTY(meta=(BindWidget)) UVerticalBox* ObjectivesBox;
	UPROPERTY(meta=(BindWidget)) UProgressBar* MissionProgressBar;

	UFUNCTION(BlueprintCallable)
	void SetActiveMission(const FMissionData& Mission);

	UFUNCTION(BlueprintCallable)
	void UpdateObjective(const FMissionObjective& Objective);

	UFUNCTION(BlueprintCallable)
	void ClearMission();

private:
	void RebuildObjectivesList(const TArray<FMissionObjective>& Objectives);
};
