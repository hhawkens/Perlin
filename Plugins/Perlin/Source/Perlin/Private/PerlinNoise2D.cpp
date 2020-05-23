#include "PerlinNoise2D.h"

FPerlinNoise2D::FPerlinNoise2D(uint8 OctaveCount, uint8 StartingOctave, float Min, float Max)
	: BaseAmplitude(0.5f)
	, bContinuousCurve(true)
	, CurrentTime(100.f)
{
	SetOctaveCount(OctaveCount);
	SetStartingOctave(StartingOctave);
	SetMin(Min);
	SetMax(Max);
}

void FPerlinNoise2D::GenerateOctaves()
{
	const TArray<float> PreviousOctavesLastValues = GetOctavesLastValues();
	auto PrevValuesIt = PreviousOctavesLastValues.CreateConstIterator();
	Octaves.Empty();

	float Amplitude;
	uint8 OctaveCounter;
	const float MinMaxMiddle = (Max + Min) / 2.f;

	for (OctaveCounter = StartingOctave, Amplitude = BaseAmplitude; OctaveCounter < StartingOctave + OctaveCount; OctaveCounter++)
	{
		FOctave NewOctave;

		const float NewAmplitude = Amplitude;
		Amplitude *= BaseAmplitude;

		const uint8 IntervalCount = OctaveCounter + 2;
		const float Interval = 1.f / (OctaveCounter + 1);
		float NewValue = FMath::FRandRange(Min, Max);


		for (uint8 p = 0; p < IntervalCount; p++)
		{
			if (bContinuousCurve && p == 0 && PrevValuesIt) // Set first values of each octave to match end of last perlin curve
			{
				NewValue = *PrevValuesIt;
				++PrevValuesIt;
			}
			else
			{
				NewValue = NewValue < MinMaxMiddle ? FMath::FRandRange(MinMaxMiddle, Max) : FMath::FRandRange(Min, MinMaxMiddle);
				NewValue *= NewAmplitude;
			}

			NewOctave.Add(p * Interval, NewValue);
		}

		Octaves.Add(NewOctave);
	}
}

float FPerlinNoise2D::GetPerlinValue(float Time, bool bAdvanceTime)
{
	Time = FMath::Max(Time, 0.f);

	if (CurrentTime > 1.f || Octaves.Num() <= 0)
	{
		GenerateOctaves();
		CurrentTime = 0.f;
	}

	if (bAdvanceTime)
	{
		CurrentTime += Time;
	}

	float RetVal = 0.f;
	float IntervalHit = Max + 1.f;
	const float ClampedTime = FMath::Clamp(CurrentTime, 0.f, 1.f);

	for (auto& Octave : Octaves)
	{
		TPair<float, float> Floor = TPairInitializer<float, float>(0.f, Octave[0.f]);
		TPair<float, float> Ceiling = TPairInitializer<float, float>(1.f, Octave[1.f]);


		for (auto& OctavePoint : Octave)
		{
			const auto& OctaveTime = OctavePoint.Key;

			if (OctaveTime > ClampedTime)
			{
				Ceiling = OctavePoint;
				break;
			}
			else if (OctaveTime < ClampedTime)
			{
				Floor = OctavePoint;
			}
			else if (OctaveTime == ClampedTime)
			{
				IntervalHit = OctavePoint.Value;
			}
		}

		const float FloorTime = Floor.Key;
		const float CeilingTime = Ceiling.Key;

		// Get the two closest interval points in time before(floor) and after(ceiling) "CurrentTime" on the curve, 
		// then interpolate between those points with a cubic function to get the value of the exact point in time
		RetVal += IntervalHit != Max + 1.f ?
			IntervalHit :
			FMath::CubicInterp(Floor.Value, 0.f, Ceiling.Value, 0.f, (ClampedTime - FloorTime) / (CeilingTime - FloorTime));
	}

	return FMath::Clamp(RetVal, Min, Max);
}

TArray<float> FPerlinNoise2D::GetOctavesLastValues() const
{
	TArray<float> RetArray;

	for (auto& Octave : Octaves)
	{
		RetArray.Add(Octave[1.f]);
	}

	return RetArray;
}
