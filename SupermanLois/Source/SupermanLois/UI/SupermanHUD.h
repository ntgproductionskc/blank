#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SupermanHUD.generated.h"

class ASupermanCharacter;
class UProgressBar;
class UTextBlock;
class UImage;
class UOverlay;
class UCanvasPanel;
class UWidgetAnimation;

// The full in-game HUD:
// - Solar energy bar (top left)
// - Health bar (only shows when Kryptonite is active)
// - Active power indicator
// - Speed readout (km/h) while flying
// - Altitude readout
// - Minimap (bottom left)
// - Active mission / objective tracker (top right)
// - Emergency alert banner
// - Reputation tier indicator
// - Combo counter (during combat)
// - Kryptonite warning overlay (green tint)
// - X-Ray vision overlay (blue skeleton scan)
// - Super Hearing overlay (sound wave visualizer)

UCLASS()
class SUPERMANLOIS_API USupermanHUD : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	// ── Bound widgets (set in UMG Designer) ────────────────────────────────────
	UPROPERTY(meta=(BindWidget)) UProgressBar* SolarEnergyBar;
	UPROPERTY(meta=(BindWidget)) UProgressBar* HealthBar;
	UPROPERTY(meta=(BindWidget)) UTextBlock*   SpeedText;
	UPROPERTY(meta=(BindWidget)) UTextBlock*   AltitudeText;
	UPROPERTY(meta=(BindWidget)) UTextBlock*   PowerNameText;
	UPROPERTY(meta=(BindWidget)) UTextBlock*   ComboText;
	UPROPERTY(meta=(BindWidget)) UImage*       PowerIcon;
	UPROPERTY(meta=(BindWidget)) UOverlay*     KryptoniteOverlay;    // green tint overlay
	UPROPERTY(meta=(BindWidget)) UOverlay*     XRayVisionOverlay;    // blue scan overlay
	UPROPERTY(meta=(BindWidget)) UOverlay*     SuperHearingOverlay;  // audio waveform overlay
	UPROPERTY(meta=(BindWidget)) UCanvasPanel* MinimapPanel;
	UPROPERTY(meta=(BindWidget)) UOverlay*     FlightSpeedOverlay;   // shows only when flying
	UPROPERTY(meta=(BindWidget)) UTextBlock*   ReputationText;
	UPROPERTY(meta=(BindWidget)) UTextBlock*   TimeOfDayText;
	UPROPERTY(meta=(BindWidget)) UOverlay*     SonicBoomFlash;       // brief flash on sonic boom
	UPROPERTY(meta=(BindWidget)) UImage*       CompassArrow;

	// ── Animations (defined in UMG) ───────────────────────────────────────────
	UPROPERTY(Transient, meta=(BindWidgetAnim)) UWidgetAnimation* KryptoniteFlash;
	UPROPERTY(Transient, meta=(BindWidgetAnim)) UWidgetAnimation* ComboCounterPop;
	UPROPERTY(Transient, meta=(BindWidgetAnim)) UWidgetAnimation* SonicBoomFlashAnim;
	UPROPERTY(Transient, meta=(BindWidgetAnim)) UWidgetAnimation* EmergencyPulse;

	// ── Public update calls ────────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category="HUD")
	void UpdateStats(float SolarPct, float HealthPct, float KryptonitePct);

	UFUNCTION(BlueprintCallable, Category="HUD")
	void UpdateFlight(float SpeedKPH, float AltitudeM, bool bIsFlying, bool bIsBoosting);

	UFUNCTION(BlueprintCallable, Category="HUD")
	void UpdateActivePower(const FText& PowerName, bool bActive);

	UFUNCTION(BlueprintCallable, Category="HUD")
	void UpdateCombo(int32 Count);

	UFUNCTION(BlueprintCallable, Category="HUD")
	void UpdateReputation(const FText& TierName, float Progress);

	UFUNCTION(BlueprintCallable, Category="HUD")
	void ShowKryptoniteWarning(float Intensity);

	UFUNCTION(BlueprintCallable, Category="HUD")
	void HideKryptoniteWarning();

	UFUNCTION(BlueprintCallable, Category="HUD")
	void ShowXRayVisionOverlay(bool bVisible);

	UFUNCTION(BlueprintCallable, Category="HUD")
	void ShowSuperHearingOverlay(bool bVisible);

	UFUNCTION(BlueprintCallable, Category="HUD")
	void TriggerSonicBoomFlash();

	UFUNCTION(BlueprintCallable, Category="HUD")
	void UpdateTimeDisplay(float Hour);

private:
	UPROPERTY()
	ASupermanCharacter* OwnerSuperman;

	bool bKryptoniteWarningActive = false;
	float KryptoniteFlashTimer    = 0.f;

	void AutoBindToSuperman();
	FString FormatTime(float Hour) const;
};
