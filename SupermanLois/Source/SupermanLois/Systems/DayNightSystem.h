#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DayNightSystem.generated.h"

class ASupermanLoisGameMode;
class ADirectionalLight;
class ASkyLight;

UENUM(BlueprintType)
enum class ETimeOfDay : uint8
{
	Dawn,       // 5–7
	Morning,    // 7–12
	Midday,     // 12–14
	Afternoon,  // 14–18
	Dusk,       // 18–20
	Night,      // 20–5
};

UCLASS()
class SUPERMANLOIS_API UDayNightSystem : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(ASupermanLoisGameMode* GameMode);
	void Tick(float DeltaTime);

	// 1 real second = TimeScale in-game minutes (default 1 min real = 10 min game)
	UPROPERTY(BlueprintReadWrite) float TimeScale        = 10.f;
	UPROPERTY(BlueprintReadOnly)  float CurrentHour      = 8.f;  // 0–24
	UPROPERTY(BlueprintReadOnly)  ETimeOfDay TimeOfDay   = ETimeOfDay::Morning;

	UFUNCTION(BlueprintCallable) void SetTimeOfDay(float Hour);
	UFUNCTION(BlueprintPure)     float GetSunAngle() const;      // 0–360 degrees
	UFUNCTION(BlueprintPure)     float GetAmbientBrightness() const;
	UFUNCTION(BlueprintPure)     bool  IsNight() const { return TimeOfDay == ETimeOfDay::Night; }

	// Superman solar power regen is faster during midday
	UFUNCTION(BlueprintPure)     float GetSolarRegenMultiplier() const;

	// Metropolis lights up at night — handled via material parameters
	UFUNCTION(BlueprintPure)     float GetCityLightIntensity() const;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnTimeOfDayChanged, ETimeOfDay);
	FOnTimeOfDayChanged OnTimeOfDayChanged;

private:
	UPROPERTY()
	ASupermanLoisGameMode* OwningGameMode;

	ETimeOfDay PreviousTimeOfDay = ETimeOfDay::Morning;

	void UpdateTimeOfDayEnum();
	void UpdateSunPosition();
	void UpdateCityLighting();
};
