#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "SupermanCharacter.generated.h"

class USupermanMovementComponent;
class USupermanPowerComponent;
class UCameraComponent;
class USpringArmComponent;
class UAbilitySystemComponent;
class UAttributeSet;
class UNiagaraComponent;
class UAudioComponent;
class UCapePhysicsComponent;
class USupermanAnimInstance;

UENUM(BlueprintType)
enum class ESupermanState : uint8
{
	Grounded,
	Jumping,
	Flying,
	FlyingBoost,    // sonic boom speed
	Hovering,
	InCombat,
	UsingPower,
	Grabbing,
	Stunned,
	Cutscene
};

UENUM(BlueprintType)
enum class EActivePower : uint8
{
	None,
	HeatVision,
	FreezeBreath,
	SuperSpeed,
	XRayVision,
	SuperHearing
};

USTRUCT(BlueprintType)
struct FSupermanStats
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) float Health           = 1000.f;
	UPROPERTY(BlueprintReadWrite) float MaxHealth        = 1000.f;
	UPROPERTY(BlueprintReadWrite) float SolarEnergy      = 100.f;  // powers deplete this
	UPROPERTY(BlueprintReadWrite) float MaxSolarEnergy   = 100.f;
	UPROPERTY(BlueprintReadWrite) float KryptoniteExposure = 0.f;  // 0–100 weakens powers
	UPROPERTY(BlueprintReadWrite) float FlightSpeed      = 2000.f; // cm/s
	UPROPERTY(BlueprintReadWrite) float BoostMultiplier  = 8.f;    // sonic boom factor
	UPROPERTY(BlueprintReadWrite) float PhysicalStrength = 1.f;    // damage multiplier
};

UCLASS()
class SUPERMANLOIS_API ASupermanCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ASupermanCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

