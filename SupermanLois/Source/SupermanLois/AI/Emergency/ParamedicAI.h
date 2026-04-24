#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ParamedicAI.generated.h"

UENUM(BlueprintType)
enum class EParamedicState : uint8
{
	Responding,
	SettingUpTriage,
	TreatingPatient,
	Loading,        // loading patient into ambulance
	Transporting,
	StandingBy
};

UCLASS()
class SUPERMANLOIS_API AParamedicAI : public ACharacter
{
	GENERATED_BODY()

public:
	AParamedicAI();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

public:
	UPROPERTY(BlueprintReadOnly) EParamedicState MedicState = EParamedicState::Responding;
	UPROPERTY(EditDefaultsOnly)  float Health               = 100.f;
	UPROPERTY(EditDefaultsOnly)  float TreatmentRange       = 200.f;
	UPROPERTY(EditDefaultsOnly)  float TreatmentRate        = 25.f;   // HP/s healed

	UPROPERTY(BlueprintReadOnly) AActor* CurrentPatient = nullptr;

	UFUNCTION(BlueprintCallable) void RespondToScene(FVector SceneLocation);
	UFUNCTION(BlueprintCallable) void TreatPatient(AActor* Patient);
	UFUNCTION(BlueprintCallable) void SetupTriage(FVector Center);
	UFUNCTION(BlueprintCallable) void TransportToHospital(AActor* Patient);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPatientTreated, AActor*, Patient);
	UPROPERTY(BlueprintAssignable) FOnPatientTreated OnPatientTreated;

private:
	FVector SceneTarget   = FVector::ZeroVector;
	float   TreatTimer    = 0.f;
	bool    bTriageReady  = false;
};
