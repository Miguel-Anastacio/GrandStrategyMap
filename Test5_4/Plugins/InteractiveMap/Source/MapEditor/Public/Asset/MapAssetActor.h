// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapAssetActor.generated.h"
class UMapObject;
class UStaticMeshComponent;
UCLASS()
class MAPEDITOR_API AMapAsset : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapAsset")
	UStaticMeshComponent* MapMesh;

	UPROPERTY()
	UMaterialInstanceDynamic* Material;

	void SetMaterial(UTexture2D* Texture, UMaterial* ParentMaterial);
	void SetBind(TWeakPtr<class FMapEditorApp> app);
	void OnTextureChanged(const TWeakObjectPtr<UTexture2D> CurrentTexture) const;
	void OnHighlightUpdate(const TWeakObjectPtr<UTexture2D> HighlightTexture) const;
protected:
	AMapAsset();
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

};
