#pragma once

#include "CoreMinimal.h"
#include "GeometryActors/GeneratedDynamicMeshActor.h"
#include "GeometryScript/GeometryScriptSelectionTypes.h"
#include "GeometryScript/MeshRemeshFunctions.h"
#include "SnowCoverGenerator.generated.h"

UCLASS()
class TD_ICICLESGENERATOR_API ASnowCoverGenerator : public AGeneratedDynamicMeshActor
{
	GENERATED_BODY()
public:
	/** Please add a function description */
	UFUNCTION(BlueprintCallable)
	void Process(UDynamicMesh* DynaMesh, UDynamicMesh* CurrDynaMesh, FGeometryScriptMeshSelection CurrSelectedTris);
public:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Default")
	TObjectPtr<UBillboardComponent> Billboard;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category="Snowy Actors")
	TArray<AStaticMeshActor*> ActorsToProcess;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Snow Settings")
	float SnowThickness;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Snow Settings", meta=(UIMin="0", ClampMin="0"))
	int32 RemeshIterations;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Snow Settings")
	float PerlinNoiseMagnitude;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Snow Settings")
	float PerlinNoiseFrequency;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Snow Settings")
	EGeometryScriptUniformRemeshTargetType RemeshType;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Snow Settings", meta=(UIMin="0", ClampMin="0"))
	int32 RemeshTargetTriangleCount;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Snow Settings", meta=(UIMin="0", ClampMin="0"))
	float RemeshTargetEdgeLength;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Snow Settings")
	double RejectActorsBiggerThan;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Snow Settings")
	int32 PerlinNoiseSeed;

};
