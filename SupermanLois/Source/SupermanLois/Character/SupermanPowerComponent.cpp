#include "SupermanPowerComponent.h"
#include "SupermanCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"

USupermanPowerComponent::USupermanPowerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USupermanPowerComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerSuperman = Cast<ASupermanCharacter>(GetOwner());
}

void USupermanPowerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Combo timeout
	if (ComboCount > 0 && !bAttacking)
	{
		ComboResetTimer -= DeltaTime;
		if (ComboResetTimer <= 0.f) ResetCombo();
	}
}

void USupermanPowerComponent::TriggerLightAttack()
{
	if (!OwnerSuperman || bAttacking) return;

	bAttacking = true;
	ComboCount = FMath::Min(ComboCount + 1, MaxCombo);
	ComboResetTimer = ComboWindow;

	UAnimMontage* Montage = OwnerSuperman->GetMovementComponent()->IsFlying()
		? LightAttackMontage_Air : LightAttackMontage_Ground;

	if (Montage)
		OwnerSuperman->PlayAnimMontage(Montage);

	// Hit detection fires via AnimNotify on the montage
	// Fallback timer if no montage assigned
	FTimerHandle AttackTimer;
	OwnerSuperman->GetWorldTimerManager().SetTimer(AttackTimer, [this]()
	{
		PerformAttackHitDetection(false);
		bAttacking = false;
	}, 0.3f, false);
}

void USupermanPowerComponent::TriggerHeavyAttack()
{
	if (!OwnerSuperman || bAttacking) return;

	bAttacking  = true;
	ComboCount  = 0; // heavy resets combo
	ComboResetTimer = ComboWindow;

	UAnimMontage* Montage = OwnerSuperman->GetMovementComponent()->IsFlying()
		? HeavyAttackMontage_Air : HeavyAttackMontage_Ground;

	if (Montage)
		OwnerSuperman->PlayAnimMontage(Montage);

	FTimerHandle AttackTimer;
	OwnerSuperman->GetWorldTimerManager().SetTimer(AttackTimer, [this]()
	{
		PerformAttackHitDetection(true);
		bAttacking = false;
	}, 0.5f, false);
}

void USupermanPowerComponent::PerformAttackHitDetection(bool bIsHeavy)
{
	if (!OwnerSuperman) return;

	TArray<FOverlapResult> Overlaps;
	const FVector Origin = OwnerSuperman->GetActorLocation()
		+ OwnerSuperman->GetActorForwardVector() * (AttackRange * 0.5f);

	FCollisionShape Sphere = FCollisionShape::MakeSphere(AttackRadius);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerSuperman);

	if (OwnerSuperman->GetWorld()->OverlapMultiByChannel(
		Overlaps, Origin, FQuat::Identity, ECC_Pawn, Sphere, Params))
	{
		const float StrengthMult = OwnerSuperman->Stats.PhysicalStrength;
		const float KrypMult     = 1.f - (OwnerSuperman->Stats.KryptoniteExposure / 100.f);
		const float ComboMult    = 1.f + (ComboCount * 0.15f);
		const float BaseDamage   = bIsHeavy ? HeavyAttackDamage : LightAttackDamage;
		const float FinalDamage  = BaseDamage * StrengthMult * KrypMult * ComboMult;

		for (const FOverlapResult& Overlap : Overlaps)
		{
			if (AActor* Target = Overlap.GetActor())
			{
				FPointDamageEvent DmgEvent(FinalDamage,
					FHitResult(), OwnerSuperman->GetActorForwardVector(), nullptr);

				Target->TakeDamage(FinalDamage, DmgEvent,
					OwnerSuperman->GetController(), OwnerSuperman);

				// Launch enemy on heavy hit
				if (bIsHeavy)
				{
					if (ACharacter* EnemyChar = Cast<ACharacter>(Target))
					{
						const FVector LaunchDir = (Target->GetActorLocation()
							- OwnerSuperman->GetActorLocation()).GetSafeNormal();
						EnemyChar->LaunchCharacter(LaunchDir * 3000.f + FVector(0,0,1000.f), true, true);
					}
				}

				OnComboHit.Broadcast(ComboCount, FinalDamage);
			}
		}
	}
}

void USupermanPowerComponent::TriggerFinisher(AActor* Target)
{
	if (!OwnerSuperman || !Target || !FinisherMontage) return;

	OwnerSuperman->PlayAnimMontage(FinisherMontage);
	// Cinematic camera cuts handled in Blueprint/Sequencer
}

void USupermanPowerComponent::ResetCombo()
{
	ComboCount = 0;
	OnComboReset.Broadcast();
}
