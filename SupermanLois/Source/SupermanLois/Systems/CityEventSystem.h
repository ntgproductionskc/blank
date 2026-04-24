#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CityEventSystem.generated.h"

class ASupermanLoisGameMode;
class ABaseVillain;

UENUM(BlueprintType)
enum class ECityEvent : uint8
{
	// Ambient life
	TrafficAccident,
	StreetCrime,
	ConstructionExplosion,
	JailBreak,
	CarChase,

	// Major events
	VillainSiegeStart,
	VillainSiegeEnd,
	MilitaryCordon,        // DOD cordon around hot zone
	PressBriefing,         // Lois at the mic
	KentFarmCall,          // Clark gets called home

	// Supernatural
	PortalOpening,
	XKryptoniteContamination,
	AlienShipSighting
};

UCLASS()
class SUPERMANLOIS_API UCityEventSystem : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(ASupermanLoisGameMode* GameMode);
	void Tick(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category="Events")
	void TriggerEvent(ECityEvent Event, FVector Location);

	UFUNCTION(BlueprintCallable, Category="Events")
	void SpawnVillainSiege(TSubclassOf<ABaseVillain> VillainClass, FVector SiegeCenter, int32 WaveCount);

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCityEventTriggered, ECityEvent, FVector);
	FOnCityEventTriggered OnCityEventTriggered;

private:
	UPROPERTY()
	ASupermanLoisGameMode* OwningGameMode;

	float AmbientEventTimer  = 0.f;
	int32 ActiveSiegeWave    = 0;
	int32 MaxSiegeWaves      = 0;
	FVector SiegeCenter      = FVector::ZeroVector;
	TSubclassOf<ABaseVillain> SiegeVillainClass;

	void SpawnAmbientEvent();
	void ContinueSiegeWave();
};
