#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "BaseVillain.generated.h"

class UBehaviorTree;
class UAIPerceptionComponent;
class UNiagaraComponent;

UENUM(BlueprintType)
enum class EVillainState : uint8
{
	Idle,
	Patrolling,
	Chasing,
	Attacking,
	UsingAbility,
	Stunned,
	Retreating,
	Defeated
};

UENUM(BlueprintType)
enum class EVillainType : uint8
{
	Bizarro,
	Doomsday,
	MorganEdge,
	InverseSuperman,
	MetaHuman,
	SmallvilleMilitary,
	Subjekt11,
	Tal_Rho,
	Zod
};

USTRUCT(BlueprintType)
struct FVillainStats
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) float MaxHealth        = 500.f;
	UPROPERTY(BlueprintReadOnly)                    float CurrentHealth    = 500.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) float AttackDamage     = 75.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) float AttackRange      = 250.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) float DetectionRadius  = 3000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) float MoveSpeed        = 600.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) float FlySpeed         = 1800.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) bool  bCanFly          = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) bool  bIsInvulnerable  = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) float ArmorRating      = 0.f;   // damage reduction 0-1
};

UCLASS(Abstract)
class SUPERMANLOIS_API ABaseVillain : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseVillain();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

public:
	// ── Config ─────────────────────────────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, Category="AI")
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	EVillainType VillainType = EVillainType::MetaHuman;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stats")
	FVillainStats VillainStats;

	UPROPERTY(EditDefaultsOnly, Category="Patrol")
	TArray<AActor*> PatrolPoints;

	// ── Perception ─────────────────────────────────────────────────────────────
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	UAIPerceptionComponent* PerceptionComp;

	// ── State ─────────────────────────────────────────────────────────────────
	UPROPERTY(BlueprintReadOnly, Category="State")
	EVillainState VillainState = EVillainState::Idle;

	UPROPERTY(BlueprintReadOnly, Category="State")
	AActor* CurrentTarget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="State")
	bool bIsFlying = false;

	// ── Interface ─────────────────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category="AI")
	void SetVillainState(EVillainState NewState);

	UFUNCTION(BlueprintCallable, Category="Combat")
	virtual void StartAttackPhase();

	UFUNCTION(BlueprintCallable, Category="Combat")
	virtual void UseSignatureAbility();

	UFUNCTION(BlueprintNativeEvent, Category="Combat")
	void OnDefeated();
	virtual void OnDefeated_Implementation();

	UFUNCTION(BlueprintPure, Category="Stats")
	float GetHealthPercent() const;

	// ── Delegates ─────────────────────────────────────────────────────────────
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVillainStateChanged, EVillainState, NewState);
	UPROPERTY(BlueprintAssignable) FOnVillainStateChanged OnVillainStateChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVillainDefeated, ABaseVillain*, Villain);
	UPROPERTY(BlueprintAssignable) FOnVillainDefeated OnVillainDefeated;

protected:
	UFUNCTION()
	virtual void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	void StartFlying();
	void Land();

private:
	FTimerHandle AbilityCooldownHandle;
	bool bAbilityOnCooldown = false;
};
