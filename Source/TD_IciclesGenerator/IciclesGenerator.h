#pragma once

#include "CoreMinimal.h"
#include "GeometryActors/GeneratedDynamicMeshActor.h"
#include "IciclesGenerator.generated.h"

class UDynamicMeshComponent;
class USphereComponent;

UCLASS()
class TD_ICICLESGENERATOR_API AIciclesGenerator : public AGeneratedDynamicMeshActor
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* RadiusVisualizer;

public:
	AIciclesGenerator();

	UFUNCTION(BlueprintCallable)
		void Process(UDynamicMeshComponent* DynaMesh);

	void OnRebuildGeneratedMesh(UDynamicMesh* TargetMesh);

protected:
	UPROPERTY(EditAnywhere, Category = "Icicles Settings")
		int32 RandomSeed = 0;

	UPROPERTY(EditAnywhere, Category = "Icicles Settings")
		float SphereRadius;

	UPROPERTY(EditAnywhere)
		float ExtrusionBias;

	UPROPERTY(EditAnywhere)
		int PointsToScatter;

	UPROPERTY(EditAnywhere)
		float ConeRadius;

	UPROPERTY(EditAnywhere)
		float ConeHeight;

	UPROPERTY(EditAnywhere)
		float DecimationTolerance;

	UPROPERTY(EditAnywhere)
		AStaticMeshActor* ActorToSample;

private:
	FVector SelfWorldLocation;
	FTransform SelfWorldTransformInverted;
	UDynamicMesh* TempDynamicMesh;
	FTransform MeshSourceLocalToWorld;
	float CurrentConeRadius;
};