public:
	// ── IAbilitySystemInterface ────────────────────────────────────────────────
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// ── Components ─────────────────────────────────────────────────────────────
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Powers")
	USupermanPowerComponent* PowerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Abilities")
	UAbilitySystemComponent* AbilitySystem;

	// Heat Vision beams
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VFX")
	UNiagaraComponent* HeatVisionLeftEye;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VFX")
	UNiagaraComponent* HeatVisionRightEye;

	// Freeze breath cone
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VFX")
	UNiagaraComponent* FreezeBreathNiagara;

	// Flight trail / sonic boom
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VFX")
	UNiagaraComponent* FlightTrailNiagara;

	// Cape physics cloth simulation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cloth")
	USkeletalMeshComponent* CapeMesh;

	// ── Stats ──────────────────────────────────────────────────────────────────
	UPROPERTY(BlueprintReadWrite, Category="Stats")
	FSupermanStats Stats;

	// ── State ──────────────────────────────────────────────────────────────────
	UPROPERTY(BlueprintReadOnly, Category="State")
	ESupermanState SupermanState = ESupermanState::Grounded;

	UPROPERTY(BlueprintReadOnly, Category="State")
	EActivePower ActivePower = EActivePower::None;

	UPROPERTY(BlueprintReadOnly, Category="State")
	bool bIsFlying = false;

	UPROPERTY(BlueprintReadOnly, Category="State")
	bool bIsHovering = false;

	UPROPERTY(BlueprintReadOnly, Category="State")
	bool bIsBoostFlying = false;

	UPROPERTY(BlueprintReadOnly, Category="State")
	bool bIsXRayVisionActive = false;

	UPROPERTY(BlueprintReadOnly, Category="State")
	bool bIsSuperHearingActive = false;

	UPROPERTY(BlueprintReadOnly, Category="State")
	bool bIsSuperSpeedActive = false;

	// ── Grabbed object ─────────────────────────────────────────────────────────
	UPROPERTY(BlueprintReadOnly, Category="Interaction")
	AActor* GrabbedActor = nullptr;

	// ── Movement interface (called by PlayerController) ────────────────────────
	UFUNCTION(BlueprintCallable, Category="Movement")
	void MoveInput(FVector2D AxisValue);

	UFUNCTION(BlueprintCallable, Category="Movement")
	void SuperJump();

	UFUNCTION(BlueprintCallable, Category="Movement")
	void StartFlying();

	UFUNCTION(BlueprintCallable, Category="Movement")
	void HoverInPlace();

	UFUNCTION(BlueprintCallable, Category="Movement")
	void Land();

	UFUNCTION(BlueprintCallable, Category="Movement")
	void ActivateFlightBoost(bool bActive);

	// ── Powers interface ───────────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category="Powers")
	void StartHeatVision();

	UFUNCTION(BlueprintCallable, Category="Powers")
	void StopHeatVision();

	UFUNCTION(BlueprintCallable, Category="Powers")
	void StartFreezeBreath();

	UFUNCTION(BlueprintCallable, Category="Powers")
	void StopFreezeBreath();

	UFUNCTION(BlueprintCallable, Category="Powers")
	void ToggleSuperSpeed();

	UFUNCTION(BlueprintCallable, Category="Powers")
	void ToggleXRayVision();

	UFUNCTION(BlueprintCallable, Category="Powers")
	void ToggleSuperHearing();

	// ── Combat interface ───────────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category="Combat")
	void LightAttack();

	UFUNCTION(BlueprintCallable, Category="Combat")
	void HeavyAttack();

	UFUNCTION(BlueprintCallable, Category="Combat")
	void Dodge();

	UFUNCTION(BlueprintCallable, Category="Combat")
	void AttemptGrab();

	UFUNCTION(BlueprintCallable, Category="Combat")
	void ThrowGrabbedObject();

	// ── Events ─────────────────────────────────────────────────────────────────
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPowerActivated, EActivePower, Power, bool, bActive);
	UPROPERTY(BlueprintAssignable) FOnPowerActivated OnPowerActivated;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateChanged, ESupermanState, NewState);
	UPROPERTY(BlueprintAssignable) FOnStateChanged OnStateChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKryptoniteExposure, float, ExposureLevel);
	UPROPERTY(BlueprintAssignable) FOnKryptoniteExposure OnKryptoniteExposure;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSonicBoomTriggered);
	UPROPERTY(BlueprintAssignable) FOnSonicBoomTriggered OnSonicBoomTriggered;

	// ── Blueprint helpers ─────────────────────────────────────────────────────
	UFUNCTION(BlueprintPure, Category="Stats")
	float GetHealthPercent() const;

	UFUNCTION(BlueprintPure, Category="Stats")
	float GetSolarEnergyPercent() const;

	UFUNCTION(BlueprintPure, Category="Stats")
	bool IsWeakenedByKryptonite() const { return Stats.KryptoniteExposure > 40.f; }

	UFUNCTION(BlueprintPure, Category="Movement")
	float GetCurrentFlightSpeed() const;

	UFUNCTION(BlueprintCallable, Category="Stats")
	void AddKryptoniteExposure(float Amount);

	UFUNCTION(BlueprintCallable, Category="Stats")
	void RemoveKryptoniteExposure(float Amount);

private:
	void SetSupermanState(ESupermanState NewState);
	void UpdateFlightPhysics(float DeltaTime);
	void UpdateHeatVision(float DeltaTime);
	void UpdateFreezeBreath(float DeltaTime);
	void UpdateSolarEnergyRegen(float DeltaTime);
	void UpdateCapePhysics(float DeltaTime);
	void TriggerSonicBoom();
	void ApplyLandingImpact();

	bool bHeatVisionActive    = false;
	bool bFreezeBreathActive  = false;
	float HeatVisionTimer     = 0.f;
	float FreezeBreathTimer   = 0.f;
	float SonicBoomCooldown   = 0.f;
	float PreviousSpeed       = 0.f;

	// Flight interpolation
	FVector  TargetFlightVelocity   = FVector::ZeroVector;
	FRotator TargetFlightRotation   = FRotator::ZeroRotator;
	float    CurrentBoostBlend      = 0.f;
};
