#include "CityEventSystem.h"
#include "../Core/SupermanLoisGameMode.h"
#include "../Systems/EmergencySystem.h"
#include "Kismet/GameplayStatics.h"

void UCityEventSystem::Initialize(ASupermanLoisGameMode* GameMode)
{
	OwningGameMode    = GameMode;
	AmbientEventTimer = FMath::RandRange(20.f, 60.f);
}

void UCityEventSystem::Tick(float DeltaTime)
{
	AmbientEventTimer -= DeltaTime;
	if (AmbientEventTimer <= 0.f)
	{
		SpawnAmbientEvent();
		AmbientEventTimer = FMath::RandRange(30.f, 90.f);
	}

	if (ActiveSiegeWave > 0 && ActiveSiegeWave <= MaxSiegeWaves)
		ContinueSiegeWave();
}

void UCityEventSystem::TriggerEvent(ECityEvent Event, FVector Location)
{
	OnCityEventTriggered.Broadcast(Event, Location);

	if (!OwningGameMode) return;

	switch (Event)
	{
	case ECityEvent::VillainSiegeStart:
		OwningGameMode->bIsMetropolisUnderAttack = true;
		if (UEmergencySystem* ES = OwningGameMode->GetEmergencySystem())
			ES->TriggerEmergency(EEmergencyType::VillainAttack, Location);
		break;

	case ECityEvent::VillainSiegeEnd:
		OwningGameMode->bIsMetropolisUnderAttack = false;
		break;

	case ECityEvent::PortalOpening:
		if (UEmergencySystem* ES = OwningGameMode->GetEmergencySystem())
			ES->TriggerEmergency(EEmergencyType::AlienInvasion, Location);
		break;

	case ECityEvent::XKryptoniteContamination:
		if (UEmergencySystem* ES = OwningGameMode->GetEmergencySystem())
			ES->TriggerEmergency(EEmergencyType::XKryptiteContamination, Location);
		break;

	case ECityEvent::CarChase:
		if (UEmergencySystem* ES = OwningGameMode->GetEmergencySystem())
			ES->TriggerEmergency(EEmergencyType::BankRobbery, Location);
		break;

	case ECityEvent::JailBreak:
		if (UEmergencySystem* ES = OwningGameMode->GetEmergencySystem())
			ES->TriggerEmergency(EEmergencyType::HostageSituation, Location);
		break;

	default: break;
	}
}

void UCityEventSystem::SpawnVillainSiege(TSubclassOf<ABaseVillain> VillainClass,
	FVector Center, int32 WaveCount)
{
	SiegeVillainClass = VillainClass;
	SiegeCenter       = Center;
	MaxSiegeWaves     = WaveCount;
	ActiveSiegeWave   = 1;

	TriggerEvent(ECityEvent::VillainSiegeStart, Center);
}

void UCityEventSystem::ContinueSiegeWave()
{
	if (!OwningGameMode || !SiegeVillainClass) return;

	// Spawn wave — number increases per wave
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (int32 i = 0; i < ActiveSiegeWave * 2; ++i)
	{
		const FVector SpawnLoc = SiegeCenter + FVector(
			FMath::RandRange(-1500.f, 1500.f),
			FMath::RandRange(-1500.f, 1500.f),
			0.f);

		OwningGameMode->GetWorld()->SpawnActor<ABaseVillain>(
			SiegeVillainClass, SpawnLoc, FRotator::ZeroRotator, Params);
	}

	ActiveSiegeWave++;

	if (ActiveSiegeWave > MaxSiegeWaves)
	{
		ActiveSiegeWave = 0;
		TriggerEvent(ECityEvent::VillainSiegeEnd, SiegeCenter);
	}
}

void UCityEventSystem::SpawnAmbientEvent()
{
	if (!OwningGameMode) return;

	const float Roll = FMath::FRand();
	const float Threat = OwningGameMode->CityThreatLevel;

	ECityEvent ChosenEvent;
	if (Threat > 0.5f && Roll < 0.3f)
		ChosenEvent = ECityEvent::VillainSiegeStart;
	else if (Roll < 0.3f)
		ChosenEvent = ECityEvent::StreetCrime;
	else if (Roll < 0.5f)
		ChosenEvent = ECityEvent::CarChase;
	else if (Roll < 0.65f)
		ChosenEvent = ECityEvent::ConstructionExplosion;
	else if (Roll < 0.75f)
		ChosenEvent = ECityEvent::TrafficAccident;
	else
		ChosenEvent = ECityEvent::JailBreak;

	const FVector EventLoc = FVector(
		FMath::RandRange(-40000.f, 40000.f),
		FMath::RandRange(-40000.f, 40000.f),
		0.f);

	TriggerEvent(ChosenEvent, EventLoc);
}
