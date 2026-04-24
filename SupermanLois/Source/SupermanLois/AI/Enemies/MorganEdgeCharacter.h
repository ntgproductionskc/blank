#pragma once

#include "CoreMinimal.h"
#include "BaseVillain.h"
#include "MorganEdgeCharacter.generated.h"

// Morgan Edge / Tal-Rho — Kryptonian-empowered, tactical, commands X-Kryptonite soldiers
UCLASS()
class SUPERMANLOIS_API AMorganEdgeCharacter : public ABaseVillain
{
	GENERATED_BODY()

public:
	AMorganEdgeCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	virtual void UseSignatureAbility() override;
	virtual void OnDefeated_Implementation() override;

	UFUNCTION(BlueprintCallable) void SpawnXKryptoniteSoldiers(int32 Count);
	UFUNCTION(BlueprintCallable) void ActivateXKryptoniteField();   // weakens Superman in radius
	UFUNCTION(BlueprintCallable) void TriggerKryptonianBlast();     // ranged energy blast
	UFUNCTION(BlueprintCallable) void CallAirstrike();              // summons military support

	// Config
	UPROPERTY(EditDefaultsOnly, Category="MorganEdge")
	TSubclassOf<ABaseVillain> XKryptoniteSoldierClass;

	UPROPERTY(EditDefaultsOnly, Category="MorganEdge")
	float XKryptoniteFieldRadius  = 800.f;

	UPROPERTY(EditDefaultsOnly, Category="MorganEdge")
	float XKryptoniteStrength     = 2.f;   // exposure per second inside field

	UPROPERTY(EditDefaultsOnly, Category="MorganEdge")
	float BlastDamage             = 200.f;

	UPROPERTY(BlueprintReadOnly)
	TArray<ABaseVillain*> ActiveSoldiers;

private:
	float FieldTimer     = 0.f;
	float BlastTimer     = 0.f;
	float SoldierTimer   = 0.f;
	bool  bFieldActive   = false;
};
