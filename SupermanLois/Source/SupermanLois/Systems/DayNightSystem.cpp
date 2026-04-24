#include "DayNightSystem.h"
#include "../Core/SupermanLoisGameMode.h"
#include "../Core/SupermanLoisGameState.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Kismet/GameplayStatics.h"

void UDayNightSystem::Initialize(ASupermanLoisGameMode* GameMode)
{
	OwningGameMode = GameMode;
	CurrentHour    = 8.f; // Start at 8 AM
	UpdateTimeOfDayEnum();
}

void UDayNightSystem::Tick(float DeltaTime)
{
	// Advance time
	const float MinutesPerSecond = TimeScale / 60.f; // game minutes per real second
	CurrentHour += MinutesPerSecond * DeltaTime / 60.f;
	if (CurrentHour >= 24.f) CurrentHour -= 24.f;

	UpdateTimeOfDayEnum();
	UpdateSunPosition();
	UpdateCityLighting();

	// Update game state
	if (OwningGameMode)
	{
		if (ASupermanLoisGameState* GS = Cast<ASupermanLoisGameState>(
			OwningGameMode->GetWorld()->GetGameState()))
		{
			GS->WorldState.TimeOfDay = CurrentHour;
		}
	}
}

void UDayNightSystem::SetTimeOfDay(float Hour)
{
	CurrentHour = FMath::Clamp(Hour, 0.f, 23.99f);
	UpdateTimeOfDayEnum();
	UpdateSunPosition();
}

void UDayNightSystem::UpdateTimeOfDayEnum()
{
	ETimeOfDay NewTOD;

	if      (CurrentHour >= 5.f  && CurrentHour < 7.f)  NewTOD = ETimeOfDay::Dawn;
	else if (CurrentHour >= 7.f  && CurrentHour < 12.f) NewTOD = ETimeOfDay::Morning;
	else if (CurrentHour >= 12.f && CurrentHour < 14.f) NewTOD = ETimeOfDay::Midday;
	else if (CurrentHour >= 14.f && CurrentHour < 18.f) NewTOD = ETimeOfDay::Afternoon;
	else if (CurrentHour >= 18.f && CurrentHour < 20.f) NewTOD = ETimeOfDay::Dusk;
	else                                                  NewTOD = ETimeOfDay::Night;

	if (NewTOD != PreviousTimeOfDay)
	{
		TimeOfDay         = NewTOD;
		PreviousTimeOfDay = NewTOD;
		OnTimeOfDayChanged.Broadcast(NewTOD);
	}
}

void UDayNightSystem::UpdateSunPosition()
{
	// Rotate the directional light (sun) based on hour
	// 6 AM = sunrise (90 deg), 12 PM = overhead (0), 18 PM = sunset (-90)
	const float SunPitch = FMath::GetMappedRangeValueClamped(
		FVector2D(6.f, 18.f), FVector2D(-90.f, 90.f), CurrentHour);

	if (UWorld* World = OwningGameMode ? OwningGameMode->GetWorld() : nullptr)
	{
		TArray<AActor*> Suns;
		UGameplayStatics::GetAllActorsOfClass(World, ADirectionalLight::StaticClass(), Suns);

		for (AActor* Sun : Suns)
		{
			if (Sun->Tags.Contains(FName("SunLight")))
			{
				Sun->SetActorRotation(FRotator(SunPitch, 45.f, 0.f));
			}
		}
	}
}

void UDayNightSystem::UpdateCityLighting()
{
	// City light intensity driven by time — Metropolis glows at night
	// Blueprint actors read GetCityLightIntensity() to drive emissive material params
}

float UDayNightSystem::GetSunAngle() const
{
	return FMath::GetMappedRangeValueClamped(FVector2D(0.f, 24.f), FVector2D(0.f, 360.f), CurrentHour);
}

float UDayNightSystem::GetAmbientBrightness() const
{
	if (CurrentHour >= 6.f && CurrentHour <= 18.f)
	{
		// Bell curve peaking at noon
		const float T = (CurrentHour - 6.f) / 12.f;
		return FMath::Sin(T * PI);
	}
	return 0.05f; // night ambient
}

float UDayNightSystem::GetSolarRegenMultiplier() const
{
	// Superman regens solar energy faster in direct sunlight
	switch (TimeOfDay)
	{
	case ETimeOfDay::Midday:    return 2.0f;
	case ETimeOfDay::Morning:   return 1.5f;
	case ETimeOfDay::Afternoon: return 1.3f;
	case ETimeOfDay::Dawn:
	case ETimeOfDay::Dusk:      return 0.8f;
	case ETimeOfDay::Night:     return 0.3f;
	default:                    return 1.f;
	}
}

float UDayNightSystem::GetCityLightIntensity() const
{
	// At night, city lights are full; during day, off
	if (IsNight()) return 1.f;
	if (TimeOfDay == ETimeOfDay::Dusk) return FMath::GetMappedRangeValueClamped(
		FVector2D(18.f, 20.f), FVector2D(0.f, 1.f), CurrentHour);
	if (TimeOfDay == ETimeOfDay::Dawn) return FMath::GetMappedRangeValueClamped(
		FVector2D(5.f, 7.f), FVector2D(1.f, 0.f), CurrentHour);
	return 0.f;
}
