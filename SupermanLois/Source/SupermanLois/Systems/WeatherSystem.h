#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WeatherSystem.generated.h"

class ASupermanLoisGameMode;
class ADirectionalLight;
class ASkyAtmosphere;
class AVolumetricCloud;
class UExponentialHeightFogComponent;

UENUM(BlueprintType)
enum class EWeatherState : uint8
{
	Clear,
	Overcast,
	LightRain,
	HeavyRain,
	Thunderstorm,
	Snow,          // Smallville winter
	Tornado,       // Smallville event
	AcidRain       // Inverse World contamination event
};

UCLASS()
class SUPERMANLOIS_API UWeatherSystem : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(ASupermanLoisGameMode* GameMode);
	void Tick(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category="Weather")
	void SetWeather(EWeatherState NewWeather, float TransitionDuration = 30.f);

	UFUNCTION(BlueprintPure, Category="Weather")
	EWeatherState GetCurrentWeather() const { return CurrentWeather; }

	UFUNCTION(BlueprintPure, Category="Weather")
	float GetRainIntensity() const { return RainIntensity; }

	UFUNCTION(BlueprintPure, Category="Weather")
	bool IsStorming() const { return CurrentWeather == EWeatherState::Thunderstorm; }

	// Dynamic weather can affect gameplay
	UFUNCTION(BlueprintPure, Category="Weather")
	float GetFlightSpeedModifier() const; // wind resistance in storms

	UFUNCTION(BlueprintPure, Category="Weather")
	float GetVisibilityRange() const;

	// Delegates
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnWeatherChanged, EWeatherState);
	FOnWeatherChanged OnWeatherChanged;

	DECLARE_MULTICAST_DELEGATE(FOnLightningStrike);
	FOnLightningStrike OnLightningStrike;

private:
	UPROPERTY()
	ASupermanLoisGameMode* OwningGameMode;

	EWeatherState CurrentWeather     = EWeatherState::Clear;
	EWeatherState TargetWeather      = EWeatherState::Clear;
	float         TransitionProgress = 0.f;
	float         TransitionRate     = 0.f;
	float         RainIntensity      = 0.f;
	float         WindStrength       = 0.f;
	float         LightningTimer     = 0.f;
	float         RandomWeatherTimer = 0.f;

	void InterpolateWeatherParameters(float DeltaTime);
	void TriggerLightning();
	void SpawnRandomWeatherEvent();
	void ApplyWeatherToAtmosphere();

	// Target parameter values per weather state
	struct FWeatherParams
	{
		float RainIntensity;
		float WindStrength;
		float FogDensity;
		float CloudCoverage;
		float AmbientBrightness;
	};

	FWeatherParams GetParamsForWeather(EWeatherState State) const;
	FWeatherParams CurrentParams;
	FWeatherParams TargetParams;
};
