#include "ReputationSystem.h"
#include "../Core/SupermanLoisGameMode.h"
#include "../Core/SupermanLoisGameState.h"
#include "Kismet/GameplayStatics.h"

void UReputationSystem::Initialize(ASupermanLoisGameMode* GameMode)
{
	OwningGameMode     = GameMode;
	CurrentReputation  = 50.f;
}

void UReputationSystem::AddReputation(float Delta, const FText& Reason)
{
	CurrentReputation = FMath::Clamp(CurrentReputation + Delta, 0.f, 100.f);

	FReputationEvent Event;
	Event.Description = Reason;
	Event.Delta       = Delta;
	Event.Timestamp   = OwningGameMode ? OwningGameMode->GetWorld()->GetTimeSeconds() : 0.f;

	RecentEvents.Add(Event);
	if (RecentEvents.Num() > 20) RecentEvents.RemoveAt(0);

	// Sync to game state
	if (OwningGameMode)
	{
		if (ASupermanLoisGameState* GS = Cast<ASupermanLoisGameState>(
			OwningGameMode->GetWorld()->GetGameState()))
		{
			GS->WorldState.PlayerReputation = CurrentReputation;
		}
	}

	OnReputationChanged.Broadcast(CurrentReputation, GetTier());
}

EReputationTier UReputationSystem::GetTier() const
{
	if (CurrentReputation < 20.f) return EReputationTier::Unknown;
	if (CurrentReputation < 40.f) return EReputationTier::Vigilante;
	if (CurrentReputation < 60.f) return EReputationTier::Hero;
	if (CurrentReputation < 80.f) return EReputationTier::Champion;
	return EReputationTier::Legend;
}

FText UReputationSystem::GetTierDisplayName() const
{
	switch (GetTier())
	{
	case EReputationTier::Unknown:   return FText::FromString(TEXT("Unknown"));
	case EReputationTier::Vigilante: return FText::FromString(TEXT("Vigilante"));
	case EReputationTier::Hero:      return FText::FromString(TEXT("Hero"));
	case EReputationTier::Champion:  return FText::FromString(TEXT("Champion of Metropolis"));
	case EReputationTier::Legend:    return FText::FromString(TEXT("The Man of Steel"));
	default:                         return FText::FromString(TEXT("Unknown"));
	}
}

float UReputationSystem::GetTierProgress() const
{
	switch (GetTier())
	{
	case EReputationTier::Unknown:   return (CurrentReputation) / 20.f;
	case EReputationTier::Vigilante: return (CurrentReputation - 20.f) / 20.f;
	case EReputationTier::Hero:      return (CurrentReputation - 40.f) / 20.f;
	case EReputationTier::Champion:  return (CurrentReputation - 60.f) / 20.f;
	case EReputationTier::Legend:    return (CurrentReputation - 80.f) / 20.f;
	default:                         return 0.f;
	}
}
