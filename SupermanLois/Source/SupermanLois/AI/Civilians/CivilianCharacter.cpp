#include "CivilianCharacter.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

ACivilianCharacter::ACivilianCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCharacterMovement()->MaxWalkSpeed = 200.f;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	Tags.Add(FName("Civilian"));
}

void ACivilianCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Start random wander
	FTimerHandle WanderTimer;
	GetWorldTimerManager().SetTimer(WanderTimer, [this]()
	{
		if (CivilianBehavior == ECivilianBehavior::WalkingAbout)
			CheckForNearbyThreats();
	}, 2.f, true);
}

void ACivilianCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CivilianBehavior == ECivilianBehavior::PanickingFlee)
		UpdatePanicBehavior(DeltaTime);
}

float ACivilianCharacter::TakeDamage(float Damage, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	Health -= Damage;

	if (Health <= 0.f)
	{
		// Civilian collapses — ragdoll
		GetMesh()->SetSimulatePhysics(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Tags.AddUnique(FName("Injured"));
		CivilianBehavior = ECivilianBehavior::Injured;
		SetActorTickEnabled(false);
	}
	else if (Health < 40.f)
	{
		Tags.AddUnique(FName("Injured"));
		CivilianBehavior = ECivilianBehavior::Injured;
	}

	return Damage;
}

void ACivilianCharacter::StartPanicking(FVector ThreatLocation)
{
	CivilianBehavior = ECivilianBehavior::PanickingFlee;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	PanicTimer = 0.f;

	PickFleeDirection(ThreatLocation);

	// Play panic voiceline
	if (PanicLines.Num() > 0)
	{
		USoundBase* Line = PanicLines[FMath::RandRange(0, PanicLines.Num()-1)];
		UGameplayStatics::PlaySoundAtLocation(this, Line, GetActorLocation(), 0.8f);
	}
}

void ACivilianCharacter::StopPanicking()
{
	CivilianBehavior = ECivilianBehavior::WalkingAbout;
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
}

void ACivilianCharacter::WitnessSuperman(ACharacter* Superman)
{
	if (CivilianBehavior == ECivilianBehavior::PanickingFlee)
	{
		// Seeing Superman calms civilians
		CivilianBehavior = ECivilianBehavior::WatchingSuperman;
		GetCharacterMovement()->MaxWalkSpeed = 200.f;

		// Face Superman
		if (Superman)
		{
			const FVector LookDir = (Superman->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
			SetActorRotation(LookDir.ToOrientationRotator());
		}

		// Awe voiceline
		if (AweLines.Num() > 0)
		{
			USoundBase* Line = AweLines[FMath::RandRange(0, AweLines.Num()-1)];
			UGameplayStatics::PlaySoundAtLocation(this, Line, GetActorLocation(), 0.7f);
		}
	}
}

void ACivilianCharacter::SetTrapped(bool bTrapped)
{
	bIsTrapped = bTrapped;

	if (bTrapped)
	{
		CivilianBehavior = ECivilianBehavior::Trapped;
		Tags.AddUnique(FName("NeedsRescue"));
		OnCivilianTrapped.Broadcast(this);

		if (AAIController* AIC = Cast<AAIController>(GetController()))
			AIC->StopMovement();
	}
}

void ACivilianCharacter::OnRescued()
{
	bIsTrapped    = false;
	Health        = 100.f;
	CivilianBehavior = ECivilianBehavior::Rescued;

	Tags.Remove(FName("NeedsRescue"));
	Tags.Remove(FName("Injured"));

	GetCharacterMovement()->MaxWalkSpeed = 350.f;

	// Move to safety
	FTimerHandle SafetyTimer;
	GetWorldTimerManager().SetTimer(SafetyTimer, [this]()
	{
		CivilianBehavior = ECivilianBehavior::WalkingAbout;
		GetCharacterMovement()->MaxWalkSpeed = 200.f;
	}, 10.f, false);
}

void ACivilianCharacter::PickFleeDirection(FVector ThreatLocation)
{
	// Flee directly away from threat
	const FVector AwayDir = (GetActorLocation() - ThreatLocation).GetSafeNormal2D();

	// Add some randomness so civilians scatter
	const float RandomAngle = FMath::RandRange(-45.f, 45.f);
	const FVector FleeDir = AwayDir.RotateAngleAxis(RandomAngle, FVector::UpVector);

	FleeTarget = GetActorLocation() + FleeDir * 3000.f;

	if (AAIController* AIC = Cast<AAIController>(GetController()))
		AIC->MoveToLocation(FleeTarget, 100.f, false);
}

void ACivilianCharacter::UpdatePanicBehavior(float DeltaTime)
{
	PanicTimer += DeltaTime;

	// Re-pick flee destination every 3 seconds if still panicking
	if (PanicTimer > 3.f)
	{
		PanicTimer = 0.f;
		CheckForNearbyThreats();
	}
}

void ACivilianCharacter::CheckForNearbyThreats()
{
	TArray<FOverlapResult> Overlaps;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(PanicRadius);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->OverlapMultiByChannel(
		Overlaps, GetActorLocation(), FQuat::Identity, ECC_Pawn, Sphere, Params))
	{
		for (const FOverlapResult& Overlap : Overlaps)
		{
			if (AActor* Nearby = Overlap.GetActor())
			{
				if (Nearby->Tags.Contains(FName("Villain")))
				{
					StartPanicking(Nearby->GetActorLocation());
					return;
				}
			}
		}
	}
}
