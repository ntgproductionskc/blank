#pragma once

#include "CoreMinimal.h"
#include "BaseVillain.h"
#include "BizarroCharacter.generated.h"

// Bizarro — reversed version of Superman. Freeze vision, fire breath, inverse logic AI.
UCLASS()
class SUPERMANLOIS_API ABizarroCharacter : public ABaseVillain
{
	GENERATED_BODY()

public:
	ABizarroCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	virtual void UseSignatureAbility() override;
	virtual void OnDefeated_Implementation() override;

	// Bizarro has reversed Superman powers
	UFUNCTION(BlueprintCallable, Category="Powers")
	void ActivateFreezeVision();  // instead of heat vision

	UFUNCTION(BlueprintCallable, Category="Powers")
	void ActivateFlameBreath();   // instead of freeze breath

	UFUNCTION(BlueprintCallable, Category="Powers")
	void BizarroRage();           // berserks at < 30% health — attacks recklessly

	// Visual — inverted Superman suit, cracked face
	UPROPERTY(EditDefaultsOnly, Category="Visuals")
	UNiagaraSystem* FreezeVisionNS;

	UPROPERTY(EditDefaultsOnly, Category="Visuals")
	UNiagaraSystem* FlameBreathNS;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UNiagaraComponent* FreezeVisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UNiagaraComponent* FlameBreathComp;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	float RageHealthThreshold = 0.3f;

private:
	bool bIsRaging        = false;
	bool bFreezeVisionOn  = false;
	float AbilityTimer    = 0.f;
	float AbilityCooldown = 6.f;
};
