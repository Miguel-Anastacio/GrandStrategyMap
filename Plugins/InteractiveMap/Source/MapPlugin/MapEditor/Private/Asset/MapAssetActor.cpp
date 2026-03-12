// Fill out your copyright notice in the Description page of Project Settings.
#include "Asset/MapAssetActor.h"
#include "MapObject.h"
#include "Editor/MapEditorApp.h"
#include "Editor/MapEditorPreset.h"
#include "Materials/MaterialInstanceConstant.h"

void AMapAsset::SetMaterial(UTexture2D* Texture, UMaterial* ParentMaterial)
{
	UMaterialInstanceConstant* MaterialInstance = NewObject<UMaterialInstanceConstant>();
	MaterialInstance->SetParentEditorOnly(ParentMaterial);
	MaterialInstance->SetTextureParameterValueEditorOnly(FName("DynamicTexture"), Texture);
	MaterialInstance->SetTextureParameterValueEditorOnly(FName("LookupTexture"), Texture);
	Material = UMaterialInstanceDynamic::Create(MaterialInstance, this);
	MapMesh->SetMaterial(0, Material);
}

void AMapAsset::SetBind(TWeakPtr<class FMapEditorApp> app)
{
	if (!app.IsValid())
	{
		return;
	}
	app.Pin()->OnCurrentTextureChanged.AddUObject(this, &AMapAsset::OnTextureChanged);
	app.Pin()->OnHighlightChanged.AddUObject(this, &AMapAsset::OnHighlightUpdate);

	UTexture2D* Texture = app.Pin()->GetCurrentTexture().Get();
	SetMaterial(Texture, app.Pin()->GetMapGenPreset()->Material);
	OnHighlightUpdate(app.Pin()->GetHighlightTexture());
}

void AMapAsset::OnTextureChanged(const TWeakObjectPtr<UTexture2D> Texture2D) const
{
	if (!Texture2D.IsValid())
		return;
	UTexture2D* Texture = Texture2D.Get();
	Material->SetTextureParameterValue(FName("DynamicTexture"), Texture);
	Material->SetTextureParameterValue(FName("LookupTexture"), Texture);
}

void AMapAsset::OnHighlightUpdate(const TWeakObjectPtr<UTexture2D> HighlightTexture) const
{
	if (!HighlightTexture.IsValid())
		return;
	UTexture2D* Texture = HighlightTexture.Get();
	
	Material->SetTextureParameterValue("TextureHighlight", Texture);
}

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

void AMapAsset::Destroyed()
{
	Super::Destroyed();
}
