#include "MorganEdgeCharacter.h"
#include "../Character/SupermanCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

AMorganEdgeCharacter::AMorganEdgeCharacter()
{
	VillainType = EVillainType::MorganEdge;

	VillainStats.MaxHealth    = 2500.f;
	VillainStats.CurrentHealth = 2500.f;
	VillainStats.AttackDamage = 140.f;
	VillainStats.ArmorRating  = 0.4f;
	VillainStats.bCanFly      = true;
	VillainStats.FlySpeed     = 1400.f;
	VillainStats.DetectionRadius = 4000.f;
}

void AMorganEdgeCharacter::BeginPlay()
{
	Super::BeginPlay();
	StartFlying();
	// Spawn initial soldier escort
	SpawnXKryptoniteSoldiers(3);
}

void AMorganEdgeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (VillainState != EVillainState::Attacking) return;

	BlastTimer  -= DeltaTime;
	SoldierTimer -= DeltaTime;

	// Keep X-Kryptonite field active near low health
	if (GetHealthPercent() < 0.5f)
		ActivateXKryptoniteField();

	// Periodic energy blasts
	if (BlastTimer <= 0.f && CurrentTarget)
	{
		TriggerKryptonianBlast();
		BlastTimer = 3.f;
	}

	// Reinforce soldiers
	if (SoldierTimer <= 0.f && ActiveSoldiers.Num() < 3)
	{
		SpawnXKryptoniteSoldiers(2);
		SoldierTimer = 20.f;
	}
}

void AMorganEdgeCharacter::UseSignatureAbility()
{
	TriggerKryptonianBlast();
}

void AMorganEdgeCharacter::SpawnXKryptoniteSoldiers(int32 Count)
{
	if (!XKryptoniteSoldierClass) return;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (int32 i = 0; i < Count; ++i)
	{
		const FVector Offset = GetActorRightVector() * (200.f * (i - Count / 2))
			+ GetActorForwardVector() * -300.f;

		ABaseVillain* Soldier = GetWorld()->SpawnActor<ABaseVillain>(
			XKryptoniteSoldierClass, GetActorLocation() + Offset,
			GetActorRotation(), Params);

		if (Soldier)
			ActiveSoldiers.Add(Soldier);
	}
}

void AMorganEdgeCharacter::ActivateXKryptoniteField()
{
	// Find Superman in radius and apply kryptonite exposure
	TArray<FOverlapResult> Overlaps;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(XKryptoniteFieldRadius);
	FCollisionQueryParams QParams;
	QParams.AddIgnoredActor(this);

	if (GetWorld()->OverlapMultiByChannel(
		Overlaps, GetActorLocation(), FQuat::Identity, ECC_Pawn, Sphere, QParams))
	{
		for (const FOverlapResult& Overlap : Overlaps)
		{
			if (ASupermanCharacter* Superman = Cast<ASupermanCharacter>(Overlap.GetActor()))
			{
				Superman->AddKryptoniteExposure(XKryptoniteStrength * GetWorld()->GetDeltaSeconds());
			}
		}
	}
}

void AMorganEdgeCharacter::TriggerKryptonianBlast()
{
	if (!CurrentTarget) return;

	// Projectile-style energy blast — launch with physics
	const FVector BlastDir = (CurrentTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();

	FHitResult Hit;
	const FVector Start = GetActorLocation() + BlastDir * 150.f;
	const FVector End   = Start + BlastDir * 4000.f;

	FCollisionQueryParams QParams;
	QParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, QParams))
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			UGameplayStatics::ApplyPointDamage(HitActor, BlastDamage,
				BlastDir, Hit, GetController(), this, UDamageType::StaticClass());

			// Push target back
			if (ACharacter* HitChar = Cast<ACharacter>(HitActor))
				HitChar->LaunchCharacter(BlastDir * 2000.f, true, false);
		}
	}
}

void AMorganEdgeCharacter::CallAirstrike()
{
	// Triggers military helicopter attack on Superman's position — handled by CityEventSystem
}

void AMorganEdgeCharacter::OnDefeated_Implementation()
{
	// Soldiers stop fighting when Morgan Edge falls
	for (ABaseVillain* Soldier : ActiveSoldiers)
	{
		if (Soldier && Soldier->IsValid())
			Soldier->SetVillainState(EVillainState::Retreating);
	}
	ActiveSoldiers.Empty();

	Land();
	Super::OnDefeated_Implementation();
}
