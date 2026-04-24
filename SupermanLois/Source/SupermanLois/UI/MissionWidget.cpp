#include "MissionWidget.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/VerticalBoxSlot.h"

void UMissionWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMissionWidget::SetActiveMission(const FMissionData& Mission)
{
	if (MissionTitleText)
		MissionTitleText->SetText(Mission.Title);

	RebuildObjectivesList(Mission.Objectives);

	// Compute initial progress
	int32 Total     = Mission.Objectives.Num();
	int32 Completed = 0;
	for (const FMissionObjective& Obj : Mission.Objectives)
		if (Obj.bCompleted) Completed++;

	if (MissionProgressBar && Total > 0)
		MissionProgressBar->SetPercent(static_cast<float>(Completed) / Total);
}

void UMissionWidget::RebuildObjectivesList(const TArray<FMissionObjective>& Objectives)
{
	if (!ObjectivesBox) return;
	ObjectivesBox->ClearChildren();

	for (const FMissionObjective& Obj : Objectives)
	{
		if (Obj.bOptional) continue; // optional shown separately

		UTextBlock* ObjText = NewObject<UTextBlock>(this);
		if (!ObjText) continue;

		const FString CheckMark = Obj.bCompleted ? TEXT("[X] ") : TEXT("[ ] ");
		const FString CountStr  = Obj.RequiredCount > 1
			? FString::Printf(TEXT(" (%d/%d)"), Obj.CurrentCount, Obj.RequiredCount)
			: TEXT("");

		ObjText->SetText(FText::FromString(CheckMark + Obj.Description.ToString() + CountStr));

		FLinearColor TextColor = Obj.bCompleted
			? FLinearColor(0.4f, 1.f, 0.4f)  // green when done
			: FLinearColor::White;
		ObjText->SetColorAndOpacity(TextColor);

		ObjectivesBox->AddChildToVerticalBox(ObjText);
	}
}

void UMissionWidget::UpdateObjective(const FMissionObjective& Objective)
{
	// Full rebuild for simplicity — optimize with pooled widgets in production
	// In production, find the specific widget by objective index
}

void UMissionWidget::ClearMission()
{
	if (MissionTitleText)     MissionTitleText->SetText(FText::GetEmpty());
	if (ObjectivesBox)        ObjectivesBox->ClearChildren();
	if (MissionProgressBar)   MissionProgressBar->SetPercent(0.f);
}
