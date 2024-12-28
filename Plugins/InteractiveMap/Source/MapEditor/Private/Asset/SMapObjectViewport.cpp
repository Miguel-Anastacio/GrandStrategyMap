#include "Asset/SMapObjectViewport.h"
#include "AdvancedPreviewScene.h"
#include "EngineUtils.h"
#include "Asset/MapObject.h"
#include "MapEditor.h"
#include "Asset/MapAssetActor.h"
#include "Math/Color.h"  
#include "HAL/PlatformApplicationMisc.h"
#include "Kismet/GameplayStatics.h"

FMapObjectViewportClient::FMapObjectViewportClient(FAdvancedPreviewScene* InPreviewScene, const TSharedRef<SEditorViewport>& InViewport)
		: FEditorViewportClient(nullptr, InPreviewScene, InViewport)
{
	
}

void FMapObjectViewportClient::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event,
	uint32 HitX, uint32 HitY)
{
	if (Key == EKeys::LeftMouseButton && Event == EInputEvent::IE_Released)
	{
		//// TEST
		FIntPoint MousePos;
		Viewport->GetMousePos(MousePos, false);
		GetHitLocationInEditor(this, HitX, HitY);
		
		if (!HitProxy)
			return;
		
		if (!HitProxy->IsA(HActor::StaticGetType()))
		{
			return;
		}
		
		HActor* ActorHitProxy = static_cast<HActor*>(HitProxy);
		if (!ActorHitProxy)
		{
			return;
		}
		if(!ActorHitProxy->Actor)
			return;
		
		AMapAsset* MapHit = Cast<AMapAsset>(ActorHitProxy->Actor);
		if(!MapHit)
			return;
		
		// FHitResult Hit;
		// Hit.Component = MapHit->MapMesh;
		// FVector2D uvs;
		// // UGameplayStatics::FindCollisionUV(Hit, 0, uvs);
		
		// const FLinearColor Color = FPlatformApplicationMisc::GetScreenPixelColor(FVector2D(MousePos));
		// UTextureRenderTarget2D* test = NewObject<UTextureRenderTarget2D>(MapHit);
		// UKismetRenderingLibrary::pix
		
		// const int Index = MapHit->MapObject->GetIndexOfTileSelected(Color.ToFColor(true));
		
		// UE_LOG(LogInteractiveMapEditor, Log, TEXT("Color on screen: %s"), *Color.ToString());
		// UE_LOG(LogInteractiveMapEditor, Log, TEXT("Index: %d"), Index);
		
		
	}
	else
	{
		UE_LOG(LogInteractiveMapEditor, Log, TEXT("No hit proxy detected at the clicked location."));
	}

	// Optionally, pass unhandled clicks to the base class
	FEditorViewportClient::ProcessClick(View, HitProxy, Key, Event, HitX, HitY);
}

void FMapObjectViewportClient::GetHitLocationInEditor(FEditorViewportClient* ViewportClient, int32 ScreenX, int32 ScreenY)
{
	if (Viewport && ViewportClient)
	{
		// Create a scene view to perform the deprojection
		FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
			Viewport,
			ViewportClient->GetWorld()->Scene,
			ViewportClient->EngineShowFlags));
         
		FSceneView* SceneView = ViewportClient->CalcSceneView(&ViewFamily);

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

			if (ViewportClient->GetWorld()->LineTraceSingleByChannel(HitResult, WorldOrigin, TraceEnd, ECC_Visibility, CollisionParams))
			{
				// Output the hit location
				FVector HitLocation = HitResult.Location;
				UE_LOG(LogTemp, Log, TEXT("Hit World Location: %s"), *HitLocation.ToString());
				FVector2d uvs;
				UGameplayStatics::FindCollisionUV(HitResult, 0, uvs);
				AMapAsset* MapAsset = Cast<AMapAsset>(HitResult.GetActor());
				if(!MapAsset)
					return;
				
				FColor Color = MapAsset->MapObject->GetColorFromUv(uvs);
				const int Index = MapAsset->MapObject->GetIndexOfTileSelected(Color);
		
				// UE_LOG(LogInteractiveMapEditor, Log, TEXT("Color on screen: %s"), *Color.ToString());
				UE_LOG(LogInteractiveMapEditor, Log, TEXT("Index: %d"), Index);
				UE_LOG(LogInteractiveMapEditor, Log, TEXT("UVs: %f, %f"), uvs.X, uvs.Y);
				
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No hit detected at screen position (%d, %d)."), ScreenX, ScreenY);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to calculate SceneView."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Viewport or ViewportClient."));
	}
}

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
	// GetWorld()->

	int result = GetWorld()->GetNumControllers();
	UE_LOG(LogInteractiveMapEditor, Display, TEXT("%d"), result);
	PreviewMapAsset->MapObject = CustomObject.Get();
	PreviewMapAsset->RerunConstructionScripts();
}

SMapObjectViewport::~SMapObjectViewport()
{

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
	LevelViewportClient = MakeShareable(new FMapObjectViewportClient(AdvancedPreviewScene.Get(), SharedThis(this)));
	
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