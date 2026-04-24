#include "SupermanLoisPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "../Character/SupermanCharacter.h"
#include "../UI/SupermanHUD.h"
#include "../UI/EmergencyAlertWidget.h"
#include "../UI/MissionWidget.h"
#include "Blueprint/UserWidget.h"

ASupermanLoisPlayerController::ASupermanLoisPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASupermanLoisPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
		Subsystem->AddMappingContext(FlightMappingContext,  1);
		Subsystem->AddMappingContext(CombatMappingContext,  2);
	}

	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<USupermanHUD>(this, HUDWidgetClass);
		HUDWidget->AddToViewport();
	}

	if (AlertWidgetClass)
	{
		AlertWidget = CreateWidget<UEmergencyAlertWidget>(this, AlertWidgetClass);
		AlertWidget->AddToViewport();
		AlertWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ASupermanLoisPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ControlledSuperman = Cast<ASupermanCharacter>(InPawn);
}

void ASupermanLoisPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASupermanLoisPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(InputComponent);

	EIC->BindAction(IA_Move,           ETriggerEvent::Triggered, this, &ASupermanLoisPlayerController::HandleMove);
	EIC->BindAction(IA_Look,           ETriggerEvent::Triggered, this, &ASupermanLoisPlayerController::HandleLook);
	EIC->BindAction(IA_Jump,           ETriggerEvent::Started,   this, &ASupermanLoisPlayerController::HandleJump);
	EIC->BindAction(IA_Fly,            ETriggerEvent::Started,   this, &ASupermanLoisPlayerController::HandleFlyStart);
	EIC->BindAction(IA_Fly,            ETriggerEvent::Completed, this, &ASupermanLoisPlayerController::HandleFlyEnd);
	EIC->BindAction(IA_FlyBoost,       ETriggerEvent::Started,   this, &ASupermanLoisPlayerController::HandleFlyBoostStart);
	EIC->BindAction(IA_FlyBoost,       ETriggerEvent::Completed, this, &ASupermanLoisPlayerController::HandleFlyBoostEnd);
	EIC->BindAction(IA_Land,           ETriggerEvent::Started,   this, &ASupermanLoisPlayerController::HandleLand);
	EIC->BindAction(IA_HeatVision,     ETriggerEvent::Started,   this, &ASupermanLoisPlayerController::HandleHeatVisionStart);
	EIC->BindAction(IA_HeatVision,     ETriggerEvent::Completed, this, &ASupermanLoisPlayerController::HandleHeatVisionEnd);
	EIC->BindAction(IA_FreezeBreath,   ETriggerEvent::Started,   this, &ASupermanLoisPlayerController::HandleFreezeBreathStart);
	EIC->BindAction(IA_FreezeBreath,   ETriggerEvent::Completed, this, &ASupermanLoisPlayerController::HandleFreezeBreathEnd);
	EIC->BindAction(IA_SuperSpeed,     ETriggerEvent::Started,   this, &ASupermanLoisPlayerController::HandleSuperSpeedToggle);
	EIC->BindAction(IA_XRayVision,     ETriggerEvent::Started,   this, &ASupermanLoisPlayerController::HandleXRayVisionToggle);
	EIC->BindAction(IA_SuperHearing,   ETriggerEvent::Started,   this, &ASupermanLoisPlayerController::HandleSuperHearingToggle);
	EIC->BindAction(IA_Grab,           ETriggerEvent::Started,   this, &ASupermanLoisPlayerController::HandleGrab);
	EIC->BindAction(IA_Throw,          ETriggerEvent::Started,   this, &ASupermanLoisPlayerController::HandleThrow);
	EIC->BindAction(IA_LightAttack,    ETriggerEvent::Started,   this, &ASupermanLoisPlayerController::HandleLightAttack);
	EIC->BindAction(IA_HeavyAttack,    ETriggerEvent::Started,   this, &ASupermanLoisPlayerController::HandleHeavyAttack);
	EIC->BindAction(IA_Dodge,          ETriggerEvent::Started,   this, &ASupermanLoisPlayerController::HandleDodge);
	EIC->BindAction(IA_OpenMap,        ETriggerEvent::Started,   this, &ASupermanLoisPlayerController::HandleOpenMap);
	EIC->BindAction(IA_OpenMissions,   ETriggerEvent::Started,   this, &ASupermanLoisPlayerController::HandleOpenMissions);
	EIC->BindAction(IA_Pause,          ETriggerEvent::Started,   this, &ASupermanLoisPlayerController::HandlePause);
}

// ── Input handlers ─────────────────────────────────────────────────────────────

