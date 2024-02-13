#pragma once

#include "CoreMinimal.h"
#include "GeometryActors/GeneratedDynamicMeshActor.h"
#include "IciclesGenerator.generated.h"
#include <GeometryScript/GeometryScriptSelectionTypes.h>

class UDynamicMeshComponent;
class USphereComponent;

UCLASS()
class TD_ICICLESGENERATOR_API AIciclesGenerator : public AGeneratedDynamicMeshActor
{
	GENERATED_BODY()

public:
	/** Please add a function description */
	UFUNCTION(BlueprintCallable)
		void Process(UDynamicMesh* DynaMesh, FGeometryScriptMeshSelection TrianglesFacingDown);
public:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
		TObjectPtr<USphereComponent> RadiusVisualizer;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Icicles Settings")
		int32 RandomSeed;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		FRandomStream RandomStream;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		FVector SelfLocationWorld;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		FTransform SelfTransformWorldInverted;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		TObjectPtr<UDynamicMesh> TempDynamicMesh;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		FTransform MeshSourceLocalToWorld;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Icicles Settings")
		double SphereRadius;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Icicles Settings")
		float ExtrusionBias;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Icicles Settings")
		int32 PointsToScatter;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Icicles Settings")
		double ConeRadius;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Icicles Settings")
		float ConeHeight;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		float ConeRadiusCurrent;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Icicles Settings")
		float DecimationTolerance;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Icicles Settings")
		TObjectPtr<AStaticMeshActor> ActorToSample;

};
