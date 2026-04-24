#include "BaseVillain.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BrainComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ABaseVillain::ABaseVillain()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));

	// Sight
	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius                  = VillainStats.DetectionRadius;
	SightConfig->LoseSightRadius              = VillainStats.DetectionRadius * 1.5f;
	SightConfig->PeripheralVisionAngleDegrees = 120.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies   = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals  = false;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	PerceptionComp->ConfigureSense(*SightConfig);

	// Hearing
	UAISenseConfig_Hearing* HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = 1200.f;
	PerceptionComp->ConfigureSense(*HearingConfig);

	PerceptionComp->SetDominantSense(UAISenseConfig_Sight::StaticClass());

	GetCharacterMovement()->MaxWalkSpeed = VillainStats.MoveSpeed;
	GetCharacterMovement()->MaxFlySpeed  = VillainStats.FlySpeed;
}

void ABaseVillain::BeginPlay()
{
	Super::BeginPlay();

	VillainStats.CurrentHealth = VillainStats.MaxHealth;

	PerceptionComp->OnPerceptionUpdated.AddDynamic(this, &ABaseVillain::OnPerceptionUpdated);

	// Start behavior tree
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		if (BehaviorTree)
			AIC->RunBehaviorTree(BehaviorTree);
	}
}

void ABaseVillain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ABaseVillain::TakeDamage(float Damage, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	if (VillainStats.bIsInvulnerable) return 0.f;

	const float ReducedDamage = Damage * (1.f - VillainStats.ArmorRating);
	VillainStats.CurrentHealth = FMath::Max(0.f, VillainStats.CurrentHealth - ReducedDamage);

	if (VillainStats.CurrentHealth <= 0.f)
	{
		SetVillainState(EVillainState::Defeated);
		OnDefeated();
	}
	else if (VillainStats.CurrentHealth < VillainStats.MaxHealth * 0.3f)
	{
		// Enter retreating or berserk — override per villain
		SetVillainState(EVillainState::Retreating);
	}

	return ReducedDamage;
}

void ABaseVillain::SetVillainState(EVillainState NewState)
{
	if (VillainState == NewState) return;
	VillainState = NewState;
	OnVillainStateChanged.Broadcast(NewState);
}

void ABaseVillain::StartAttackPhase()
{
	SetVillainState(EVillainState::Attacking);
}

void ABaseVillain::UseSignatureAbility()
{
	// Override in subclasses
}

void ABaseVillain::OnDefeated_Implementation()
{
	SetVillainState(EVillainState::Defeated);
	OnVillainDefeated.Broadcast(this);

	if (AAIController* AIC = Cast<AAIController>(GetController()))
		AIC->StopMovement();

	// Ragdoll
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Despawn after delay
	FTimerHandle DespawnTimer;
	GetWorldTimerManager().SetTimer(DespawnTimer, this, &ABaseVillain::Destroy, 8.f, false);
}

float ABaseVillain::GetHealthPercent() const
{
	return VillainStats.MaxHealth > 0.f ? VillainStats.CurrentHealth / VillainStats.MaxHealth : 0.f;
}

void ABaseVillain::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	for (AActor* Actor : UpdatedActors)
	{
		FActorPerceptionBlueprintInfo Info;
		PerceptionComp->GetActorsPerception(Actor, Info);

		for (const FAIStimulus& Stimulus : Info.LastSensedStimuli)
		{
			if (Stimulus.WasSuccessfullySensed())
			{
				CurrentTarget = Actor;
				SetVillainState(EVillainState::Chasing);
				return;
			}
		}
	}
}

void ABaseVillain::StartFlying()
{
	if (!VillainStats.bCanFly) return;
	bIsFlying = true;
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
}

void ABaseVillain::Land()
{
	bIsFlying = false;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}
