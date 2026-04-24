#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SupermanLoisGameMode.generated.h"

class UMissionSystem;
class UEmergencySystem;
class UCityEventSystem;
class UWeatherSystem;
class UDayNightSystem;
class UReputationSystem;

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	MainMenu,
	Loading,
	Gameplay,
	Cutscene,
	MissionBriefing,
	GameOver,
	Credits
};

UCLASS()
class SUPERMANLOIS_API ASupermanLoisGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASupermanLoisGameMode();

protected:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// ── Game phase ─────────────────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category="Game")
	void SetGamePhase(EGamePhase NewPhase);

	UFUNCTION(BlueprintPure, Category="Game")
	EGamePhase GetCurrentPhase() const { return CurrentPhase; }

	// ── Systems ─────────────────────────────────────────────────────────────────
	UFUNCTION(BlueprintPure, Category="Systems")
	UMissionSystem* GetMissionSystem() const { return MissionSystem; }

	UFUNCTION(BlueprintPure, Category="Systems")
	UEmergencySystem* GetEmergencySystem() const { return EmergencySystem; }

	UFUNCTION(BlueprintPure, Category="Systems")
	UCityEventSystem* GetCityEventSystem() const { return CityEventSystem; }

	UFUNCTION(BlueprintPure, Category="Systems")
	UWeatherSystem* GetWeatherSystem() const { return WeatherSystem; }

	UFUNCTION(BlueprintPure, Category="Systems")
	UDayNightSystem* GetDayNightSystem() const { return DayNightSystem; }

	UFUNCTION(BlueprintPure, Category="Systems")
	UReputationSystem* GetReputationSystem() const { return ReputationSystem; }

	// ── World state ─────────────────────────────────────────────────────────────
	UPROPERTY(BlueprintReadWrite, Category="World")
	bool bIsMetropolisUnderAttack = false;

	UPROPERTY(BlueprintReadWrite, Category="World")
	int32 ActiveEmergencyCount = 0;

	UPROPERTY(BlueprintReadWrite, Category="World")
	float CityThreatLevel = 0.f;   // 0–1, drives ambient NPC panic

	// ── Delegates ───────────────────────────────────────────────────────────────
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGamePhaseChanged, EGamePhase, NewPhase);
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnGamePhaseChanged OnGamePhaseChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCityAttackBegin);
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnCityAttackBegin OnCityAttackBegin;

private:
	EGamePhase CurrentPhase = EGamePhase::Loading;

	UPROPERTY()
	UMissionSystem* MissionSystem;

	UPROPERTY()
	UEmergencySystem* EmergencySystem;

	UPROPERTY()
	UCityEventSystem* CityEventSystem;

	UPROPERTY()
	UWeatherSystem* WeatherSystem;

	UPROPERTY()
	UDayNightSystem* DayNightSystem;

	UPROPERTY()
	UReputationSystem* ReputationSystem;

	void InitializeSystems();
	void UpdateCityThreatLevel(float DeltaTime);
};
