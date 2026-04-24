#include "DoomsdayCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

ADoomsdayCharacter::ADoomsdayCharacter()
{
	VillainType = EVillainType::Doomsday;

	// Doomsday is a tank — nearly unkillable
	VillainStats.MaxHealth    = 8000.f;
	VillainStats.CurrentHealth = 8000.f;
	VillainStats.AttackDamage = 250.f;
	VillainStats.ArmorRating  = 0.75f;
	VillainStats.bCanFly      = false;
	VillainStats.MoveSpeed    = 500.f;
	VillainStats.DetectionRadius = 5000.f;

	// Scale up the character
	SetActorScale3D(FVector(1.5f));
}

void ADoomsdayCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->GravityScale = 2.f; // heavy
}

void ADoomsdayCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	NextSlamCooldown   -= DeltaTime;
	NextChargeCooldown -= DeltaTime;

	if (VillainState != EVillainState::Attacking || !CurrentTarget) return;

	const float DistToTarget = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());

	// Ground slam when target is close
	if (NextSlamCooldown <= 0.f && DistToTarget < 600.f)
	{
		BoneSpikeSlam();
		NextSlamCooldown = 5.f;
	}

	// Charge when target is far
	if (NextChargeCooldown <= 0.f && DistToTarget > 1000.f && !bIsCharging)
	{
		UnstoppableCharge();
		NextChargeCooldown = 8.f;
	}

	// Continue charge
	if (bIsCharging)
	{
		AddMovementInput(ChargeDirection, 1.f);
		ChargeTimer -= DeltaTime;
		if (ChargeTimer <= 0.f)
		{
			bIsCharging = false;
			GetCharacterMovement()->MaxWalkSpeed = VillainStats.MoveSpeed;
		}
	}
}

float ADoomsdayCharacter::TakeDamage(float Damage, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	// Check adaptation — immune to damage types he's adapted to
	if (DamageEvent.DamageTypeClass)
	{
		for (const TSubclassOf<UDamageType>& Resistance : AcquiredResistances)
		{
			if (DamageEvent.DamageTypeClass == Resistance) return 0.f;
		}
	}

	const float Taken = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	// Every major hit makes him slightly stronger and triggers adaptation
	if (Taken > 100.f)
	{
		AdaptArmor();
		VillainStats.AttackDamage += 5.f; // enrage
	}

	return Taken;
}

void ADoomsdayCharacter::UseSignatureAbility()
{
	const float DistToTarget = CurrentTarget
		? FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation()) : 9999.f;

	if (DistToTarget < 600.f)
		BoneSpikeSlam();
	else if (!bIsCharging)
		UnstoppableCharge();
}

void ADoomsdayCharacter::BoneSpikeSlam()
{
	// Jump up, then crash down
	LaunchCharacter(FVector(0.f, 0.f, 1200.f), false, true);

	FTimerHandle SlamTimer;
	GetWorldTimerManager().SetTimer(SlamTimer, [this]()
	{
		// Impact on landing
		UGameplayStatics::ApplyRadialDamage(
			GetWorld(), SlamDamage, GetActorLocation(), SlamRadius,
			UDamageType::StaticClass(), {this}, this, GetController(), true);

		// Screen shake — handled in camera system
		// Spawn ground crack VFX — BP notifies this via event
	}, 1.2f, false);
}

void ADoomsdayCharacter::UnstoppableCharge()
{
	if (!CurrentTarget) return;

	bIsCharging = true;
	ChargeDirection = (CurrentTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
	GetCharacterMovement()->MaxWalkSpeed = ChargeSpeed;
	ChargeTimer = ChargeDistance / ChargeSpeed;

	// Break through destructible objects during charge (handled by physics)
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Destructible, ECR_Overlap);
}

void ADoomsdayCharacter::AdaptArmor()
{
	AdaptationCount++;

	// Each adaptation improves armor
	VillainStats.ArmorRating = FMath::Min(0.95f, VillainStats.ArmorRating + 0.03f);
}

void ADoomsdayCharacter::OnDefeated_Implementation()
{
	// Doomsday doesn't fully die — he collapses but might return (story hook)
	bIsCharging = false;
	VillainStats.bIsInvulnerable = true; // scripted moment

	// Dramatic fall, camera event triggered via Blueprint
	Super::OnDefeated_Implementation();
}
