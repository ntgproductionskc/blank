#include "SupermanHUD.h"
#include "../Character/SupermanCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/CanvasPanel.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/WidgetAnimation.h"

void USupermanHUD::NativeConstruct()
{
	Super::NativeConstruct();
	AutoBindToSuperman();
}

void USupermanHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!OwnerSuperman) { AutoBindToSuperman(); return; }

	// Auto-update every tick from the Superman character
	UpdateStats(
		OwnerSuperman->GetSolarEnergyPercent(),
		OwnerSuperman->GetHealthPercent(),
		OwnerSuperman->Stats.KryptoniteExposure / 100.f);

	UpdateFlight(
		OwnerSuperman->GetCurrentFlightSpeed(),
		OwnerSuperman->GetActorLocation().Z / 100.f, // cm to meters
		OwnerSuperman->bIsFlying || OwnerSuperman->bIsHovering,
		OwnerSuperman->bIsBoostFlying);
}

void USupermanHUD::AutoBindToSuperman()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		OwnerSuperman = Cast<ASupermanCharacter>(PC->GetPawn());

		if (OwnerSuperman)
		{
			OwnerSuperman->OnPowerActivated.AddDynamic(this, [](EActivePower P, bool bA)
			{
				// Handled via NativeTick polling — or wire directly if needed
			});

			OwnerSuperman->OnKryptoniteExposure.AddDynamic(this, [this](float Exposure)
			{
				if (Exposure > 20.f)
					ShowKryptoniteWarning(Exposure / 100.f);
				else
					HideKryptoniteWarning();
			});

			OwnerSuperman->OnSonicBoomTriggered.AddDynamic(this, &USupermanHUD::TriggerSonicBoomFlash);
		}
	}
}

void USupermanHUD::UpdateStats(float SolarPct, float HealthPct, float KryptonitePct)
{
	if (SolarEnergyBar)
	{
		SolarEnergyBar->SetPercent(SolarPct);

		// Color shifts from gold → red as energy depletes
		FLinearColor BarColor;
		BarColor = FLinearColor::LerpUsingHSV(
			FLinearColor(1.f, 0.1f, 0.f),   // red (low)
			FLinearColor(1.f, 0.85f, 0.f),  // gold (full)
			SolarPct);
		SolarEnergyBar->SetFillColorAndOpacity(BarColor);
	}

	if (HealthBar)
	{
		// Only show health bar when kryptonite is active
		const bool bShowHealth = KryptonitePct > 0.2f;
		HealthBar->SetVisibility(bShowHealth ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		HealthBar->SetPercent(HealthPct);

		FLinearColor HColor = FLinearColor::LerpUsingHSV(
			FLinearColor(1.f, 0.f, 0.f),
			FLinearColor(0.f, 1.f, 0.f),
			HealthPct);
		HealthBar->SetFillColorAndOpacity(HColor);
	}
}

void USupermanHUD::UpdateFlight(float SpeedKPH, float AltitudeM, bool bIsFlying, bool bIsBoosting)
{
	if (FlightSpeedOverlay)
		FlightSpeedOverlay->SetVisibility(bIsFlying ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

	if (SpeedText)
	{
		const FString SpeedStr = FString::Printf(TEXT("%d km/h"), FMath::RoundToInt(SpeedKPH));
		SpeedText->SetText(FText::FromString(SpeedStr));

		// Color goes white → orange → red as speed increases toward Mach
		FLinearColor SColor;
		const float SpeedNorm = FMath::Clamp(SpeedKPH / 1200.f, 0.f, 1.f);
		SColor = FLinearColor::LerpUsingHSV(
			FLinearColor::White,
			FLinearColor(1.f, 0.2f, 0.f),
			SpeedNorm);
		SpeedText->SetColorAndOpacity(SColor);
	}

	if (AltitudeText)
	{
		FString AltStr;
		if (AltitudeM >= 1000.f)
			AltStr = FString::Printf(TEXT("%.1f km"), AltitudeM / 1000.f);
		else
			AltStr = FString::Printf(TEXT("%d m"), FMath::RoundToInt(AltitudeM));
		AltitudeText->SetText(FText::FromString(AltStr));
	}
}

void USupermanHUD::UpdateActivePower(const FText& PowerName, bool bActive)
{
	if (PowerNameText)
	{
		PowerNameText->SetText(bActive ? PowerName : FText::GetEmpty());
		PowerNameText->SetVisibility(bActive ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void USupermanHUD::UpdateCombo(int32 Count)
{
	if (!ComboText) return;

	if (Count > 1)
	{
		const FString ComboStr = FString::Printf(TEXT("x%d COMBO"), Count);
		ComboText->SetText(FText::FromString(ComboStr));
		ComboText->SetVisibility(ESlateVisibility::Visible);

		if (ComboCounterPop) PlayAnimation(ComboCounterPop);
	}
	else
	{
		ComboText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void USupermanHUD::UpdateReputation(const FText& TierName, float Progress)
{
	if (ReputationText)
		ReputationText->SetText(TierName);
}

void USupermanHUD::ShowKryptoniteWarning(float Intensity)
{
	if (!bKryptoniteWarningActive)
	{
		bKryptoniteWarningActive = true;
		if (KryptoniteOverlay)
			KryptoniteOverlay->SetVisibility(ESlateVisibility::Visible);
		if (KryptoniteFlash) PlayAnimation(KryptoniteFlash, 0.f, 0, EUMGSequencePlayMode::PingPong);
	}

	// Drive opacity by exposure intensity
	if (KryptoniteOverlay)
		KryptoniteOverlay->SetRenderOpacity(Intensity * 0.4f);
}

void USupermanHUD::HideKryptoniteWarning()
{
	bKryptoniteWarningActive = false;
	if (KryptoniteOverlay)
		KryptoniteOverlay->SetVisibility(ESlateVisibility::Hidden);
	if (KryptoniteFlash) StopAnimation(KryptoniteFlash);
}

void USupermanHUD::ShowXRayVisionOverlay(bool bVisible)
{
	if (XRayVisionOverlay)
		XRayVisionOverlay->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void USupermanHUD::ShowSuperHearingOverlay(bool bVisible)
{
	if (SuperHearingOverlay)
		SuperHearingOverlay->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void USupermanHUD::TriggerSonicBoomFlash()
{
	if (SonicBoomFlash)
		SonicBoomFlash->SetVisibility(ESlateVisibility::Visible);
	if (SonicBoomFlashAnim) PlayAnimation(SonicBoomFlashAnim);
}

void USupermanHUD::UpdateTimeDisplay(float Hour)
{
	if (TimeOfDayText)
		TimeOfDayText->SetText(FText::FromString(FormatTime(Hour)));
}

FString USupermanHUD::FormatTime(float Hour) const
{
	const int32 H   = FMath::FloorToInt(Hour) % 24;
	const int32 Min = FMath::FloorToInt((Hour - FMath::FloorToInt(Hour)) * 60.f);
	const bool  bPM = H >= 12;
	const int32 H12 = (H == 0 || H == 12) ? 12 : H % 12;
	return FString::Printf(TEXT("%d:%02d %s"), H12, Min, bPM ? TEXT("PM") : TEXT("AM"));
}
