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
// TODO - refactor this function
void FMapObjectViewportClient::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event,
	uint32 HitX, uint32 HitY)
{
	if (Key == EKeys::LeftMouseButton && Event == EInputEvent::IE_Released)
	{
		const int32 Index = GetIndexOfTileSelected(HitX, HitY);
		if (const SMapObjectViewport* ViewportWidget = static_cast<SMapObjectViewport*>(EditorViewportWidget.Pin().Get()))
		{
			UMapObject* WorkingAsset = ViewportWidget->MapEditorApp.Pin().Get()->GetWorkingAsset();
			if(ViewportWidget->MapEditorApp.IsValid() && Index != -1)
			{
				// when not pressing shift clear 
				const bool bShiftDown = Viewport->KeyState(EKeys::LeftShift) || Viewport->KeyState(EKeys::RightShift);
				if(!bShiftDown)
				{
					WorkingAsset->ClearTilesSelected();
				}
				
				ViewportWidget->MapEditorApp.Pin()->UpdateEntrySelected(Index);
				bTileSelected = true;
				WorkingAsset->AddTileSelected(Index);
			}
			// not clicking on a tile we clear list
			if(Index == -1)
			{
				bTileSelected = false;
				WorkingAsset->ClearTilesSelected();
				ViewportWidget->MapEditorApp.Pin().Get()->UpdateHighlightTexture({});
			}
			else
			{
				
				ViewportWidget->MapEditorApp.Pin()->UpdateHighlightTexture(WorkingAsset->GetTilesSelected());
			}

		}
	}
	
	// Optionally, pass unhandled clicks to the base class
	FEditorViewportClient::ProcessClick(View, HitProxy, Key, Event, HitX, HitY);
}

bool FMapObjectViewportClient::InputKey(const FInputKeyEventArgs& EventArgs)
{
	if(EventArgs.Key == EKeys::M && EventArgs.Event == EInputEvent::IE_Released)
	{
		if(MapObject)
		{
			MapObject->LogMapData();
		}
	}
	if(EventArgs.Key == EKeys::L && EventArgs.Event == EInputEvent::IE_Released)
	{
		if(MapObject)
		{
			MapObject->LogLookupTable();
		}
		
	}
	if(EventArgs.Key == EKeys::V && EventArgs.Event == EInputEvent::IE_Released)
	{
		if(MapObject)
		{
			MapObject->LogVisualProperties();
		}
		
	}
	
	return FEditorViewportClient::InputKey(EventArgs);
}

void FMapObjectViewportClient::Tick(float DeltaSeconds)
{
	FIntPoint MousePos;
	Viewport->GetMousePos(MousePos);
	if(!bTileSelected)
	{
		// int32 Index = GetIndexOfTileSelected(MousePos.X, MousePos.Y);
	}
	
	FEditorViewportClient::Tick(DeltaSeconds);
}

int32 FMapObjectViewportClient::GetIndexOfTileSelected(int32 ScreenX, int32 ScreenY)
{
	if (!Viewport)
	{
		return -1;
	}
	// Create a scene view to perform the deprojection
	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		Viewport,
		this->GetWorld()->Scene,
		this->EngineShowFlags));
     
	FSceneView* SceneView = this->CalcSceneView(&ViewFamily);
	if (!SceneView)
	{
		return -1;
	}
	// Deproject screen coordinates into world space
	FVector WorldOrigin, WorldDirection;
	SceneView->DeprojectFVector2D(FVector2D(ScreenX, ScreenY), WorldOrigin, WorldDirection);

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
		const FColor Color = MapAsset->MapObject->GetColorFromUv(Uvs);

		if(MapAsset->Material)
		{
			return  MapAsset->MapObject->FindID(Color);
		}
	}
	return -1;
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

void SMapObjectViewport::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SEditorViewport::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
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