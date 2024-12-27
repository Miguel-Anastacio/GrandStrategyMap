#include "Asset/SMapObjectViewport.h"
#include "AdvancedPreviewScene.h"
#include "Asset/MapObject.h"
#include "MapEditor.h"
#include "Asset/MapAssetActor.h"
#include "Engine/PointLight.h"

void SMapObjectViewport::Construct(const FArguments& InArgs)
{
	// We need to create a new Scene before constructing this viewport. Otherwise, it will default to grabbing the one from the main World in the Editor
	AdvancedPreviewScene = MakeShareable(new FAdvancedPreviewScene(FPreviewScene::ConstructionValues()));

	SEditorViewport::Construct(SEditorViewport::FArguments());
	
	CustomObject = InArgs._EditingObject;

	if (!CustomObject.IsValid())
	{
		UE_LOG(LogInteractiveMapEditor, Error, TEXT("Editing asset is null"));
		return;
	}

	AMapAsset* PreviewMapAsset = GetWorld()->SpawnActor<AMapAsset>();
	PreviewMapAsset->MapObject = CustomObject.Get();
	PreviewMapAsset->RerunConstructionScripts();
}

void SMapObjectViewport::UpdatePreviewActor()
{
	if (PreviewActor)
	{
		PreviewActor->Destroy();
		PreviewActor = nullptr;
	}
}

void SMapObjectViewport::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SEditorViewport::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	
	if (PreviewActor)
	{
		// float X = CustomObject->Radius * FMath::Cos(FPlatformTime::Seconds() * 0.1f * CustomObject->Speed);
		// float Y = CustomObject->Radius * FMath::Sin(FPlatformTime::Seconds() * 0.1f * CustomObject->Speed);
		// PreviewActor->SetActorLocation(FVector(X,Y,0));
	}
}

TSharedRef<FEditorViewportClient> SMapObjectViewport::MakeEditorViewportClient()
{
	LevelViewportClient = MakeShareable(new FEditorViewportClient(nullptr, AdvancedPreviewScene.Get(), SharedThis(this)));
	
	LevelViewportClient->ViewportType = LVT_Perspective;
	LevelViewportClient->bSetListenerPosition = false;
	LevelViewportClient->SetRealtime(true);
	
	LevelViewportClient->SetViewLocation( FVector(-25000.f,0.f,500));
	LevelViewportClient->SetViewRotation( FRotator(-30.0f, -90.0f, 0.0f) );
	LevelViewportClient->SetViewLocationForOrbiting( FVector::ZeroVector );
	
	return LevelViewportClient.ToSharedRef();
}