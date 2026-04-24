#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EmergencyAlertWidget.generated.h"

class UTextBlock;
class UImage;
class UWidgetAnimation;

UCLASS()
class SUPERMANLOIS_API UEmergencyAlertWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget)) UTextBlock* AlertTitleText;
	UPROPERTY(meta=(BindWidget)) UTextBlock* AlertBodyText;
	UPROPERTY(meta=(BindWidget)) UImage*     AlertIcon;
	UPROPERTY(meta=(BindWidget)) UTextBlock* DistanceText;

	UPROPERTY(Transient, meta=(BindWidgetAnim)) UWidgetAnimation* AlertSlideIn;
	UPROPERTY(Transient, meta=(BindWidgetAnim)) UWidgetAnimation* AlertPulse;

	UFUNCTION(BlueprintCallable)
	void SetAlertData(const FText& Message, const FVector& EmergencyLocation);

	UFUNCTION(BlueprintCallable)
	void PlayAlertAnimation();

	UFUNCTION(BlueprintCallable)
	void UpdateDistance(FVector PlayerLocation, FVector EmergencyLocation);

private:
	FVector StoredEmergencyLocation = FVector::ZeroVector;
	FTimerHandle AutoHideTimer;
};
