#pragma once
#include "SCommonEditorViewportToolbarBase.h"
#include "SEditorViewport.h"

class FAdvancedPreviewScene;
class UMapObject;
class AMapAsset;

class FMapObjectViewportClient : public FEditorViewportClient
{
public:
	FMapObjectViewportClient(FAdvancedPreviewScene* InPreviewScene, const TSharedRef<SEditorViewport>& InViewport, UMapObject* InMapObject);
	virtual void ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) override;
	virtual bool InputKey(const FInputKeyEventArgs& EventArgs) override;

private:
	void ProcessMouseClick(FSceneView& View, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY);
	int32 GetIndexOfTileSelected(const FSceneView& View, const class FMapEditorApp* App, int32 ScreenX, int32 ScreenY);
	static bool WasKeyReleased(const FKey& Key, const FKey& KeyQueried, const EInputEvent Event);
	UMapObject* MapObject;
	TWeakObjectPtr<AMapAsset> MapAssetRef;
};


class SMapObjectViewport: public SEditorViewport, public ICommonEditorViewportToolbarInfoProvider
{
public:
	SLATE_BEGIN_ARGS(SMapObjectViewport) : _EditingObject(nullptr) {}

	SLATE_ARGUMENT(UMapObject*, EditingObject)
	SLATE_ARGUMENT(TWeakPtr<class FMapEditorApp>, app)
	
SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs);
	void UpdatePreviewActor() const;
	void UpdatePreviewActorMaterial(UMaterial* ParentMaterial, UTexture2D* Texture2D) const;
	
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