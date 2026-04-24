#include "SupermanLoisGameMode.h"
#include "../Systems/MissionSystem.h"
#include "../Systems/EmergencySystem.h"
#include "../Systems/CityEventSystem.h"
#include "../Systems/WeatherSystem.h"
#include "../Systems/DayNightSystem.h"
#include "../Systems/ReputationSystem.h"
#include "SupermanLoisPlayerController.h"
#include "SupermanLoisGameState.h"

ASupermanLoisGameMode::ASupermanLoisGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	DefaultPawnClass = nullptr; // set in blueprint subclass
}

void ASupermanLoisGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	InitializeSystems();
}

void ASupermanLoisGameMode::BeginPlay()
{
	Super::BeginPlay();
	SetGamePhase(EGamePhase::Gameplay);
}

void ASupermanLoisGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateCityThreatLevel(DeltaTime);
}

void ASupermanLoisGameMode::InitializeSystems()
{
	MissionSystem    = NewObject<UMissionSystem>(this);
	EmergencySystem  = NewObject<UEmergencySystem>(this);
	CityEventSystem  = NewObject<UCityEventSystem>(this);
	WeatherSystem    = NewObject<UWeatherSystem>(this);
	DayNightSystem   = NewObject<UDayNightSystem>(this);
	ReputationSystem = NewObject<UReputationSystem>(this);

	MissionSystem->Initialize(this);
	EmergencySystem->Initialize(this);
	CityEventSystem->Initialize(this);
	WeatherSystem->Initialize(this);
	DayNightSystem->Initialize(this);
	ReputationSystem->Initialize(this);
}

void ASupermanLoisGameMode::SetGamePhase(EGamePhase NewPhase)
{
	CurrentPhase = NewPhase;
	OnGamePhaseChanged.Broadcast(NewPhase);
}

void ASupermanLoisGameMode::UpdateCityThreatLevel(float DeltaTime)
{
	const float TargetThreat = bIsMetropolisUnderAttack
		? FMath::Clamp(ActiveEmergencyCount * 0.1f, 0.f, 1.f)
		: FMath::Max(0.f, CityThreatLevel - DeltaTime * 0.05f);

	CityThreatLevel = FMath::FInterpTo(CityThreatLevel, TargetThreat, DeltaTime, 1.5f);

	if (CityThreatLevel > 0.5f && !bIsMetropolisUnderAttack)
	{
		bIsMetropolisUnderAttack = true;
		OnCityAttackBegin.Broadcast();
	}
}
