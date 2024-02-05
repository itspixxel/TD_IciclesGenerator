#include "IciclesGenerator.h"
#include "Components/SphereComponent.h"

AIciclesGenerator::AIciclesGenerator()
{
	RadiusVisualizer = CreateDefaultSubobject<USphereComponent>(TEXT("Radius Visualizer"));
	RadiusVisualizer->SetupAttachment(RootComponent);
	RadiusVisualizer->InitSphereRadius(SphereRadius);
}

void AIciclesGenerator::Process(UDynamicMeshComponent* DynaMesh)
{
	// Allocate a temporary dynamic mesh to work on
	TempDynamicMesh = AllocateComputeMesh();
	
	// Initialize a deterministic RNG
	FRandomStream RandomStream(RandomSeed);

	// Save the transforms for later

}

void AIciclesGenerator::OnRebuildGeneratedMesh(UDynamicMesh* TargetMesh)
{
}
