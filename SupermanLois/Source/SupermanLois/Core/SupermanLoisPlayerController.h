#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "SupermanLoisPlayerController.generated.h"

class ASupermanCharacter;
class UInputMappingContext;
class UInputAction;
class USupermanHUD;
class UMissionWidget;
class UEmergencyAlertWidget;

UCLASS()
class SUPERMANLOIS_API ASupermanLoisPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASupermanLoisPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

public:
	// ── Enhanced Input contexts ────────────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputMappingContext* FlightMappingContext;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputMappingContext* CombatMappingContext;

	// ── Actions ────────────────────────────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, Category="Input|Movement")
	UInputAction* IA_Move;

	UPROPERTY(EditDefaultsOnly, Category="Input|Movement")
	UInputAction* IA_Look;

	UPROPERTY(EditDefaultsOnly, Category="Input|Movement")
	UInputAction* IA_Jump;

	UPROPERTY(EditDefaultsOnly, Category="Input|Movement")
	UInputAction* IA_Fly;

	UPROPERTY(EditDefaultsOnly, Category="Input|Movement")
	UInputAction* IA_FlyBoost;

	UPROPERTY(EditDefaultsOnly, Category="Input|Movement")
	UInputAction* IA_Land;

	UPROPERTY(EditDefaultsOnly, Category="Input|Powers")
	UInputAction* IA_HeatVision;

	UPROPERTY(EditDefaultsOnly, Category="Input|Powers")
	UInputAction* IA_FreezeBreath;

	UPROPERTY(EditDefaultsOnly, Category="Input|Powers")
	UInputAction* IA_SuperSpeed;

	UPROPERTY(EditDefaultsOnly, Category="Input|Powers")
	UInputAction* IA_XRayVision;

	UPROPERTY(EditDefaultsOnly, Category="Input|Powers")
	UInputAction* IA_SuperHearing;

	UPROPERTY(EditDefaultsOnly, Category="Input|Powers")
	UInputAction* IA_Grab;

	UPROPERTY(EditDefaultsOnly, Category="Input|Powers")
	UInputAction* IA_Throw;

	UPROPERTY(EditDefaultsOnly, Category="Input|Combat")
	UInputAction* IA_LightAttack;

	UPROPERTY(EditDefaultsOnly, Category="Input|Combat")
	UInputAction* IA_HeavyAttack;

	UPROPERTY(EditDefaultsOnly, Category="Input|Combat")
	UInputAction* IA_Dodge;

	UPROPERTY(EditDefaultsOnly, Category="Input|UI")
	UInputAction* IA_OpenMap;

	UPROPERTY(EditDefaultsOnly, Category="Input|UI")
	UInputAction* IA_OpenMissions;

	UPROPERTY(EditDefaultsOnly, Category="Input|UI")
	UInputAction* IA_Pause;

	// ── UI widgets ─────────────────────────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<USupermanHUD> HUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UMissionWidget> MissionWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UEmergencyAlertWidget> AlertWidgetClass;

	UFUNCTION(BlueprintCallable, Category="UI")
	void ShowEmergencyAlert(const FText& AlertMessage, const FVector& Location);

	UFUNCTION(BlueprintCallable, Category="UI")
	void HideEmergencyAlert();

private:
	UPROPERTY()
	ASupermanCharacter* ControlledSuperman;

	UPROPERTY()
	USupermanHUD* HUDWidget;

	UPROPERTY()
	UEmergencyAlertWidget* AlertWidget;

	// Input handlers
	void HandleMove(const FInputActionValue& Value);
	void HandleLook(const FInputActionValue& Value);
	void HandleJump(const FInputActionValue& Value);
	void HandleFlyStart(const FInputActionValue& Value);
	void HandleFlyEnd(const FInputActionValue& Value);
	void HandleFlyBoostStart(const FInputActionValue& Value);
	void HandleFlyBoostEnd(const FInputActionValue& Value);
	void HandleLand(const FInputActionValue& Value);
	void HandleHeatVisionStart(const FInputActionValue& Value);
	void HandleHeatVisionEnd(const FInputActionValue& Value);
	void HandleFreezeBreathStart(const FInputActionValue& Value);
	void HandleFreezeBreathEnd(const FInputActionValue& Value);
	void HandleSuperSpeedToggle(const FInputActionValue& Value);
	void HandleXRayVisionToggle(const FInputActionValue& Value);
	void HandleSuperHearingToggle(const FInputActionValue& Value);
	void HandleGrab(const FInputActionValue& Value);
	void HandleThrow(const FInputActionValue& Value);
	void HandleLightAttack(const FInputActionValue& Value);
	void HandleHeavyAttack(const FInputActionValue& Value);
	void HandleDodge(const FInputActionValue& Value);
	void HandleOpenMap(const FInputActionValue& Value);
	void HandleOpenMissions(const FInputActionValue& Value);
	void HandlePause(const FInputActionValue& Value);
};