void ASupermanLoisPlayerController::HandleMove(const FInputActionValue& Value)
{
	if (!ControlledSuperman) return;
	ControlledSuperman->MoveInput(Value.Get<FVector2D>());
}

void ASupermanLoisPlayerController::HandleLook(const FInputActionValue& Value)
{
	const FVector2D LookDelta = Value.Get<FVector2D>();
	AddYawInput(LookDelta.X);
	AddPitchInput(-LookDelta.Y);
}

void ASupermanLoisPlayerController::HandleJump(const FInputActionValue& Value)
{
	if (ControlledSuperman) ControlledSuperman->SuperJump();
}

void ASupermanLoisPlayerController::HandleFlyStart(const FInputActionValue& Value)
{
	if (ControlledSuperman) ControlledSuperman->StartFlying();
}

void ASupermanLoisPlayerController::HandleFlyEnd(const FInputActionValue& Value)
{
	if (ControlledSuperman) ControlledSuperman->HoverInPlace();
}

void ASupermanLoisPlayerController::HandleFlyBoostStart(const FInputActionValue& Value)
{
	if (ControlledSuperman) ControlledSuperman->ActivateFlightBoost(true);
}

void ASupermanLoisPlayerController::HandleFlyBoostEnd(const FInputActionValue& Value)
{
	if (ControlledSuperman) ControlledSuperman->ActivateFlightBoost(false);
}

void ASupermanLoisPlayerController::HandleLand(const FInputActionValue& Value)
{
	if (ControlledSuperman) ControlledSuperman->Land();
}

void ASupermanLoisPlayerController::HandleHeatVisionStart(const FInputActionValue& Value)
{
	if (ControlledSuperman) ControlledSuperman->StartHeatVision();
}

void ASupermanLoisPlayerController::HandleHeatVisionEnd(const FInputActionValue& Value)
{
	if (ControlledSuperman) ControlledSuperman->StopHeatVision();
}

void ASupermanLoisPlayerController::HandleFreezeBreathStart(const FInputActionValue& Value)
{
	if (ControlledSuperman) ControlledSuperman->StartFreezeBreath();
}

void ASupermanLoisPlayerController::HandleFreezeBreathEnd(const FInputActionValue& Value)
{
	if (ControlledSuperman) ControlledSuperman->StopFreezeBreath();
}

void ASupermanLoisPlayerController::HandleSuperSpeedToggle(const FInputActionValue& Value)
{
	if (ControlledSuperman) ControlledSuperman->ToggleSuperSpeed();
}

void ASupermanLoisPlayerController::HandleXRayVisionToggle(const FInputActionValue& Value)
{
	if (ControlledSuperman) ControlledSuperman->ToggleXRayVision();
}

void ASupermanLoisPlayerController::HandleSuperHearingToggle(const FInputActionValue& Value)
{
	if (ControlledSuperman) ControlledSuperman->ToggleSuperHearing();
}

void ASupermanLoisPlayerController::HandleGrab(const FInputActionValue& Value)
{
	if (ControlledSuperman) ControlledSuperman->AttemptGrab();
}

void ASupermanLoisPlayerController::HandleThrow(const FInputActionValue& Value)
{
	if (ControlledSuperman) ControlledSuperman->ThrowGrabbedObject();
}

void ASupermanLoisPlayerController::HandleLightAttack(const FInputActionValue& Value)
{
	if (ControlledSuperman) ControlledSuperman->LightAttack();
}

void ASupermanLoisPlayerController::HandleHeavyAttack(const FInputActionValue& Value)
{
	if (ControlledSuperman) ControlledSuperman->HeavyAttack();
}

void ASupermanLoisPlayerController::HandleDodge(const FInputActionValue& Value)
{
	if (ControlledSuperman) ControlledSuperman->Dodge();
}

void ASupermanLoisPlayerController::HandleOpenMap(const FInputActionValue& Value)
{
	// Toggle world map widget
}

void ASupermanLoisPlayerController::HandleOpenMissions(const FInputActionValue& Value)
{
	// Toggle mission journal widget
}

void ASupermanLoisPlayerController::HandlePause(const FInputActionValue& Value)
{
	SetPause(!IsPaused());
}

void ASupermanLoisPlayerController::ShowEmergencyAlert(const FText& AlertMessage, const FVector& Location)
{
	if (AlertWidget)
	{
		AlertWidget->SetAlertData(AlertMessage, Location);
		AlertWidget->SetVisibility(ESlateVisibility::Visible);
		AlertWidget->PlayAlertAnimation();
	}
}

void ASupermanLoisPlayerController::HideEmergencyAlert()
{
	if (AlertWidget) AlertWidget->SetVisibility(ESlateVisibility::Hidden);
}
