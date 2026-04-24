#include "PoliceOfficerAI.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

APoliceOfficerAI::APoliceOfficerAI()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCharacterMovement()->MaxWalkSpeed = 450.f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void APoliceOfficerAI::BeginPlay()
{
	Super::BeginPlay();

	// Start ambient radio chatter
	GetWorldTimerManager().SetTimer(RadioChatterTimer,
		this, &APoliceOfficerAI::PlayRadioChatter,
		FMath::RandRange(5.f, 15.f), true);
}

void APoliceOfficerAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ShootCooldown -= DeltaTime;

	switch (PoliceState)
	{
	case EPoliceState::Responding:
		if (!EmergencyTarget.IsZero())
		{
			const float DistToScene = FVector::Dist(GetActorLocation(), EmergencyTarget);
			if (DistToScene < 300.f)
			{
				PoliceState = EPoliceState::Securing;
				if (!bPerimeterEstablished)
					SetupPerimeter(EmergencyTarget, 600.f);
			}
		}
		break;

	case EPoliceState::Securing:
	case EPoliceState::Engaging:
		LookForThreats();
		if (CurrentEngageTarget && ShootCooldown <= 0.f)
		{
			AttemptShoot();
			ShootCooldown = ShootInterval;
		}
		break;

	case EPoliceState::RequestingBackup:
		BroadcastRadio(TEXT("All units — requesting backup! We have a 10-100 in progress!"));
		PoliceState = EPoliceState::Securing;
		break;

	default: break;
	}
}

float APoliceOfficerAI::TakeDamage(float Damage, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	Health -= Damage;

	if (Health <= 0.f)
	{
		OnOfficerDown.Broadcast();
		// Ragdoll
		GetMesh()->SetSimulatePhysics(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetActorTickEnabled(false);
	}
	else if (Health < 30.f)
	{
		PoliceState = EPoliceState::RequestingBackup;
	}

	return Damage;
}

void APoliceOfficerAI::RespondToEmergency(FVector EmergencyLocation)
{
	EmergencyTarget = EmergencyLocation;
	PoliceState = EPoliceState::Responding;

	if (AAIController* AIC = Cast<AAIController>(GetController()))
		AIC->MoveToLocation(EmergencyLocation, 200.f, false);
}

void APoliceOfficerAI::SetupPerimeter(FVector Center, float Radius)
{
	bPerimeterEstablished = true;

	// Move to a position around the perimeter
	const float Angle = FMath::RandRange(0.f, 360.f);
	const FVector PerimPos = Center + FVector(
		FMath::Cos(FMath::DegreesToRadians(Angle)) * Radius,
		FMath::Sin(FMath::DegreesToRadians(Angle)) * Radius,
		0.f);

	if (AAIController* AIC = Cast<AAIController>(GetController()))
		AIC->MoveToLocation(PerimPos, 50.f, false);
}

void APoliceOfficerAI::EvacuateCivilian(AActor* Civilian)
{
	if (!Civilian) return;
	PoliceState = EPoliceState::Evacuating;

	// Guide civilian away from danger (simplified — tell civilian to follow us)
	Civilian->Tags.AddUnique(FName("Evacuating"));
}

void APoliceOfficerAI::OnSupermanArrived()
{
	bSupermanOnScene = true;

	// Officers stand down from combat — let Superman handle it
	if (PoliceState == EPoliceState::Engaging)
		PoliceState = EPoliceState::Securing;

	// Play greeting VO
	if (SupermanGreetingLines.Num() > 0)
	{
		USoundBase* Line = SupermanGreetingLines[FMath::RandRange(0, SupermanGreetingLines.Num()-1)];
		UGameplayStatics::PlaySoundAtLocation(this, Line, GetActorLocation());
	}
}

void APoliceOfficerAI::LookForThreats()
{
	TArray<FOverlapResult> Overlaps;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(ShootRange);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->OverlapMultiByChannel(Overlaps, GetActorLocation(),
		FQuat::Identity, ECC_Pawn, Sphere, Params))
	{
		for (const FOverlapResult& Overlap : Overlaps)
		{
			AActor* Candidate = Overlap.GetActor();
			if (Candidate && Candidate->Tags.Contains(FName("Villain")))
			{
				CurrentEngageTarget = Candidate;
				PoliceState = EPoliceState::Engaging;

				// Face threat
				const FVector Dir = (Candidate->GetActorLocation() - GetActorLocation()).GetSafeNormal();
				SetActorRotation(Dir.ToOrientationRotator());
				return;
			}
		}
	}

	CurrentEngageTarget = nullptr;
	if (PoliceState == EPoliceState::Engaging)
		PoliceState = EPoliceState::Securing;
}

void APoliceOfficerAI::AttemptShoot()
{
	if (!CurrentEngageTarget || bSupermanOnScene) return;

	// Line of sight check
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	const FVector GunMuzzle = GetMesh()->GetSocketLocation(TEXT("GunMuzzle"));
	const FVector TargetPos = CurrentEngageTarget->GetActorLocation();

	if (GetWorld()->LineTraceSingleByChannel(Hit, GunMuzzle, TargetPos, ECC_Visibility, Params))
	{
		if (Hit.GetActor() == CurrentEngageTarget)
		{
			// Fire — minimal damage vs powered enemies
			UGameplayStatics::ApplyPointDamage(CurrentEngageTarget, AttackDamage,
				(TargetPos - GunMuzzle).GetSafeNormal(), Hit,
				GetController(), this, UDamageType::StaticClass());
		}
	}
}

void APoliceOfficerAI::BroadcastRadio(const FString& Message)
{
	// Play radio crackle + voice — handled via MetaSounds blueprint
}

void APoliceOfficerAI::PlayRadioChatter()
{
	if (RadioChatterSounds.Num() == 0) return;
	USoundBase* Chatter = RadioChatterSounds[FMath::RandRange(0, RadioChatterSounds.Num()-1)];
	UGameplayStatics::PlaySoundAtLocation(this, Chatter, GetActorLocation(), 0.4f);
}
