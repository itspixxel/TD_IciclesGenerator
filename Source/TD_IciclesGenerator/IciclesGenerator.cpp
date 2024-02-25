#include "IciclesGenerator.h"
#include "Kismet/KismetMathLibrary.h"
#include "GeometryScript/SceneUtilityFunctions.h"
#include "GeometryScript/MeshSelectionFunctions.h"
#include "GeometryScript/MeshDecompositionFunctions.h"
#include "GeometryScript/MeshTransformFunctions.h"
#include "GeometryScript/MeshModelingFunctions.h"
#include "GeometryScript/MeshSimplifyFunctions.h"
#include "GeometryScript/MeshQueryFunctions.h"
#include "GeometryScript/ListUtilityFunctions.h"
#include "GeometryScript/MeshPrimitiveFunctions.h"
#include "GeometryScript/MeshDeformFunctions.h"
#include "GeometryScript/MeshVoxelFunctions.h"
#include "GeometryScript/MeshNormalsFunctions.h"

#include "Engine/StaticMeshActor.h"

AIciclesGenerator::AIciclesGenerator()
{
	RadiusVisualizer = CreateDefaultSubobject<USphereComponent>(TEXT("Radius Visualizer"));
	RadiusVisualizer->SetupAttachment(RootComponent);
	RadiusVisualizer->InitSphereRadius(SphereRadius);
}

