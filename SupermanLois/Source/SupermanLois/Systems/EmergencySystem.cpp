#include "EmergencySystem.h"
#include "../Core/SupermanLoisGameMode.h"
#include "../Core/SupermanLoisGameState.h"
#include "../Core/SupermanLoisPlayerController.h"
#include "../AI/Emergency/PoliceOfficerAI.h"
#include "../AI/Emergency/FirefighterAI.h"
#include "../AI/Emergency/ParamedicAI.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"

void UEmergencySystem::Initialize(ASupermanLoisGameMode* GameMode)
{
	OwningGameMode = GameMode;
	RandomEmergencyTimer = FMath::RandRange(MinEmergencyInterval, MaxEmergencyInterval);
}

void UEmergencySystem::Tick(float DeltaTime)
{
	UpdateEmergencyTimers(DeltaTime);

	RandomEmergencyTimer -= DeltaTime;
	if (RandomEmergencyTimer <= 0.f)
	{
		SpawnRandomEmergency();
		RandomEmergencyTimer = FMath::RandRange(MinEmergencyInterval, MaxEmergencyInterval);
	}
}

void UEmergencySystem::TriggerEmergency(EEmergencyType Type, FVector Location)
{
	FActiveEmergency Emergency;
	Emergency.ID       = FGuid::NewGuid();
	Emergency.Location = Location;
	Emergency.Status   = EEmergencyStatus::Pending;

	// Load emergency data from table based on type
	switch (Type)
	{
	case EEmergencyType::BuildingFire:
		Emergency.Data.Type              = Type;
		Emergency.Data.Priority          = EEmergencyPriority::High;
		Emergency.Data.AlertMessage      = FText::FromString(TEXT("FIRE: Multi-story building ablaze in Metropolis. Civilians trapped."));
		Emergency.Data.TimeLimit         = 90.f;
		Emergency.Data.CiviliansAtRisk   = FMath::RandRange(5, 20);
		Emergency.Data.FireUnits         = 3;
		Emergency.Data.AMBUnits          = 2;
		Emergency.Data.PoliceUnits       = 2;
		Emergency.Data.RequiresSuperman  = false;
		break;

	case EEmergencyType::PlaneDown:
		Emergency.Data.Type              = Type;
		Emergency.Data.Priority          = EEmergencyPriority::Critical;
		Emergency.Data.AlertMessage      = FText::FromString(TEXT("MAYDAY: Commercial airliner in distress over Metropolis. Immediate Superman response needed."));
		Emergency.Data.TimeLimit         = 45.f;
		Emergency.Data.CiviliansAtRisk   = FMath::RandRange(80, 250);
		Emergency.Data.RequiresSuperman  = true;
		Emergency.Data.FireUnits         = 4;
		Emergency.Data.AMBUnits          = 6;
		Emergency.Data.PoliceUnits       = 4;
		break;

	case EEmergencyType::BridgeCollapse:
		Emergency.Data.Type              = Type;
		Emergency.Data.Priority          = EEmergencyPriority::Critical;
		Emergency.Data.AlertMessage      = FText::FromString(TEXT("BRIDGE FAILURE: Metropolis Harbor Bridge collapsing. Vehicles in the water."));
		Emergency.Data.TimeLimit         = 60.f;
		Emergency.Data.CiviliansAtRisk   = FMath::RandRange(10, 40);
		Emergency.Data.RequiresSuperman  = true;
		Emergency.Data.FireUnits         = 3;
		Emergency.Data.AMBUnits          = 4;
		Emergency.Data.PoliceUnits       = 6;
		break;

	case EEmergencyType::VillainAttack:
		Emergency.Data.Type              = Type;
		Emergency.Data.Priority          = EEmergencyPriority::Critical;
		Emergency.Data.AlertMessage      = FText::FromString(TEXT("VILLAIN ALERT: Powered individual destroying downtown Metropolis. All units stand by. Superman needed."));
		Emergency.Data.TimeLimit         = 180.f;
		Emergency.Data.CiviliansAtRisk   = FMath::RandRange(20, 80);
		Emergency.Data.RequiresSuperman  = true;
		Emergency.Data.PoliceUnits       = 8;
		Emergency.Data.AMBUnits          = 4;
		break;

	case EEmergencyType::AlienInvasion:
		Emergency.Data.Type              = Type;
		Emergency.Data.Priority          = EEmergencyPriority::Critical;
		Emergency.Data.AlertMessage      = FText::FromString(TEXT("INVASION ALERT: Unidentified hostiles entering Metropolis airspace. Military assets mobilizing."));
		Emergency.Data.TimeLimit         = 300.f;
		Emergency.Data.CiviliansAtRisk   = FMath::RandRange(100, 500);
		Emergency.Data.RequiresSuperman  = true;
		Emergency.Data.PoliceUnits       = 12;
		Emergency.Data.AMBUnits          = 8;
		Emergency.Data.FireUnits         = 6;
		break;

	case EEmergencyType::HostageSituation:
		Emergency.Data.Type              = Type;
		Emergency.Data.Priority          = EEmergencyPriority::High;
		Emergency.Data.AlertMessage      = FText::FromString(TEXT("HOSTAGE: Armed suspects holding civilians at Metropolis National Bank. Negotiators en route."));
		Emergency.Data.TimeLimit         = 150.f;
		Emergency.Data.CiviliansAtRisk   = FMath::RandRange(3, 15);
		Emergency.Data.PoliceUnits       = 6;
		Emergency.Data.AMBUnits          = 2;
		break;

	case EEmergencyType::GasPipelineExplosion:
		Emergency.Data.Type              = Type;
		Emergency.Data.Priority          = EEmergencyPriority::High;
		Emergency.Data.AlertMessage      = FText::FromString(TEXT("EXPLOSION: Gas main rupture and fire in Metropolis industrial district. Evacuation in progress."));
		Emergency.Data.TimeLimit         = 120.f;
		Emergency.Data.CiviliansAtRisk   = FMath::RandRange(0, 10);
		Emergency.Data.FireUnits         = 5;
		Emergency.Data.AMBUnits          = 3;
		Emergency.Data.PoliceUnits       = 3;
		break;

	case EEmergencyType::TornadoWarning:
		Emergency.Data.Type              = Type;
		Emergency.Data.Priority          = EEmergencyPriority::High;
		Emergency.Data.AlertMessage      = FText::FromString(TEXT("TORNADO WARNING: F3 tornado approaching Smallville. Residents seek shelter immediately."));
		Emergency.Data.TimeLimit         = 90.f;
		Emergency.Data.CiviliansAtRisk   = FMath::RandRange(5, 30);
		Emergency.Data.PoliceUnits       = 3;
		Emergency.Data.AMBUnits          = 2;
		break;

	case EEmergencyType::MetaHumanRampage:
		Emergency.Data.Type              = Type;
		Emergency.Data.Priority          = EEmergencyPriority::Critical;
		Emergency.Data.AlertMessage      = FText::FromString(TEXT("META ALERT: X-Kryptonite-enhanced individual causing mass destruction. Standard weapons ineffective."));
		Emergency.Data.TimeLimit         = 120.f;
		Emergency.Data.CiviliansAtRisk   = FMath::RandRange(10, 50);
		Emergency.Data.RequiresSuperman  = true;
		Emergency.Data.PoliceUnits       = 6;
		Emergency.Data.AMBUnits          = 3;
		break;

	default:
		Emergency.Data.Type              = Type;
		Emergency.Data.Priority          = EEmergencyPriority::Medium;
		Emergency.Data.AlertMessage      = FText::FromString(TEXT("Emergency reported. Units responding."));
		Emergency.Data.TimeLimit         = 120.f;
		Emergency.Data.CiviliansAtRisk   = FMath::RandRange(1, 5);
		Emergency.Data.PoliceUnits       = 2;
		break;
	}

	Emergency.TimeRemaining = Emergency.Data.TimeLimit;

	ActiveEmergencies.Add(Emergency);
	SpawnResponders(Emergency);

	// Notify player controller for HUD alert
	if (UWorld* World = OwningGameMode->GetWorld())
	{
		if (ASupermanLoisPlayerController* PC = Cast<ASupermanLoisPlayerController>(
			UGameplayStatics::GetPlayerController(World, 0)))
		{
			PC->ShowEmergencyAlert(Emergency.Data.AlertMessage, Location);
		}

		// Update game mode stats
		OwningGameMode->ActiveEmergencyCount++;

		if (Emergency.Data.Priority == EEmergencyPriority::Critical)
			OwningGameMode->bIsMetropolisUnderAttack = true;
	}

	OnEmergencyTriggered.Broadcast(Emergency);
}

