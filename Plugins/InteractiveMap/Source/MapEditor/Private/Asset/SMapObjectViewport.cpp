#include "Asset/SMapObjectViewport.h"
#include "AdvancedPreviewScene.h"
#include "EngineUtils.h"
#include "MapObject.h"
#include "MapEditor.h"
#include "Asset/MapAssetActor.h"
#include "Asset/MapObjectToolkit.h"
#include "BlueprintLibrary/AssetCreatorFunctionLibrary.h"
#include "Math/Color.h"  
#include "HAL/PlatformApplicationMisc.h"
#include "Kismet/GameplayStatics.h"

FMapObjectViewportClient::FMapObjectViewportClient(FAdvancedPreviewScene* InPreviewScene, const TSharedRef<SEditorViewport>& InViewport, UMapObject* InMapObject)
		: FEditorViewportClient(nullptr, InPreviewScene, InViewport), MapObject(InMapObject)
{
	
}

void FMapObjectViewportClient::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event,
	uint32 HitX, uint32 HitY)
{
	if (Key == EKeys::LeftMouseButton && Event == EInputEvent::IE_Released)
	{
		GetHitLocationInEditor(HitX, HitY);
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

void FMapObjectViewportClient::GetHitLocationInEditor(int32 ScreenX, int32 ScreenY)
{
	if (Viewport)
	{
		// Create a scene view to perform the deprojection
		FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
			Viewport,
			this->GetWorld()->Scene,
			this->EngineShowFlags));
         
		FSceneView* SceneView = this->CalcSceneView(&ViewFamily);
		if (SceneView)
		{
			// Deproject screen coordinates into world space
			FVector WorldOrigin, WorldDirection;
			SceneView->DeprojectFVector2D(FVector2D(ScreenX, ScreenY), WorldOrigin, WorldDirection);

			// Define a trace end point (far along the direction vector)
			FVector TraceEnd = WorldOrigin + (WorldDirection * 100000.0f);

			// Perform a line trace
			FHitResult HitResult;
			FCollisionQueryParams CollisionParams;
			CollisionParams.bTraceComplex = true; // Enable complex collision
			CollisionParams.bReturnFaceIndex = true;

			if (this->GetWorld()->LineTraceSingleByChannel(HitResult, WorldOrigin, TraceEnd, ECC_Visibility, CollisionParams))
			{
				FVector2D Uvs;
				UGameplayStatics::FindCollisionUV(HitResult, 0, Uvs);
				const AMapAsset* MapAsset = Cast<AMapAsset>(HitResult.GetActor());
				if(!MapAsset)
					return;
				FColor Color = MapAsset->MapObject->GetColorFromUv(Uvs);
				
				const int Index = MapAsset->MapObject->GetIndexOfTileSelected(Color);
				// bool Executed = OnClickedOnMapDelegate.ExecuteIfBound(Index);

				if (const SMapObjectViewport* ViewportWidget = static_cast<SMapObjectViewport*>(EditorViewportWidget.Pin().Get()))
				{
					if(ViewportWidget->MapObjectToolKit.IsValid())
					{
						ViewportWidget->MapObjectToolKit.Pin().Get()->UpdateTreeSelection(Index);
					}
				}
				
			}
				
		}
	}
}

void SMapObjectViewport::Construct(const FArguments& InArgs)
{
	CustomObject = InArgs._EditingObject;
	// We need to create a new Scene before constructing this viewport. Otherwise, it will default to grabbing the one from the main World in the Editor
	AdvancedPreviewScene = MakeShareable(new FAdvancedPreviewScene(FPreviewScene::ConstructionValues()));
	
	SEditorViewport::Construct(SEditorViewport::FArguments());
	
	MapObjectToolKit = InArgs._Toolkit;
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
	
	// LevelViewportClient->SetViewLocation( FVector(1000000.0f,-200.f, 0.f));
	// LevelViewportClient->SetViewRotation( FRotator(-90.0f, -90.0f, 0.0f) );
	// LevelViewportClient->SetViewLocationForOrbiting( FVector::ZeroVector );
	LevelViewportClient->SetViewLocationForOrbiting(FVector(200, -150, 400));
	// LevelViewportClient->bDisableInput = true;
	// LevelViewportClient->inpu
	// LevelViewportClient->SetOrthoZoom(1.0f);
	
	return LevelViewportClient.ToSharedRef();
}