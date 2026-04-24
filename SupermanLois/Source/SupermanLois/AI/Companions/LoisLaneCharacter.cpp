#include "LoisLaneCharacter.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

ALoisLaneCharacter::ALoisLaneCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCharacterMovement()->MaxWalkSpeed = 350.f;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	Tags.Add(FName("LoisLane"));
}

void ALoisLaneCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ALoisLaneCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (LoisState == ELoisState::Investigating)
		UpdateInvestigation(DeltaTime);

	if (LoisState == ELoisState::InDanger && !bIsCaptured)
	{
		// Call for help periodically
		FTimerHandle CallTimer;
		GetWorldTimerManager().SetTimer(CallTimer, this, &ALoisLaneCharacter::CallForSuperman, 5.f, false);
	}
}

float ALoisLaneCharacter::TakeDamage(float Damage, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	Health -= Damage;

	if (Health <= 30.f && LoisState != ELoisState::InDanger)
	{
		LoisState = ELoisState::InDanger;
		OnLoisInDanger.Broadcast();
		CallForSuperman();
	}

	return Damage;
}

void ALoisLaneCharacter::TakePhoto(FVector Subject)
{
	OnLoisPhotoTaken.Broadcast(Subject);
	// Adds photo to Daily Planet story board — mission system picks this up
}

void ALoisLaneCharacter::InvestigateLead(FVector Location, FText LeadDescription)
{
	CurrentLeadLocation   = Location;
	InvestigationProgress = 0.f;
	InvestigationTarget   = 100.f;
	LoisState             = ELoisState::Investigating;

	if (AAIController* AIC = Cast<AAIController>(GetController()))
		AIC->MoveToLocation(Location, 100.f, false);
}

void ALoisLaneCharacter::UpdateInvestigation(float DeltaTime)
{
	if (FVector::Dist(GetActorLocation(), CurrentLeadLocation) < 200.f)
	{
		InvestigationProgress = FMath::Min(100.f, InvestigationProgress + 5.f * DeltaTime);

		if (InvestigationProgress >= 100.f)
		{
			LoisState = ELoisState::AtDailyPlanet;
			// Notify mission system of completed investigation
		}
	}
}

void ALoisLaneCharacter::CallForSuperman()
{
	PlayVoiceLine(DangerCalloutLines);
}

void ALoisLaneCharacter::SetCaptured(bool bCaptured)
{
	bIsCaptured = bCaptured;
	LoisState   = bCaptured ? ELoisState::Captured : ELoisState::InDanger;

	if (bCaptured)
	{
		if (AAIController* AIC = Cast<AAIController>(GetController()))
			AIC->StopMovement();
	}

	if (bCaptured)
	{
		OnLoisInDanger.Broadcast();
		CallForSuperman();
	}
}

void ALoisLaneCharacter::OnRescuedBySuperman()
{
	Health    = 100.f;
	LoisState = ELoisState::Rescued;
	bIsCaptured = false;

	PlayVoiceLine(RescuedThankYouLines);

	// Return to Daily Planet after delay
	FTimerHandle ReturnTimer;
	GetWorldTimerManager().SetTimer(ReturnTimer, [this]()
	{
		LoisState = ELoisState::AtDailyPlanet;
	}, 5.f, false);
}

void ALoisLaneCharacter::LookAround()
{
	// Rotate Lois to face points of interest — driven by perception component
}

void ALoisLaneCharacter::PlayVoiceLine(const TArray<USoundBase*>& Lines)
{
	if (Lines.Num() == 0) return;
	USoundBase* Line = Lines[FMath::RandRange(0, Lines.Num()-1)];
	UGameplayStatics::PlaySoundAtLocation(this, Line, GetActorLocation());
}