void UEmergencySystem::SpawnResponders(const FActiveEmergency& Emergency)
{
	if (!OwningGameMode) return;
	UWorld* World = OwningGameMode->GetWorld();
	if (!World) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	const FVector Base = Emergency.Location;

	// Spawn police
	for (int32 i = 0; i < Emergency.Data.PoliceUnits; ++i)
	{
		const FVector SpawnLoc = Base + FVector(FMath::RandRange(-500.f, 500.f), FMath::RandRange(-500.f, 500.f), 0.f);
		World->SpawnActor<APoliceOfficerAI>(SpawnLoc, FRotator::ZeroRotator, SpawnParams);
	}

	// Spawn firefighters
	for (int32 i = 0; i < Emergency.Data.FireUnits; ++i)
	{
		const FVector SpawnLoc = Base + FVector(FMath::RandRange(-600.f, 600.f), FMath::RandRange(-600.f, 600.f), 0.f);
		World->SpawnActor<AFirefighterAI>(SpawnLoc, FRotator::ZeroRotator, SpawnParams);
	}

	// Spawn paramedics
	for (int32 i = 0; i < Emergency.Data.AMBUnits; ++i)
	{
		const FVector SpawnLoc = Base + FVector(FMath::RandRange(-400.f, 400.f), FMath::RandRange(-400.f, 400.f), 0.f);
		World->SpawnActor<AParamedicAI>(SpawnLoc, FRotator::ZeroRotator, SpawnParams);
	}
}

