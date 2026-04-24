#include "ParamedicAI.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AParamedicAI::AParamedicAI()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
}

void AParamedicAI::BeginPlay()
{
	Super::BeginPlay();
}

void AParamedicAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MedicState == EParamedicState::TreatingPatient && CurrentPatient)
	{
		TreatTimer += DeltaTime;

		// Heal patient over time — they need to be close
		if (FVector::Dist(GetActorLocation(), CurrentPatient->GetActorLocation()) < TreatmentRange)
		{
			// Apply healing via damage event (negative) — or through health component
			// Here we tag the patient as being treated
			CurrentPatient->Tags.AddUnique(FName("UnderMedicalCare"));

			if (TreatTimer >= 5.f)
			{
				OnPatientTreated.Broadcast(CurrentPatient);
				CurrentPatient->Tags.Remove(FName("Injured"));
				CurrentPatient = nullptr;
				MedicState     = EParamedicState::StandingBy;
				TreatTimer     = 0.f;
			}
		}
	}
}

float AParamedicAI::TakeDamage(float Damage, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	Health -= Damage;

	if (Health <= 0.f)
	{
		GetMesh()->SetSimulatePhysics(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetActorTickEnabled(false);
	}

	return Damage;
}

void AParamedicAI::RespondToScene(FVector SceneLocation)
{
	SceneTarget = SceneLocation;
	MedicState  = EParamedicState::Responding;

	if (AAIController* AIC = Cast<AAIController>(GetController()))
		AIC->MoveToLocation(SceneLocation, 200.f, false);
}

void AParamedicAI::TreatPatient(AActor* Patient)
{
	if (!Patient) return;

	CurrentPatient = Patient;
	MedicState     = EParamedicState::TreatingPatient;
	TreatTimer     = 0.f;

	if (AAIController* AIC = Cast<AAIController>(GetController()))
		AIC->MoveToActor(Patient, 150.f, false);
}

void AParamedicAI::SetupTriage(FVector Center)
{
	bTriageReady = true;
	MedicState   = EParamedicState::SettingUpTriage;

	if (AAIController* AIC = Cast<AAIController>(GetController()))
		AIC->MoveToLocation(Center, 100.f, false);
}

void AParamedicAI::TransportToHospital(AActor* Patient)
{
	if (!Patient) return;

	MedicState = EParamedicState::Transporting;
	// Trigger ambulance vehicle to drive to hospital — handled by traffic/vehicle system
}
