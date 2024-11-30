#include "Editor/SMapCreatorViewport.h"
#include "AdvancedPreviewScene.h"
#include "Editor/MapEditorPreset.h"

void SMapCreatorViewport::Construct(const FArguments& InArgs)
{
	// We need to create a new Scene before constructing this viewport. Otherwise, it will default to grabbing the one from the main World in the Editor
	AdvancedPreviewScene = MakeShareable(new FAdvancedPreviewScene(FPreviewScene::ConstructionValues()));

	SEditorViewport::Construct(SEditorViewport::FArguments());
	
	StylePreset = InArgs._EditingObject;
	if (!StylePreset.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Editing asset is not valid"));
		return;
	}

	//ABuildingBase* PreviewBuilding = GetWorld()->SpawnActor<ABuildingBase>();
	//PreviewBuilding->BuildingStyle = StylePreset.Get();
	//PreviewBuilding->RerunConstructionScripts();
}


TSharedRef<FEditorViewportClient> SMapCreatorViewport::MakeEditorViewportClient()
{
	LevelViewportClient = MakeShareable(new FEditorViewportClient(nullptr, AdvancedPreviewScene.Get(), SharedThis(this)));
	
	LevelViewportClient->ViewportType = LVT_Perspective;
	LevelViewportClient->bSetListenerPosition = false;
	LevelViewportClient->SetRealtime(true);
	
	LevelViewportClient->SetViewLocation( FVector(-50000.f,0.f,5000));
	LevelViewportClient->SetViewRotation( FRotator(-30.0f, -90.0f, 0.0f) );
	LevelViewportClient->SetViewLocationForOrbiting( FVector::ZeroVector );
	
	return LevelViewportClient.ToSharedRef();
}