void UEmergencySystem::UpdateEmergencyTimers(float DeltaTime)
{
	for (int32 i = ActiveEmergencies.Num() - 1; i >= 0; --i)
	{
		FActiveEmergency& E = ActiveEmergencies[i];

		if (E.Status == EEmergencyStatus::Resolved || E.Status == EEmergencyStatus::Failed)
		{
			ActiveEmergencies.RemoveAt(i);
			if (OwningGameMode) OwningGameMode->ActiveEmergencyCount--;
			continue;
		}

		E.TimeRemaining -= DeltaTime;

		if (E.TimeRemaining <= 0.f)
		{
			// Failed — civilians lost
			ResolveEmergency(E.ID, false);
		}
	}
}

void UEmergencySystem::ResolveEmergency(FGuid EmergencyID, bool bSucceeded)
{
	for (FActiveEmergency& E : ActiveEmergencies)
	{
		if (E.ID == EmergencyID)
		{
			E.Status = bSucceeded ? EEmergencyStatus::Resolved : EEmergencyStatus::Failed;

			if (OwningGameMode)
			{
				if (ASupermanLoisGameState* GS = Cast<ASupermanLoisGameState>(
					OwningGameMode->GetWorld()->GetGameState()))
				{
					if (bSucceeded)
						GS->WorldState.CiviliansRescued += E.CiviliansRescued;
					else
						GS->WorldState.CiviliansLost += (E.Data.CiviliansAtRisk - E.CiviliansRescued);
				}
			}

			OnEmergencyResolved.Broadcast(EmergencyID, bSucceeded);
			break;
		}
	}
}

FActiveEmergency UEmergencySystem::GetNearestEmergency(FVector PlayerLocation) const
{
	FActiveEmergency Nearest;
	float MinDist = MAX_FLT;

	for (const FActiveEmergency& E : ActiveEmergencies)
	{
		if (E.Status == EEmergencyStatus::Resolved || E.Status == EEmergencyStatus::Failed) continue;

		const float Dist = FVector::Dist(PlayerLocation, E.Location);
		if (Dist < MinDist)
		{
			MinDist = Dist;
			Nearest = E;
		}
	}

	return Nearest;
}

void UEmergencySystem::SpawnRandomEmergency()
{
	// Weight emergency types by city threat level
	const float ThreatLevel = OwningGameMode ? OwningGameMode->CityThreatLevel : 0.f;

	EEmergencyType ChosenType;
	const float Roll = FMath::FRand();

	if (ThreatLevel > 0.7f && Roll < 0.4f)
		ChosenType = EEmergencyType::VillainAttack;
	else if (ThreatLevel > 0.5f && Roll < 0.3f)
		ChosenType = EEmergencyType::MetaHumanRampage;
	else if (Roll < 0.25f)
		ChosenType = EEmergencyType::BuildingFire;
	else if (Roll < 0.4f)
		ChosenType = EEmergencyType::MultiVehicleCrash;
	else if (Roll < 0.55f)
		ChosenType = EEmergencyType::HostageSituation;
	else if (Roll < 0.65f)
		ChosenType = EEmergencyType::GasPipelineExplosion;
	else if (Roll < 0.75f)
		ChosenType = EEmergencyType::BankRobbery;
	else if (Roll < 0.85f)
		ChosenType = EEmergencyType::PlaneDown;
	else
		ChosenType = EEmergencyType::BridgeCollapse;

	// Pick random Metropolis or Smallville location
	const FVector Loc = FMath::RandBool()
		? GetRandomMetropolisLocation()
		: GetRandomSmallvilleLocation();

	TriggerEmergency(ChosenType, Loc);
}

FVector UEmergencySystem::GetRandomMetropolisLocation() const
{
	// Metropolis world bounds — placeholder, set to actual level extents
	return FVector(
		FMath::RandRange(-50000.f, 50000.f),
		FMath::RandRange(-50000.f, 50000.f),
		0.f);
}

FVector UEmergencySystem::GetRandomSmallvilleLocation() const
{
	return FVector(
		FMath::RandRange(80000.f, 130000.f),
		FMath::RandRange(-20000.f, 20000.f),
		0.f);
}
