#include "BizarroCharacter.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

ABizarroCharacter::ABizarroCharacter()
{
	VillainType = EVillainType::Bizarro;

	VillainStats.MaxHealth    = 1800.f;
	VillainStats.CurrentHealth = 1800.f;
	VillainStats.AttackDamage = 120.f;
	VillainStats.ArmorRating  = 0.6f;
	VillainStats.bCanFly      = true;
	VillainStats.FlySpeed     = 1600.f;

	FreezeVisionComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FreezeVisionComp"));
	FreezeVisionComp->SetupAttachment(GetMesh(), TEXT("EyeSocket_L"));
	FreezeVisionComp->SetAutoActivate(false);

	FlameBreathComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FlameBreathComp"));
	FlameBreathComp->SetupAttachment(GetMesh(), TEXT("MouthSocket"));
	FlameBreathComp->SetAutoActivate(false);
}

void ABizarroCharacter::BeginPlay()
{
	Super::BeginPlay();
	StartFlying(); // Bizarro prefers aerial combat
}

void ABizarroCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AbilityTimer -= DeltaTime;

	if (AbilityTimer <= 0.f && CurrentTarget && VillainState == EVillainState::Attacking)
	{
		UseSignatureAbility();
		AbilityTimer = AbilityCooldown;
	}

	// Trigger rage phase
	if (!bIsRaging && GetHealthPercent() < RageHealthThreshold)
		BizarroRage();
}

void ABizarroCharacter::UseSignatureAbility()
{
	// Alternate between freeze vision and flame breath
	if (!bFreezeVisionOn)
		ActivateFreezeVision();
	else
		ActivateFlameBreath();

	bFreezeVisionOn = !bFreezeVisionOn;
}

void ABizarroCharacter::ActivateFreezeVision()
{
	if (!CurrentTarget) return;

	FreezeVisionComp->Activate(true);

	// Raycast from eyes toward Superman
	FHitResult Hit;
	const FVector Start = GetMesh()->GetSocketLocation(TEXT("EyeSocket_L"));
	const FVector End   = CurrentTarget->GetActorLocation();

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, Params))
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			// Freeze the target
			HitActor->Tags.AddUnique(FName("Frozen"));

			UGameplayStatics::ApplyPointDamage(
				HitActor, 60.f * (bIsRaging ? 2.f : 1.f),
				(End - Start).GetSafeNormal(), Hit,
				GetController(), this, UDamageType::StaticClass());
		}
	}

	// Deactivate after brief burst
	FTimerHandle StopTimer;
	GetWorldTimerManager().SetTimer(StopTimer, [this]()
	{
		FreezeVisionComp->Deactivate();
	}, 1.5f, false);
}

void ABizarroCharacter::ActivateFlameBreath()
{
	if (!CurrentTarget) return;

	FlameBreathComp->Activate(true);

	// Wide cone fire damage
	TArray<FOverlapResult> Overlaps;
	FCollisionShape Cone = FCollisionShape::MakeSphere(700.f);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->OverlapMultiByChannel(
		Overlaps, GetActorLocation() + GetActorForwardVector() * 350.f,
		FQuat::Identity, ECC_Pawn, Cone, Params))
	{
		for (const FOverlapResult& Overlap : Overlaps)
		{
			if (AActor* Target = Overlap.GetActor())
			{
				UGameplayStatics::ApplyDamage(
					Target, 80.f * (bIsRaging ? 2.f : 1.f),
					GetController(), this, UDamageType::StaticClass());

				// Remove frozen tag — fire thaws
				Target->Tags.Remove(FName("Frozen"));
			}
		}
	}

	FTimerHandle StopTimer;
	GetWorldTimerManager().SetTimer(StopTimer, [this]()
	{
		FlameBreathComp->Deactivate();
	}, 2.f, false);
}

void ABizarroCharacter::BizarroRage()
{
	bIsRaging = true;
	AbilityCooldown = 2.f; // much faster ability use
	VillainStats.AttackDamage *= 1.8f;
	GetCharacterMovement()->MaxFlySpeed *= 1.5f;
	SetVillainState(EVillainState::Attacking);
}

void ABizarroCharacter::OnDefeated_Implementation()
{
	FreezeVisionComp->Deactivate();
	FlameBreathComp->Deactivate();

	// Dramatic collapse — Bizarro lands and kneels before falling
	Land();
	Super::OnDefeated_Implementation();
}
