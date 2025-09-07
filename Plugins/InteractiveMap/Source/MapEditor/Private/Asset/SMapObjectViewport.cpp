#include "Asset/SMapObjectViewport.h"
#include "AdvancedPreviewScene.h"
#include "MapObject.h"
#include "MapEditor.h"
#include "Asset/MapAssetActor.h"
#include "BlueprintLibrary/TextureUtilsFunctionLibrary.h"
#include "Editor/MapEditorApp.h"
#include "Math/Color.h"  
#include "Kismet/GameplayStatics.h"

FMapObjectViewportClient::FMapObjectViewportClient(FAdvancedPreviewScene* InPreviewScene, const TSharedRef<SEditorViewport>& InViewport, UMapObject* InMapObject)
		: FEditorViewportClient(nullptr, InPreviewScene, InViewport), MapObject(InMapObject)
{
	
}
void FMapObjectViewportClient::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event,
	uint32 HitX, uint32 HitY)
{
	ProcessMouseClick(View, Key, Event, HitX, HitY);
	// Optionally, pass unhandled clicks to the base class
	FEditorViewportClient::ProcessClick(View, HitProxy, Key, Event, HitX, HitY);
}

void FMapObjectViewportClient::ProcessMouseClick(FSceneView& View,FKey Key, EInputEvent Event,
	uint32 HitX, uint32 HitY)
{
	if (!WasKeyReleased(Key, EKeys::LeftMouseButton, Event))
	{
		return;
	}
	
	const SMapObjectViewport* ViewportWidget = static_cast<SMapObjectViewport*>(EditorViewportWidget.Pin().Get());
	if (!ViewportWidget)
	{
		return;
	}
	if(!ViewportWidget->MapEditorApp.IsValid())
	{
		return;
	}
			
	FMapEditorApp* App = ViewportWidget->MapEditorApp.Pin().Get();
	UMapObject* WorkingAsset = App->GetWorkingAsset();
	const int32 Index = GetIndexOfTileSelected(View, App, HitX, HitY); 
	
	// not clicking on a tile we clear list
	if(Index == -1)
	{
		WorkingAsset->ClearTilesSelected();
		App->UpdateHighlightTexture({});
	}
	else
	{
		// when not pressing shift clear current selected tiles
		if(!IsShiftPressed())
		{
			WorkingAsset->ClearTilesSelected();
		}

		if(WorkingAsset->IsTileSelected(Index))
		{
			WorkingAsset->RemoveTileSelected(Index);
			if(!WorkingAsset->GetTilesSelected().IsEmpty())
			{
				App->UpdateEntrySelected(WorkingAsset->GetTilesSelected().Last());
			}
			else
			{
				App->ClearSelection();
			}
		}
		else
		{
			WorkingAsset->AddTileSelected(Index);
			App->UpdateEntrySelected(Index);
		}
		
		App->UpdateHighlightTexture(WorkingAsset->GetTilesSelected());
	}
}

bool FMapObjectViewportClient::InputKey(const FInputKeyEventArgs& EventArgs)
{
	if(WasKeyReleased(EventArgs.Key, EKeys::M, EventArgs.Event))
	{
		if(MapObject)
		{
			MapObject->LogMapData();
		}
	}
	if(WasKeyReleased(EventArgs.Key, EKeys::L, EventArgs.Event))
	{
		if(MapObject)
		{
			MapObject->LogLookupTable();
		}
	}
	if(WasKeyReleased(EventArgs.Key, EKeys::V, EventArgs.Event))
	{
		if(MapObject)
		{
			MapObject->LogVisualProperties();
		}
	}
	
	return FEditorViewportClient::InputKey(EventArgs);
}

int32 FMapObjectViewportClient::GetIndexOfTileSelected(const FSceneView& View, const FMapEditorApp* App, int32 ScreenX, int32 ScreenY)
{
	FVector WorldOrigin, WorldDirection;
	View.DeprojectFVector2D(FVector2D(ScreenX, ScreenY), WorldOrigin, WorldDirection);

	// Define a trace end point (far along the direction vector)
	const FVector TraceEnd = WorldOrigin + (WorldDirection * 100000.0f);

	// Perform a line trace
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.bTraceComplex = true; // Enable complex collision
	CollisionParams.bReturnFaceIndex = true;

	if (this->GetWorld()->LineTraceSingleByChannel(HitResult, WorldOrigin, TraceEnd, ECC_Visibility, CollisionParams))
	{
		FVector2D Uvs;
		UGameplayStatics::FindCollisionUV(HitResult, 0, Uvs);
		AMapAsset* MapAsset = Cast<AMapAsset>(HitResult.GetActor());
		if(!MapAsset)
			return -1;
		if(!MapAssetRef.IsValid())
		{
			MapAssetRef = MapAsset;
		}
		const TArray<uint8> buffer = UAtkTextureUtilsFunctionLibrary::ReadTextureToArray(App->GetCurrentTexture().Get());
		const FColor Color = UAtkTextureUtilsFunctionLibrary::GetColorFromUV(App->GetCurrentTexture().Get(), Uvs, buffer);

		if(MapAsset->Material)
		{
			return  MapAsset->MapObject->FindID(Color);
		}
	}
	return -1;
}

bool FMapObjectViewportClient::WasKeyReleased(const FKey& Key,  const FKey& KeyQueried, const EInputEvent Event)
{
	return Key == KeyQueried && Event == EInputEvent::IE_Released;
}

void SMapObjectViewport::Construct(const FArguments& InArgs)
{
	CustomObject = InArgs._EditingObject;
	// We need to create a new Scene before constructing this viewport. Otherwise, it will default to grabbing the one from the main World in the Editor
	AdvancedPreviewScene = MakeShareable(new FAdvancedPreviewScene(FPreviewScene::ConstructionValues()));
	
	SEditorViewport::Construct(SEditorViewport::FArguments());
	
	MapEditorApp = InArgs._app;
	if (!CustomObject.IsValid())
	{
		UE_LOG(LogInteractiveMapEditor, Error, TEXT("Editing asset is null"));
		return;
	}

	MapAsset = GetWorld()->SpawnActor<AMapAsset>();
	MapAsset->MapObject = CustomObject.Get();
	MapAsset->RerunConstructionScripts();
}

void SMapObjectViewport::UpdatePreviewActor() const 
{
	MapAsset->Material->SetTextureParameterValue("TextureHighlight", MapEditorApp.Pin()->GetHighlightTexture().Get());
}

void SMapObjectViewport::UpdatePreviewActorMaterial(UMaterial* ParentMaterial, UTexture2D* Texture2D) const
{
	MapAsset->SetMaterial(Texture2D, ParentMaterial);
	UpdatePreviewActor();
}

TSharedRef<FEditorViewportClient> SMapObjectViewport::MakeEditorViewportClient()
{
	LevelViewportClient = MakeShareable(new FMapObjectViewportClient(AdvancedPreviewScene.Get(), SharedThis(this), CustomObject.Get()));
	
	LevelViewportClient->ViewportType = LVT_OrthoXY;
	LevelViewportClient->bSetListenerPosition = false;
	LevelViewportClient->SetRealtime(true);
	
	LevelViewportClient->SetViewLocationForOrbiting(FVector(200, -150, 400));
	
	return LevelViewportClient.ToSharedRef();
}