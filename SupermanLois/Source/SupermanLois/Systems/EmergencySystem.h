#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "EmergencySystem.generated.h"

class ASupermanLoisGameMode;
class AEmergencyEvent;
class APoliceOfficerAI;
class AFirefighterAI;
class AParamedicAI;

UENUM(BlueprintType)
enum class EEmergencyType : uint8
{
	// Standard city emergencies
	BuildingFire,
	MultiVehicleCrash,
	BridgeCollapse,
	PlaneDown,
	GasPipelineExplosion,
	FloodingZone,
	HostageSituation,
	BankRobbery,
	DomesticDisturbance,
	MassShootingThreat,

	// Supernatural / villain emergencies
	VillainAttack,
	AlienInvasion,
	MetaHumanRampage,
	XKryptiteContamination,
	KryptonianTerrorEvent,

	// Smallville specific
	FarmAccident,
	TornadoWarning,
	HighSchoolIncident
};

UENUM(BlueprintType)
enum class EEmergencyPriority : uint8
{
	Low    = 0,
	Medium = 1,
	High   = 2,
	Critical = 3
};

UENUM(BlueprintType)
enum class EEmergencyStatus : uint8
{
	Pending,        // just spawned, responders en route
	Active,         // ongoing
	SupermanOnScene, // player arrived
	Resolved,       // completed
	Failed          // civilians lost
};

USTRUCT(BlueprintType)
struct FEmergencyData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) EEmergencyType     Type         = EEmergencyType::BuildingFire;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) EEmergencyPriority Priority     = EEmergencyPriority::Medium;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) FText              AlertMessage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) float              TimeLimit     = 120.f; // seconds to resolve
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) int32              CiviliansAtRisk = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) bool               RequiresSuperman = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) int32              PoliceUnits   = 2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) int32              FireUnits     = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) int32              AMBUnits      = 0;
};

USTRUCT(BlueprintType)
struct FActiveEmergency
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) FGuid              ID;
	UPROPERTY(BlueprintReadWrite) FEmergencyData     Data;
	UPROPERTY(BlueprintReadWrite) FVector            Location     = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite) EEmergencyStatus   Status       = EEmergencyStatus::Pending;
	UPROPERTY(BlueprintReadWrite) float              TimeRemaining = 0.f;
	UPROPERTY(BlueprintReadWrite) int32              CiviliansRescued = 0;
	UPROPERTY(BlueprintReadWrite) int32              ResponderCount   = 0;
};

UCLASS()
class SUPERMANLOIS_API UEmergencySystem : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(ASupermanLoisGameMode* GameMode);

	UFUNCTION(BlueprintCallable, Category="Emergency")
	void TriggerEmergency(EEmergencyType Type, FVector Location);

	UFUNCTION(BlueprintCallable, Category="Emergency")
	void ResolveEmergency(FGuid EmergencyID, bool bSucceeded);

	UFUNCTION(BlueprintCallable, Category="Emergency")
	FActiveEmergency GetNearestEmergency(FVector PlayerLocation) const;

	UFUNCTION(BlueprintPure, Category="Emergency")
	int32 GetActiveEmergencyCount() const { return ActiveEmergencies.Num(); }

	UFUNCTION(BlueprintPure, Category="Emergency")
	TArray<FActiveEmergency> GetAllActiveEmergencies() const { return ActiveEmergencies; }

	void Tick(float DeltaTime);

	// Delegates
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnEmergencyTriggered, const FActiveEmergency&);
	FOnEmergencyTriggered OnEmergencyTriggered;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEmergencyResolved, FGuid, bool);
	FOnEmergencyResolved OnEmergencyResolved;

private:
	UPROPERTY()
	ASupermanLoisGameMode* OwningGameMode;

	TArray<FActiveEmergency> ActiveEmergencies;

	float RandomEmergencyTimer  = 0.f;
	float MinEmergencyInterval  = 60.f;   // seconds between random emergencies
	float MaxEmergencyInterval  = 180.f;

	void SpawnResponders(const FActiveEmergency& Emergency);
	void UpdateEmergencyTimers(float DeltaTime);
	void SpawnRandomEmergency();
	FVector GetRandomMetropolisLocation() const;
	FVector GetRandomSmallvilleLocation() const;
};
