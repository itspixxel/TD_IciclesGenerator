#include "SnowCoverGenerator.h"

#include "Engine/StaticMeshActor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GeometryScript/MeshAssetFunctions.h"
#include "GeometryScript/MeshSelectionFunctions.h"
#include "GeometryScript/MeshTransformFunctions.h"
#include "GeometryScript/MeshBasicEditFunctions.h"
#include "GeometryScript/MeshDeformFunctions.h"
#include "GeometryScript/MeshModelingFunctions.h"
#include "GeometryScript/MeshNormalsFunctions.h"
#include "Kismet/KismetMathLibrary.h"
#include "GeometryScript/MeshUVFunctions.h"

void ASnowCoverGenerator::Process(UDynamicMesh* DynaMesh, UDynamicMesh* CurrDynaMesh,
                                  FGeometryScriptMeshSelection CurrSelectedTris)
{
	for (int i = 0; i < ActorsToProcess.Num(); ++i)
	{
		FVector Origin;
		FVector BoxExtent;
		float SphereRadius;
		if (ActorsToProcess[i])
		{
			UStaticMeshComponent* StaticMeshComp = ActorsToProcess[i]->GetStaticMeshComponent();
			if (StaticMeshComp)
			{
				UKismetSystemLibrary::GetComponentBounds(StaticMeshComp, Origin, BoxExtent, SphereRadius);
				USceneComponent* DynaMeshComp = GetRootComponent();

				if (BoxExtent.Size() < RejectActorsBiggerThan)
				{
					CurrDynaMesh = AllocateComputeMesh();

					FGeometryScriptCopyMeshFromAssetOptions AssetOptions;
					FGeometryScriptMeshReadLOD TargetLOD;
					EGeometryScriptOutcomePins Outcome;
					UDynamicMesh* DynamicMesh = UGeometryScriptLibrary_StaticMeshFunctions::CopyMeshFromStaticMesh(
							StaticMeshComp->GetStaticMesh(),
							CurrDynaMesh,
							AssetOptions,
							TargetLOD,
							Outcome
						);

					UGeometryScriptLibrary_MeshTransformFunctions::TransformMesh(DynamicMesh, StaticMeshComp->GetComponentTransform());
			
					FGeometryScriptMeshSelection NormalSelection;
					FVector Normal;
					Normal.Z = 1.0f;
					float MaxAngleDeg = 80.0f;
					EGeometryScriptMeshSelectionType SelectionType = EGeometryScriptMeshSelectionType::Triangles;
					UGeometryScriptLibrary_MeshSelectionFunctions::SelectMeshElementsByNormalAngle(
						CurrDynaMesh,
						NormalSelection,
						Normal,
						MaxAngleDeg,
						SelectionType,
						true
					);
					CurrSelectedTris = NormalSelection;

					int NumDeleted;
					UGeometryScriptLibrary_MeshBasicEditFunctions::DeleteSelectedTrianglesFromMesh(
						CurrDynaMesh,
						CurrSelectedTris,
						NumDeleted
					);

					FVector ExtrudeOptionsDirection;
					ExtrudeOptionsDirection.Z = 1.0f;
					FGeometryScriptMeshLinearExtrudeOptions ExtrudeOptions;
					ExtrudeOptions.Distance = SnowThickness;
					ExtrudeOptions.DirectionMode = EGeometryScriptLinearExtrudeDirection::AverageFaceNormal;
					ExtrudeOptions.Direction = ExtrudeOptionsDirection;
					FGeometryScriptMeshSelection ExtrudeSelection;
			
					UGeometryScriptLibrary_MeshModelingFunctions::ApplyMeshLinearExtrudeFaces(CurrDynaMesh, ExtrudeOptions, ExtrudeSelection);

					FGeometryScriptMeshOffsetOptions OffsetOptions;
					OffsetOptions.OffsetDistance = SnowThickness;
			
					UGeometryScriptLibrary_MeshModelingFunctions::ApplyMeshOffset(CurrDynaMesh, OffsetOptions);

					FTransform DynaMeshCompTransformInverted = UKismetMathLibrary::InvertTransform(DynaMeshComp->GetComponentTransform());
					UGeometryScriptLibrary_MeshBasicEditFunctions::AppendMesh(DynaMesh, CurrDynaMesh, DynaMeshCompTransformInverted);

					FGeometryScriptRemeshOptions RemeshOptions;
					RemeshOptions.bDiscardAttributes = true;
					RemeshOptions.SmoothingType = EGeometryScriptRemeshSmoothingType::Uniform;
					RemeshOptions.RemeshIterations = RemeshIterations;
					FGeometryScriptUniformRemeshOptions UniformOptions;
					UniformOptions.TargetType = RemeshType;
					UniformOptions.TargetTriangleCount = RemeshTargetTriangleCount;
					UniformOptions.TargetEdgeLength = RemeshTargetEdgeLength;
					DynaMesh = UGeometryScriptLibrary_RemeshingFunctions::ApplyUniformRemesh(DynaMesh, RemeshOptions, UniformOptions);

					FGeometryScriptMeshSelection PerlinNoiseSelection;
					FGeometryScriptPerlinNoiseOptions PerlinNoiseOptions;
					PerlinNoiseOptions.BaseLayer.Magnitude = PerlinNoiseMagnitude;
					PerlinNoiseOptions.BaseLayer.Frequency = PerlinNoiseFrequency;
					PerlinNoiseOptions.BaseLayer.RandomSeed = PerlinNoiseSeed;
					DynaMesh = UGeometryScriptLibrary_MeshDeformFunctions::ApplyPerlinNoiseToMesh(DynaMesh, PerlinNoiseSelection, PerlinNoiseOptions);

					FGeometryScriptMeshSelection SmoothingSelection;
					FGeometryScriptIterativeMeshSmoothingOptions SmoothingOptions;
					SmoothingOptions.NumIterations = 6;
					SmoothingOptions.Alpha = 0.1;
					DynaMesh = UGeometryScriptLibrary_MeshDeformFunctions::ApplyIterativeSmoothingToMesh(DynaMesh, SmoothingSelection, SmoothingOptions);

					FGeometryScriptCalculateNormalsOptions CalculateOptions;
					DynaMesh = UGeometryScriptLibrary_MeshNormalsFunctions::RecomputeNormals(DynaMesh, CalculateOptions);
			
					FGeometryScriptXAtlasOptions UVOptions;
					UGeometryScriptLibrary_MeshUVFunctions::AutoGenerateXAtlasMeshUVs(DynaMesh, 0, UVOptions);
				}
			}
			
		}
	}
}
