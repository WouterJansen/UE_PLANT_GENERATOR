#include "AppleActor.h"

AAppleActor::AAppleActor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AAppleActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAppleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAppleActor::GenerateApple(TMap<FString, float> parameters)
{
	
}

