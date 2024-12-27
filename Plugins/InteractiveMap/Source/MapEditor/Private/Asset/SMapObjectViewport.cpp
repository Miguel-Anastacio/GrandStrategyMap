#include "Asset/SMapObjectViewport.h"
#include "AdvancedPreviewScene.h"
#include "EngineUtils.h"
#include "Asset/MapObject.h"
#include "MapEditor.h"
#include "Asset/MapAssetActor.h"
#include "Math/Color.h"  
#include "GenericPlatform/GenericPlatformApplicationMisc.h"
#include "HAL/PlatformApplicationMisc.h"

FMapObjectViewportClient::FMapObjectViewportClient(FAdvancedPreviewScene* InPreviewScene, const TSharedRef<SEditorViewport>& InViewport)
		: FEditorViewportClient(nullptr, InPreviewScene, InViewport)
{
	
}

void FMapObjectViewportClient::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event,
	uint32 HitX, uint32 HitY)
{
	if (Key == EKeys::LeftMouseButton && Event == EInputEvent::IE_Released)
	{
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
		
		const AMapAsset* MapHit = Cast<AMapAsset>(ActorHitProxy->Actor);
		if(!MapHit)
			return;
		FIntPoint MousePos;
		Viewport->GetMousePos(MousePos, false);
		const FLinearColor Color = FPlatformApplicationMisc::GetScreenPixelColor(FVector2D(MousePos));
		// FLinearColor Color = FLinearColor::White;
		
		UE_LOG(LogInteractiveMapEditor, Log, TEXT("Color on screen: %s"), *Color.ToString());
		UE_LOG(LogInteractiveMapEditor, Log, TEXT("Pos X:%d, Y:%d"), MousePos.X, MousePos.Y);
		UE_LOG(LogInteractiveMapEditor, Log, TEXT("Hit Pos X:%d, Y:%d"), HitX, HitY);
		// UE_LOG(LogInteractiveMapEditor, Log, TEXT("Color on screen: %s"), *Color.ToString());
		
	}
	else
	{
		UE_LOG(LogInteractiveMapEditor, Log, TEXT("No hit proxy detected at the clicked location."));
	}

	// Optionally, pass unhandled clicks to the base class
	FEditorViewportClient::ProcessClick(View, HitProxy, Key, Event, HitX, HitY);
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