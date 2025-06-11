#include "AppleActor.h"
#include "Util.h"

AAppleActor::AAppleActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Apple = this->GetStaticMeshComponent();
	Apple->SetMobility(EComponentMobility::Movable);

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
	UStaticMesh* RandomAppleMesh = Util::GetRandomMeshFromFolder(TEXT("/PLANT_GENERATOR/Apple/"));
	Apple->SetStaticMesh(RandomAppleMesh);
	
}

