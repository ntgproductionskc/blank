#include "FirefighterAI.h"
#include "AIController.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

AFirefighterAI::AFirefighterAI()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	HoseStreamNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("HoseStream"));
	HoseStreamNiagara->SetupAttachment(GetMesh(), TEXT("HoseNozzle"));
	HoseStreamNiagara->SetAutoActivate(false);

	GetCharacterMovement()->MaxWalkSpeed = 380.f;
}

void AFirefighterAI::BeginPlay()
{
	Super::BeginPlay();
}

void AFirefighterAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FFState == EFirefighterState::FightingFire && bHoseDeployed)
	{
		HoseTimer += DeltaTime;

		// Suppress fire actor tags periodically
		TArray<FOverlapResult> Overlaps;
		FCollisionShape Sphere = FCollisionShape::MakeSphere(HoseRange);
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		if (GetWorld()->OverlapMultiByChannel(Overlaps, GetActorLocation(),
			FQuat::Identity, ECC_WorldDynamic, Sphere, Params))
		{
			for (const FOverlapResult& Overlap : Overlaps)
			{
				if (AActor* FireActor = Overlap.GetActor())
				{
					if (FireActor->Tags.Contains(FName("OnFire")))
					{
						// Tell fire actor to reduce intensity — handled by fire component
						FireActor->Tags.AddUnique(FName("BeingSuppressed"));
					}
				}
			}
		}
	}
}

float AFirefighterAI::TakeDamage(float Damage, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	Health -= Damage;

	if (Health <= 0.f)
	{
		OnFirefighterDown.Broadcast();
		RetractHose();
		GetMesh()->SetSimulatePhysics(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetActorTickEnabled(false);
	}
	else if (Health < 40.f)
	{
		FFState = EFirefighterState::Retreating;
		RetractHose();
	}

	return Damage;
}

void AFirefighterAI::RespondToFire(FVector FireLocation)
{
	FireTarget = FireLocation;
	FFState    = EFirefighterState::Responding;

	if (AAIController* AIC = Cast<AAIController>(GetController()))
		AIC->MoveToLocation(FireLocation, 300.f, false);

	// Once close enough, deploy hose
	FTimerHandle ArrivalCheck;
	GetWorldTimerManager().SetTimer(ArrivalCheck, [this, FireLocation]()
	{
		if (FVector::Dist(GetActorLocation(), FireLocation) < 500.f)
			DeployHose();
	}, 0.5f, true);
}

void AFirefighterAI::RescueCivilian(AActor* Civilian, FVector SafeZone)
{
	if (!Civilian) return;
	FFState = EFirefighterState::RescuingCivilian;

	if (AAIController* AIC = Cast<AAIController>(GetController()))
		AIC->MoveToActor(Civilian, 100.f, false);
}

void AFirefighterAI::DeployHose()
{
	bHoseDeployed = true;
	FFState       = EFirefighterState::FightingFire;
	HoseStreamNiagara->Activate(true);

	// Face the fire
	if (!FireTarget.IsZero())
	{
		const FVector Dir = (FireTarget - GetActorLocation()).GetSafeNormal2D();
		SetActorRotation(Dir.ToOrientationRotator());
	}
}

void AFirefighterAI::RetractHose()
{
	bHoseDeployed = false;
	HoseStreamNiagara->Deactivate();
}
