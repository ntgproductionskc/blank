#include "InverseSupermanCharacter.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

AInverseSupermanCharacter::AInverseSupermanCharacter()
{
	VillainType = EVillainType::InverseSuperman;

	VillainStats.MaxHealth    = 3000.f;
	VillainStats.CurrentHealth = 3000.f;
	VillainStats.AttackDamage = 175.f;
	VillainStats.ArmorRating  = 0.55f;
	VillainStats.bCanFly      = true;
	VillainStats.FlySpeed     = 2200.f;
	VillainStats.DetectionRadius = 6000.f;

	InverseHeatVisionComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("InverseHeatVision"));
	InverseHeatVisionComp->SetupAttachment(GetMesh(), TEXT("EyeSocket_L"));
	InverseHeatVisionComp->SetAutoActivate(false);

	SolarFlareComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SolarFlare"));
	SolarFlareComp->SetupAttachment(RootComponent);
	SolarFlareComp->SetAutoActivate(false);
}

void AInverseSupermanCharacter::BeginPlay()
{
	Super::BeginPlay();
	StartFlying();
}

void AInverseSupermanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (VillainState != EVillainState::Attacking) return;

	SolarFlareTimer    -= DeltaTime;
	AbilityRotateTimer -= DeltaTime;

	// Cycle abilities every few seconds
	if (AbilityRotateTimer <= 0.f)
	{
		UseSignatureAbility();
		AbilityRotateTimer = 4.f;
	}

	// Solar flare as desperation move at low health
	if (SolarFlareTimer <= 0.f && GetHealthPercent() < 0.4f)
	{
		UseSolarFlare();
		SolarFlareTimer = SolarFlareCooldown;
	}
}

void AInverseSupermanCharacter::StartAttackPhase()
{
	Super::StartAttackPhase();
	bIsMirroringPlayer = true;
	SelectNextAbility();
}

void AInverseSupermanCharacter::UseSignatureAbility()
{
	SelectNextAbility();
}

void AInverseSupermanCharacter::SelectNextAbility()
{
	switch (CurrentAbilityIdx % 3)
	{
	case 0: UseInverseHeatVision();  break;
	case 1: UseInverseFreezeBreath(); break;
	case 2: UseInverseFlightCharge(); break;
	}
	CurrentAbilityIdx++;
}

void AInverseSupermanCharacter::UseInverseHeatVision()
{
	if (!CurrentTarget) return;

	InverseHeatVisionComp->Activate(true);

	// Twin beams — trace toward Superman
	for (const FName& Socket : TArray<FName>{ TEXT("EyeSocket_L"), TEXT("EyeSocket_R") })
	{
		FHitResult Hit;
		const FVector Start = GetMesh()->GetSocketLocation(Socket);
		const FVector End   = CurrentTarget->GetActorLocation();
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, Params))
		{
			if (AActor* HitActor = Hit.GetActor())
				UGameplayStatics::ApplyPointDamage(HitActor, 80.f, (End-Start).GetSafeNormal(),
					Hit, GetController(), this, UDamageType::StaticClass());
		}
	}

	FTimerHandle T;
	GetWorldTimerManager().SetTimer(T, [this]() { InverseHeatVisionComp->Deactivate(); }, 2.f, false);
}

void AInverseSupermanCharacter::UseInverseFreezeBreath()
{
	if (!CurrentTarget) return;

	TArray<FOverlapResult> Overlaps;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(500.f);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->OverlapMultiByChannel(Overlaps,
		GetActorLocation() + GetActorForwardVector() * 250.f,
		FQuat::Identity, ECC_Pawn, Sphere, Params))
	{
		for (const FOverlapResult& Overlap : Overlaps)
		{
			if (AActor* Target = Overlap.GetActor())
			{
				Target->Tags.AddUnique(FName("Frozen"));
				UGameplayStatics::ApplyDamage(Target, 90.f, GetController(), this,
					UDamageType::StaticClass());
			}
		}
	}
}

void AInverseSupermanCharacter::UseInverseFlightCharge()
{
	if (!CurrentTarget) return;

	const FVector Dir = (CurrentTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	GetCharacterMovement()->Velocity = Dir * VillainStats.FlySpeed * 2.f;

	// Brief invulnerability during charge
	VillainStats.bIsInvulnerable = true;
	FTimerHandle T;
	GetWorldTimerManager().SetTimer(T, [this]() { VillainStats.bIsInvulnerable = false; }, 0.6f, false);
}

void AInverseSupermanCharacter::UseSolarFlare()
{
	SolarFlareComp->Activate(true);
	SetVillainState(EVillainState::UsingAbility);

	// Brief wind-up
	FTimerHandle WindupTimer;
	GetWorldTimerManager().SetTimer(WindupTimer, [this]()
	{
		UGameplayStatics::ApplyRadialDamage(
			GetWorld(), SolarFlareDamage, GetActorLocation(), SolarFlareRadius,
			UDamageType::StaticClass(), {this}, this, GetController(), true);

		SolarFlareComp->Deactivate();
		SetVillainState(EVillainState::Attacking);
	}, 2.f, false);
}

void AInverseSupermanCharacter::CounterSupermanMove()
{
	// Triggered by AnimNotify when player attacks — dodge and counter
	Dodge();
	FTimerHandle CounterTimer;
	GetWorldTimerManager().SetTimer(CounterTimer, [this]() { StartAttackPhase(); }, 0.4f, false);
}

void AInverseSupermanCharacter::OnDefeated_Implementation()
{
	InverseHeatVisionComp->Deactivate();
	SolarFlareComp->Deactivate();
	Land();
	Super::OnDefeated_Implementation();
}
