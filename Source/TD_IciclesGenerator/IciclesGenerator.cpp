#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "IciclesGenerator.h"

AIciclesGenerator::AIciclesGenerator()
{
	RadiusVisualizer = CreateDefaultSubobject<USphereComponent>(TEXT("Radius Visualizer"));
	RadiusVisualizer->SetupAttachment(RootComponent);
	RadiusVisualizer->InitSphereRadius(SphereRadius);
}

void AIciclesGenerator::Process(UDynamicMesh* DynaMesh, FGeometryScriptMeshSelection TrianglesFacingDown)
{
	// Allocate a temporary dynamic mesh to work on
	TempDynamicMesh = AllocateComputeMesh();
	
	// Initialize a deterministic RNG
	FRandomStream RandomStream(RandomSeed);

	// Save the transforms for later
    // Get a reference to the UDynamicMeshComponent attached to this actor
    UDynamicMeshComponent* DynamicMeshComponent = FindComponentByClass<UDynamicMeshComponent>();

    if (DynamicMeshComponent)
    {
        // Retrieve the world location of the component
        SelfLocationWorld = DynamicMeshComponent->GetComponentLocation();

        // Retrieve the transform (including position, rotation, and scale) of the component
        FTransform ComponentTransform = DynamicMeshComponent->GetComponentTransform();
        SelfTransformWorldInverted = ComponentTransform.Inverse();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("An error occurred: Dynamic Mesh Component not found."));
    }
}
