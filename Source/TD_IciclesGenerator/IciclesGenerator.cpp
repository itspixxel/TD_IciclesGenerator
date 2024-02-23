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
	RadiusVisualizer->SetSphereRadius(SphereRadius, true);
	if (DynaMesh)
	{
		DynaMesh = DynaMesh->Reset();
	}

	TempDynamicMesh = AllocateComputeMesh();

	RandomStream = UKismetMathLibrary::MakeRandomStream(RandomSeed);

	UDynamicMeshComponent* DynaMeshComp = FindComponentByClass<UDynamicMeshComponent>();
	
	SelfLocationWorld = DynaMeshComp->GetComponentLocation();
	SelfTransformWorldInverted = UKismetMathLibrary::InvertTransform(DynaMeshComp->GetComponentTransform());

	if (ActorToSample != nullptr)
	{
		UStaticMeshComponent* StaticMeshComp = ActorToSample->GetStaticMeshComponent();
		FGeometryScriptCopyMeshFromComponentOptions CopyMeshOptions;
		CopyMeshOptions.bWantNormals = true;
		CopyMeshOptions.bWantTangents = true;
		EGeometryScriptOutcomePins Output = EGeometryScriptOutcomePins::Success;
		UGeometryScriptLibrary_SceneUtilityFunctions::CopyMeshFromComponent(StaticMeshComp, TempDynamicMesh, CopyMeshOptions, true, MeshSourceLocalToWorld, Output);
	}
	
			
	FGeometryScriptMeshSelection SelectionA;
	FGeometryScriptMeshSelection SelectionB;
	FVector Normal;
	Normal.Z = -1.0f;
	EGeometryScriptCombineSelectionMode CombineMode = EGeometryScriptCombineSelectionMode::Intersection;
	UGeometryScriptLibrary_MeshSelectionFunctions::SelectMeshElementsByNormalAngle(TempDynamicMesh, SelectionA, Normal, 40.f);
	UGeometryScriptLibrary_MeshSelectionFunctions::SelectMeshElementsInSphere(TempDynamicMesh, SelectionB, SelfLocationWorld, SphereRadius);
	UGeometryScriptLibrary_MeshSelectionFunctions::CombineMeshSelections(SelectionA, SelectionB, TrianglesFacingDown, CombineMode);

	UDynamicMesh* SubMesh;
	UGeometryScriptLibrary_MeshDecompositionFunctions::CopyMeshSelectionToMesh(TempDynamicMesh, DynaMesh, TrianglesFacingDown, SubMesh);
	UGeometryScriptLibrary_MeshTransformFunctions::TransformMesh(SubMesh, SelfTransformWorldInverted);

	FGeometryScriptMeshOffsetOptions MeshOffsetOptions;
	MeshOffsetOptions.OffsetDistance = ExtrusionBias;
	MeshOffsetOptions.bFixedBoundary = false;
	MeshOffsetOptions.SolveSteps = 5;
	MeshOffsetOptions.SmoothAlpha = 0.1f;
	MeshOffsetOptions.bReprojectDuringSmoothing = false;
	MeshOffsetOptions.BoundaryAlpha = 0.2f;

	UGeometryScriptLibrary_MeshModelingFunctions::ApplyMeshOffset(DynaMesh, MeshOffsetOptions);

	FGeometryScriptSimplifyMeshOptions SimplifyOptions;
	SimplifyOptions.Method = EGeometryScriptRemoveMeshSimplificationType::VolumePreserving;
	SimplifyOptions.bAllowSeamCollapse = true;
	SimplifyOptions.bAllowSeamSmoothing = true;
	SimplifyOptions.bAllowSeamSplits = true;
	SimplifyOptions.bPreserveVertexPositions = true;
	SimplifyOptions.bRetainQuadricMemory = false;
	SimplifyOptions.bAutoCompact = true;
	DynaMesh = UGeometryScriptLibrary_MeshSimplifyFunctions::ApplySimplifyToVertexCount(DynaMesh, PointsToScatter, SimplifyOptions);

	FGeometryScriptVectorList PositionList;
	bool HasVertexIDGaps;
	UGeometryScriptLibrary_MeshQueryFunctions::GetAllVertexPositions(DynaMesh, PositionList, false, HasVertexIDGaps);

	TArray<FVector> VectorArray;
	UGeometryScriptLibrary_ListUtilityFunctions::ConvertVectorListToArray(PositionList, VectorArray);

	TempDynamicMesh = TempDynamicMesh->Reset();

	FRotator TempRotation;
	TempRotation.Pitch = 180.0f;
	FTransform AppendConeTransform;
	float BaseRadius;
	float TopRadius = 0.0f;
	float TempConeHeight;
	FGeometryScriptPrimitiveOptions PrimitiveOptions;
	for (int i = 0; i < VectorArray.Num(); ++i)
	{
		AppendConeTransform = UKismetMathLibrary::MakeTransform(VectorArray[i], TempRotation);
		BaseRadius = UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream, ConeRadius * 0.2f, ConeRadius);
		TempConeHeight = UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream, ConeHeight * 0.3f, ConeHeight);
		UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendCone(TempDynamicMesh, PrimitiveOptions, AppendConeTransform, BaseRadius, TopRadius, TempConeHeight, 24, 24);
	}

	FGeometryScriptMeshSelection PerlinNoiseSelection;
	FGeometryScriptPerlinNoiseOptions PerlinNoiseOptions;
	PerlinNoiseOptions.bApplyAlongNormal = true;
	PerlinNoiseOptions.BaseLayer.Magnitude = ConeRadius * 0.15f;
	TempDynamicMesh = UGeometryScriptLibrary_MeshDeformFunctions::ApplyPerlinNoiseToMesh(TempDynamicMesh, PerlinNoiseSelection, PerlinNoiseOptions);

	FGeometryScriptSolidifyOptions SolidifyOptions;
	SolidifyOptions.GridParameters.GridResolution = 96;

	TempDynamicMesh = UGeometryScriptLibrary_MeshVoxelFunctions::ApplyMeshSolidify(TempDynamicMesh, SolidifyOptions);

	FGeometryScriptSimplifyMeshOptions SimplifyMeshOptions;
	SimplifyMeshOptions.Method = EGeometryScriptRemoveMeshSimplificationType::VolumePreserving;
	TempDynamicMesh = UGeometryScriptLibrary_MeshSimplifyFunctions::ApplySimplifyToTolerance(TempDynamicMesh, DecimationTolerance, SimplifyMeshOptions);

	FGeometryScriptCalculateNormalsOptions NormalsOptions;
	UGeometryScriptLibrary_MeshNormalsFunctions::RecomputeNormals(TempDynamicMesh, NormalsOptions);

	DynaMesh = DynaMesh->Reset();
	UGeometryScriptLibrary_MeshDecompositionFunctions::CopyMeshToMesh(TempDynamicMesh, DynaMesh, DynaMesh);
}
