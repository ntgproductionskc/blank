#include "WeatherSystem.h"
#include "../Core/SupermanLoisGameMode.h"
#include "../Core/SupermanLoisGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DirectionalLight.h"

void UWeatherSystem::Initialize(ASupermanLoisGameMode* GameMode)
{
	OwningGameMode    = GameMode;
	CurrentWeather    = EWeatherState::Clear;
	CurrentParams     = GetParamsForWeather(EWeatherState::Clear);
	RandomWeatherTimer = FMath::RandRange(120.f, 300.f);
}

void UWeatherSystem::Tick(float DeltaTime)
{
	InterpolateWeatherParameters(DeltaTime);

	RandomWeatherTimer -= DeltaTime;
	if (RandomWeatherTimer <= 0.f)
	{
		SpawnRandomWeatherEvent();
		RandomWeatherTimer = FMath::RandRange(120.f, 300.f);
	}

	// Lightning during thunderstorm
	if (CurrentWeather == EWeatherState::Thunderstorm)
	{
		LightningTimer -= DeltaTime;
		if (LightningTimer <= 0.f)
		{
			TriggerLightning();
			LightningTimer = FMath::RandRange(3.f, 12.f);
		}
	}

	// Update game state
	if (OwningGameMode)
	{
		if (ASupermanLoisGameState* GS = Cast<ASupermanLoisGameState>(
			OwningGameMode->GetWorld()->GetGameState()))
		{
			GS->WorldState.bIsRaining = (RainIntensity > 0.1f);
			GS->WorldState.bIsStorming = IsStorming();
			GS->WorldState.WeatherIntensity = RainIntensity;
		}
	}
}

void UWeatherSystem::SetWeather(EWeatherState NewWeather, float TransitionDuration)
{
	if (CurrentWeather == NewWeather) return;

	TargetWeather  = NewWeather;
	TargetParams   = GetParamsForWeather(NewWeather);
	TransitionRate = (TransitionDuration > 0.f) ? 1.f / TransitionDuration : 1.f;

	OnWeatherChanged.Broadcast(NewWeather);
}

void UWeatherSystem::InterpolateWeatherParameters(float DeltaTime)
{
	if (TransitionProgress < 1.f)
	{
		TransitionProgress = FMath::Min(1.f, TransitionProgress + TransitionRate * DeltaTime);
		const float Alpha  = TransitionProgress;

		CurrentParams.RainIntensity    = FMath::Lerp(CurrentParams.RainIntensity,   TargetParams.RainIntensity,   Alpha);
		CurrentParams.WindStrength     = FMath::Lerp(CurrentParams.WindStrength,    TargetParams.WindStrength,    Alpha);
		CurrentParams.FogDensity       = FMath::Lerp(CurrentParams.FogDensity,      TargetParams.FogDensity,      Alpha);
		CurrentParams.CloudCoverage    = FMath::Lerp(CurrentParams.CloudCoverage,   TargetParams.CloudCoverage,   Alpha);
		CurrentParams.AmbientBrightness = FMath::Lerp(CurrentParams.AmbientBrightness, TargetParams.AmbientBrightness, Alpha);

		RainIntensity = CurrentParams.RainIntensity;
		WindStrength  = CurrentParams.WindStrength;

		if (TransitionProgress >= 1.f)
		{
			CurrentWeather     = TargetWeather;
			TransitionProgress = 0.f;
			ApplyWeatherToAtmosphere();
		}
	}
}

void UWeatherSystem::TriggerLightning()
{
	// Find random position in Metropolis and spawn lightning
	const FVector LightningPos = FVector(
		FMath::RandRange(-30000.f, 30000.f),
		FMath::RandRange(-30000.f, 30000.f),
		FMath::RandRange(5000.f, 15000.f));

	// Trigger AoE damage at strike point
	if (OwningGameMode)
	{
		UGameplayStatics::ApplyRadialDamage(
			OwningGameMode->GetWorld(), 300.f, LightningPos,
			200.f, UDamageType::StaticClass(), {}, nullptr, nullptr, true);
	}

	OnLightningStrike.Broadcast();
}

void UWeatherSystem::SpawnRandomWeatherEvent()
{
	const float ThreatLevel = OwningGameMode ? OwningGameMode->CityThreatLevel : 0.f;

	EWeatherState NewWeather;
	const float Roll = FMath::FRand();

	if (ThreatLevel > 0.7f && Roll < 0.3f)
		NewWeather = EWeatherState::Thunderstorm;
	else if (Roll < 0.4f)
		NewWeather = EWeatherState::Clear;
	else if (Roll < 0.6f)
		NewWeather = EWeatherState::Overcast;
	else if (Roll < 0.75f)
		NewWeather = EWeatherState::LightRain;
	else if (Roll < 0.88f)
		NewWeather = EWeatherState::HeavyRain;
	else
		NewWeather = EWeatherState::Thunderstorm;

	SetWeather(NewWeather, 60.f);
}

void UWeatherSystem::ApplyWeatherToAtmosphere()
{
	// In production: find VolumetricCloud actor, set cloud coverage
	// find ExponentialHeightFog, set fog density
	// All driven by CurrentParams — Blueprints read these values via GetRainIntensity() etc.
}

float UWeatherSystem::GetFlightSpeedModifier() const
{
	switch (CurrentWeather)
	{
	case EWeatherState::Thunderstorm: return 0.75f;
	case EWeatherState::HeavyRain:    return 0.85f;
	case EWeatherState::Tornado:      return 0.6f;
	default:                          return 1.f;
	}
}

float UWeatherSystem::GetVisibilityRange() const
{
	switch (CurrentWeather)
	{
	case EWeatherState::Thunderstorm: return 15000.f;
	case EWeatherState::HeavyRain:    return 20000.f;
	case EWeatherState::Snow:         return 12000.f;
	case EWeatherState::AcidRain:     return 8000.f;
	default:                          return 80000.f;
	}
}

UWeatherSystem::FWeatherParams UWeatherSystem::GetParamsForWeather(EWeatherState State) const
{
	FWeatherParams P;
	switch (State)
	{
	case EWeatherState::Clear:
		P = { 0.f,   0.f,   0.001f, 0.1f, 1.f };   break;
	case EWeatherState::Overcast:
		P = { 0.f,   0.1f,  0.01f,  0.7f, 0.7f };  break;
	case EWeatherState::LightRain:
		P = { 0.3f,  0.2f,  0.02f,  0.8f, 0.6f };  break;
	case EWeatherState::HeavyRain:
		P = { 0.7f,  0.4f,  0.05f,  0.9f, 0.45f }; break;
	case EWeatherState::Thunderstorm:
		P = { 1.f,   0.8f,  0.08f,  1.f,  0.3f };  break;
	case EWeatherState::Snow:
		P = { 0.5f,  0.15f, 0.04f,  0.85f,0.55f }; break;
	case EWeatherState::Tornado:
		P = { 0.8f,  1.f,   0.1f,   1.f,  0.25f }; break;
	case EWeatherState::AcidRain:
		P = { 0.6f,  0.3f,  0.15f,  0.95f,0.2f };  break;
	default:
		P = { 0.f, 0.f, 0.001f, 0.1f, 1.f };
	}
	return P;
}
