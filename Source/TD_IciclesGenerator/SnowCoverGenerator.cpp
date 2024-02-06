// Fill out your copyright notice in the Description page of Project Settings.


#include "SnowCoverGenerator.h"
#include <GeometryScript/MeshSelectionFunctions.h>
#include <GeometryScript/MeshTransformFunctions.h>
#include <GeometryScript/MeshBasicEditFunctions.h>

void ASnowCoverGenerator::Process(UDynamicMesh* DynaMesh, UDynamicMesh* CurrDynaMesh, FGeometryScriptMeshSelection CurrSelectedTris)
{
	AllocateComputeMesh();
	CopyPropertiesFromStaticMesh();
	UGeometryScriptLibrary_MeshTransformFunctions::TransformMesh();
	UGeometryScriptLibrary_MeshSelectionFunctions::SelectMeshElementsByNormalAngle();
	UGeometryScriptLibrary_MeshBasicEditFunctions::DeleteSelectedTrianglesFromMesh();
}
