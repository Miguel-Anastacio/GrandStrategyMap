// Fill out your copyright notice in the Description page of Project Settings.
#include "Asset/MapAssetActor.h"

#include "Asset/MapObject.h"

AMapAsset::AMapAsset()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
	MapMesh = CreateDefaultSubobject<UStaticMeshComponent>("MapMesh");
	MapMesh->SetupAttachment(RootComponent);

	ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMeshFinder(TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (PlaneMeshFinder.Succeeded())
	{
		MapMesh->SetStaticMesh(PlaneMeshFinder.Object);
		MapMesh->SetWorldScale3D(FVector(20, 15, 1));
	}
}

void AMapAsset::BeginPlay()
{
	Super::BeginPlay();
	MapMesh->SetMobility(EComponentMobility::Static);
}

void AMapAsset::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if(!MapObject)
		return;

#if WITH_EDITOR
	if(MapObject->Mesh)
	{
		MapMesh->SetStaticMesh(MapObject->Mesh);
	}
	if(MapObject->MaterialOverride)
	{
		MapMesh->SetMaterial(0, MapObject->MaterialOverride);
	}
	
	if (!MapObject->OnObjectChanged.IsBoundToObject(this))
	{
		MapObject->OnObjectChanged.AddUObject(this, &AMapAsset::RerunConstructionScripts);
	}
#endif
}

void AMapAsset::Destroyed()
{
	Super::Destroyed();
}
