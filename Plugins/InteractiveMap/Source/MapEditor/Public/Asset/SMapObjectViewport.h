#pragma once
#include "SCommonEditorViewportToolbarBase.h"
#include "SEditorViewport.h"

class FAdvancedPreviewScene;
class UMapObject;
class AMapAsset;

DECLARE_DELEGATE_OneParam(FOnClickedOnMapSignature, uint32);
class FMapObjectViewportClient : public FEditorViewportClient
{
public:
	FMapObjectViewportClient(FAdvancedPreviewScene* InPreviewScene, const TSharedRef<SEditorViewport>& InViewport, UMapObject* InMapObject);
	virtual void ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) override;
	virtual bool InputKey(const FInputKeyEventArgs& EventArgs) override;
	virtual void Tick(float DeltaSeconds) override;
	int32 GetIndexOfTileSelected(int32 ScreenX, int32 ScreenY);
	FOnClickedOnMapSignature OnClickedOnMapDelegate;

private:
	UMapObject* MapObject;
	TWeakObjectPtr<AMapAsset> MapAssetRef;
	bool bTileSelected = false;
};



class SMapObjectViewport: public SEditorViewport, public ICommonEditorViewportToolbarInfoProvider
{
public:
	SLATE_BEGIN_ARGS(SMapObjectViewport) : _EditingObject(nullptr) {}

	SLATE_ARGUMENT(UMapObject*, EditingObject)
	SLATE_ARGUMENT(TWeakPtr<class FMapEditorApp>, app)
	
SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs);
	void UpdatePreviewActor(int32 ID) const;
	void UpdatePreviewActorMaterial(UMaterial* ParentMaterial, UTexture2D* Texture2D) const;

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	
	//Toolbar interface
	virtual TSharedRef<SEditorViewport> GetViewportWidget() override {return SharedThis(this);}
	virtual TSharedPtr<FExtender> GetExtenders() const override {return MakeShareable(new FExtender);}
	virtual void OnFloatingButtonClicked() override {}
	
	TSoftObjectPtr<UMapObject> CustomObject = nullptr;
	TWeakPtr<class FMapEditorApp> MapEditorApp = nullptr;

protected:
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	
private:
	TSharedPtr<FAdvancedPreviewScene> AdvancedPreviewScene = nullptr;
	TSharedPtr<FEditorViewportClient> LevelViewportClient;
	TWeakObjectPtr<AMapAsset> MapAsset;
	
};