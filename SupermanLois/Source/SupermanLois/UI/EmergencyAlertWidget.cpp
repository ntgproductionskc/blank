#include "EmergencyAlertWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Animation/WidgetAnimation.h"
#include "Kismet/GameplayStatics.h"

void UEmergencyAlertWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UEmergencyAlertWidget::SetAlertData(const FText& Message, const FVector& EmergencyLocation)
{
	StoredEmergencyLocation = EmergencyLocation;

	if (AlertBodyText)
		AlertBodyText->SetText(Message);

	if (AlertTitleText)
		AlertTitleText->SetText(FText::FromString(TEXT("EMERGENCY ALERT")));

	// Update distance immediately
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (APawn* Pawn = PC->GetPawn())
			UpdateDistance(Pawn->GetActorLocation(), EmergencyLocation);
	}
}

void UEmergencyAlertWidget::PlayAlertAnimation()
{
	if (AlertSlideIn) PlayAnimation(AlertSlideIn);

	// Auto-hide after 8 seconds
	GetWorld()->GetTimerManager().SetTimer(AutoHideTimer, [this]()
	{
		SetVisibility(ESlateVisibility::Hidden);
	}, 8.f, false);
}

void UEmergencyAlertWidget::UpdateDistance(FVector PlayerLocation, FVector EmergencyLocation)
{
	if (!DistanceText) return;

	const float DistCM = FVector::Dist(PlayerLocation, EmergencyLocation);
	const float DistKM = DistCM / 100000.f;

	FString DistStr;
	if (DistKM >= 1.f)
		DistStr = FString::Printf(TEXT("%.1f km away"), DistKM);
	else
		DistStr = FString::Printf(TEXT("%d m away"), FMath::RoundToInt(DistKM * 1000.f));

	DistanceText->SetText(FText::FromString(DistStr));
}