void AIciclesGenerator::Process(UDynamicMesh* DynaMesh)
{
	// Set the radius visualizer sphere radius
	RadiusVisualizer->SetSphereRadius(SphereRadius, true);
	if (DynaMesh)
	{
		// Reset the dynamic mesh
		DynaMesh->Reset();
	}

	// Allocate a temporary dynamic mesh to work on
	TempDynamicMesh = AllocateComputeMesh();

	// Initialize a deterministic RNG
	RandomStream = UKismetMathLibrary::MakeRandomStream(RandomSeed);

	// Get a reference the dynamic mesh component attached to the class by default
	UDynamicMeshComponent* DynaMeshComp = GetComponentByClass<UDynamicMeshComponent>();

	// Save the transforms for later
	SelfLocationWorld = DynaMeshComp->GetComponentLocation();
	UE_LOG(LogTemp, Warning, TEXT("dynamic mesh component location: %s"), *SelfLocationWorld.ToString());
	SelfTransformWorldInverted = UKismetMathLibrary::InvertTransform(DynaMeshComp->GetComponentTransform());
	UE_LOG(LogTemp, Warning, TEXT("dynamic mesh component transform: %s"), *SelfTransformWorldInverted.ToString());
	
	if (ActorToSample != nullptr)
	{
		// Check if the actor is within the sphere volume
		FVector ActorLocation = ActorToSample->GetActorLocation();
		float DistanceSquared = FVector::DistSquared(ActorLocation, SelfLocationWorld);
		float SphereRadiusSquared = SphereRadius * SphereRadius;
    
		if (DistanceSquared > SphereRadiusSquared)
		{
			// The selected actor is outside the sphere volume
			// Handle this case, for example, by logging a warning and returning early
			UE_LOG(LogTemp, Warning, TEXT("Selected actor is outside the sphere volume."));
			return;
		}

		// Copy the geometry from the static mesh
		UStaticMeshComponent* StaticMeshComp = ActorToSample->GetStaticMeshComponent();
		FGeometryScriptCopyMeshFromComponentOptions CopyMeshOptions;
		CopyMeshOptions.bWantNormals = true;
		CopyMeshOptions.bWantTangents = true;
		EGeometryScriptOutcomePins Output;
		UGeometryScriptLibrary_SceneUtilityFunctions::CopyMeshFromComponent(
			StaticMeshComp, TempDynamicMesh, CopyMeshOptions, true, MeshSourceLocalToWorld, Output);
		UE_LOG(LogTemp, Warning, TEXT("Mesh Source Local To World: %s"), *MeshSourceLocalToWorld.ToString());
	}
	else
	{
		// Handle the case where ActorToSample is nullptr
		UE_LOG(LogTemp, Warning, TEXT("ActorToSample is nullptr."));
		return;
	}

	// Make a selection of downwards facing triangles within defined sphere
	FGeometryScriptMeshSelection SelectionA;
	FGeometryScriptMeshSelection SelectionB;
	FVector Normal;
	Normal.Z = -1.0f;
	EGeometryScriptCombineSelectionMode CombineMode = EGeometryScriptCombineSelectionMode::Intersection;
	
	UGeometryScriptLibrary_MeshSelectionFunctions::SelectMeshElementsByNormalAngle(
		TempDynamicMesh, SelectionA, Normal, 40.f);
	UGeometryScriptLibrary_MeshSelectionFunctions::SelectMeshElementsInSphere(
		TempDynamicMesh, SelectionB, SelfLocationWorld, SphereRadius);
	UGeometryScriptLibrary_MeshSelectionFunctions::CombineMeshSelections(
		SelectionA, SelectionB, TrianglesFacingDown, CombineMode);

	UDynamicMesh* SubMesh;

	// Copy only the selected tris into the mesh
	if (ActorToSample != nullptr)
	{
		UGeometryScriptLibrary_MeshDecompositionFunctions::CopyMeshSelectionToMesh(
			TempDynamicMesh, DynaMesh, TrianglesFacingDown, SubMesh);
		UGeometryScriptLibrary_MeshTransformFunctions::TransformMesh(SubMesh, SelfTransformWorldInverted);
	}


	// Mesh offset options
	FGeometryScriptMeshOffsetOptions MeshOffsetOptions;
	MeshOffsetOptions.OffsetDistance = ExtrusionBias;
	MeshOffsetOptions.bFixedBoundary = false;
	MeshOffsetOptions.SolveSteps = 5;
	MeshOffsetOptions.SmoothAlpha = 0.1f;
	MeshOffsetOptions.bReprojectDuringSmoothing = false;
	MeshOffsetOptions.BoundaryAlpha = 0.2f;

	// Make the mesh thicker
	UGeometryScriptLibrary_MeshModelingFunctions::ApplyMeshOffset(DynaMesh, MeshOffsetOptions);

	// Mesh simplify options
	FGeometryScriptSimplifyMeshOptions SimplifyOptions;
	SimplifyOptions.Method = EGeometryScriptRemoveMeshSimplificationType::VolumePreserving;
	SimplifyOptions.bAllowSeamCollapse = true;
	SimplifyOptions.bAllowSeamSmoothing = true;
	SimplifyOptions.bAllowSeamSplits = true;
	SimplifyOptions.bPreserveVertexPositions = true;
	SimplifyOptions.bRetainQuadricMemory = false;
	SimplifyOptions.bAutoCompact = true;
	// Scatter points on the mesh for icicles
	DynaMesh = UGeometryScriptLibrary_MeshSimplifyFunctions::ApplySimplifyToVertexCount(
		DynaMesh, PointsToScatter, SimplifyOptions);

	FGeometryScriptVectorList PositionList;
	bool HasVertexIDGaps;
	UGeometryScriptLibrary_MeshQueryFunctions::GetAllVertexPositions(DynaMesh, PositionList, false, HasVertexIDGaps);

	TArray<FVector> VectorArray;
	UGeometryScriptLibrary_ListUtilityFunctions::ConvertVectorListToArray(PositionList, VectorArray);

	TempDynamicMesh->Reset();

	FRotator TempRotation;
	TempRotation.Pitch = 180.0f;
	FTransform AppendConeTransform;
	float BaseRadius;
	float TempConeHeight;
	// Append cones at scattered points
	for (int i = 0; i < VectorArray.Num(); ++i)
	{
		float TopRadius = 0.0f;
		FGeometryScriptPrimitiveOptions PrimitiveOptions;
		AppendConeTransform = UKismetMathLibrary::MakeTransform(VectorArray[i], TempRotation);
		BaseRadius = UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream, ConeRadius * 0.2f, ConeRadius);
		TempConeHeight = UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream, ConeHeight * 0.3f, ConeHeight);
		UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendCone(TempDynamicMesh, PrimitiveOptions,
		                                                          AppendConeTransform, BaseRadius, TopRadius,
		                                                          TempConeHeight, 24, 24);
	}

	// Apply post processes to the icicles
	FGeometryScriptMeshSelection PerlinNoiseSelection;
	FGeometryScriptPerlinNoiseOptions PerlinNoiseOptions;
	PerlinNoiseOptions.bApplyAlongNormal = true;
	PerlinNoiseOptions.BaseLayer.Magnitude = ConeRadius * 0.15f;
	TempDynamicMesh = UGeometryScriptLibrary_MeshDeformFunctions::ApplyPerlinNoiseToMesh(
		TempDynamicMesh, PerlinNoiseSelection, PerlinNoiseOptions);

	FGeometryScriptSolidifyOptions SolidifyOptions;
	SolidifyOptions.GridParameters.GridResolution = 96;

	TempDynamicMesh = UGeometryScriptLibrary_MeshVoxelFunctions::ApplyMeshSolidify(TempDynamicMesh, SolidifyOptions);

	FGeometryScriptSimplifyMeshOptions SimplifyMeshOptions;
	SimplifyMeshOptions.Method = EGeometryScriptRemoveMeshSimplificationType::VolumePreserving;
	TempDynamicMesh = UGeometryScriptLibrary_MeshSimplifyFunctions::ApplySimplifyToTolerance(
		TempDynamicMesh, DecimationTolerance, SimplifyMeshOptions);

	FGeometryScriptCalculateNormalsOptions NormalsOptions;
	UGeometryScriptLibrary_MeshNormalsFunctions::RecomputeNormals(TempDynamicMesh, NormalsOptions);

	// Reset the dynamic mesh and copy the temp dynamic mesh to the dynamic mesh component
	DynaMesh->Reset();
	UGeometryScriptLibrary_MeshDecompositionFunctions::CopyMeshToMesh(TempDynamicMesh, DynaMesh, DynaMesh);
}
