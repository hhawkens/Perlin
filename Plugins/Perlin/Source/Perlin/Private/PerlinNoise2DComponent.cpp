#include "PerlinNoise2DComponent.h"

UPerlinNoise2DComponent::UPerlinNoise2DComponent()
	: Max(1.f)
	, Min(0.f)
	, StartingOctave(0)
	, OctaveCount(4)
	, BaseAmplitude(0.5f)
	, bContinuousCurve(true)
	, bShowDebugCurve(false)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPerlinNoise2DComponent::BeginPlay()
{
	if (bShowDebugCurve)
	{
		auto TempPerlin = FPerlinNoise2D(OctaveCount, StartingOctave, Min, Max);
		TempPerlin.SetBaseAmplitude(BaseAmplitude);
		TempPerlin.SetContinuousCurve(bContinuousCurve);
		// TODO draw debug line
	}
}

void UPerlinNoise2DComponent::AddPerlinInstances(int32 Amount)
{
	for (auto i = 0; i < Amount; i++)
	{
		const auto NewPerlinPtr = MakeShareable(new FPerlinNoise2D);
		FPerlinNoise2D* NewPerlin = NewPerlinPtr.Object;
		NewPerlin->SetMax(Max);
		NewPerlin->SetMin(Min);
		NewPerlin->SetStartingOctave(StartingOctave);
		NewPerlin->SetOctaveCount(OctaveCount);
		NewPerlin->SetBaseAmplitude(BaseAmplitude);
		NewPerlin->SetContinuousCurve(bContinuousCurve);

		PerlinInstances.Add(NewPerlinPtr);
	}
}

void UPerlinNoise2DComponent::RemovePerlinInstance(int32 Amount, int32 Index, bool bRemoveAll)
{
	if (bRemoveAll || Amount >= PerlinInstances.Num() || PerlinInstances.Num() == 0)
	{
		PerlinInstances.Empty();
	}
	else
	{
		Amount = FMath::Max(0, Amount);
		Index = FMath::Clamp(Index, 0, PerlinInstances.Num() - 1);
		PerlinInstances.RemoveAt(Index, Amount);
	}
}

FPerlinNoise2D* UPerlinNoise2DComponent::GetPerlinAt(int32 Index)
{
	Index = FMath::Max(0, Index);
	return Index >= PerlinInstances.Num() ? nullptr : &PerlinInstances[Index].Get();
}
