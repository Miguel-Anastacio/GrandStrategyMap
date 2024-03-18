// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/InteractiveMap.h"
#include "Map/MapUtils.h"
// Sets default values
AInteractiveMap::AInteractiveMap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	MapSelectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Map Selection"));
	MapSelectMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AInteractiveMap::BeginPlay()
{
	Super::BeginPlay();
	CreateLookUpTable();
	
}

// Called every frame
void AInteractiveMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UE_DISABLE_OPTIMIZATION
void AInteractiveMap::CreateLookUpTable()
{
	TArray<FName> RowNames = MapDataTable->GetRowNames();

	for (auto& name : RowNames)
	{
		FProvinceIDData* Item = MapDataTable->FindRow<FProvinceIDData>(name, "");
		if (Item)
		{
			FString temp = Item->Color;
			temp += FString("FF");
			FColor sRGBColor = Item->ConvertHexStringToRGB(temp);

			LookUpTable.Add(FVector(sRGBColor.R, sRGBColor.G, sRGBColor.B), Item->Name);
		}
	}
}
UE_ENABLE_OPTIMIZATION

UTextureRenderTarget2D* AInteractiveMap::GetMapRenderTarget() const
{
	return MapRenderTarget;
}

TMap<FVector,FString> AInteractiveMap::GetLookUpTable() const
{
	return LookUpTable;
}

FString AInteractiveMap::GetProvinceID(const FVector& color) const
{
	const FString* ID = LookUpTable.Find(color);
	if (ID)
	{
		return (*ID);
	}

	return FString();
}
