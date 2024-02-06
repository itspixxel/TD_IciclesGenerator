#pragma once

#include "CoreMinimal.h"
#include "GeometryActors/GeneratedDynamicMeshActor.h"
#include "SnowCoverGenerator.generated.h"
#include <GeometryScript/GeometryScriptSelectionTypes.h>
#include <GeometryScript/MeshRemeshFunctions.h>

UCLASS()
class TD_ICICLESGENERATOR_API ASnowCoverGenerator : public AGeneratedDynamicMeshActor
{
	GENERATED_BODY()

public:
	/* Processes the given mesh(es) to generate snow cover on */
	UFUNCTION(BlueprintCallable)
		void Process(UDynamicMesh* DynaMesh, UDynamicMesh* CurrDynaMesh, FGeometryScriptMeshSelection CurrSelectedTris);

	public:
	/* Array of actors to process */
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category="Snowy Actors")
	TArray<AStaticMeshActor*> ActorsToProcess;

	/* Thickness of the snow cover */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Snow Settings")
	float SnowThickness;

	/* Amount of remesh iterations */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Snow Settings", meta=(UIMin="0", ClampMin="0"))
	int32 RemeshIterations;

	/* Magnitude of the perlin noise applied to the dynamic mesh */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Snow Settings")
	float PerlinNoiseMagnitude;

	/* Frequency of the perlin noise applied to the dynamic mesh */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Snow Settings")
	float PerlinNoiseFrequency;

	/* Type of remesh */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Snow Settings")
	EGeometryScriptUniformRemeshTargetType RemeshType;

	/* Target triangle count for the remeshed dynamic mesh */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Snow Settings", meta=(UIMin="0", ClampMin="0"))
	int32 RemeshTargetTriangleCount;

	/* Target edge length for the remeshed dynamic mesh */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Snow Settings", meta=(UIMin="0", ClampMin="0"))
	float RemeshTargetEdgeLength;

	/* Failsafe to reject massive actors */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Snow Settings")
	double RejectActorsBiggerThan;

	/* Seed for the perlin noise applied to the dynamic mesh */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Snow Settings")
	int32 PerlinNoiseSeed;

};
