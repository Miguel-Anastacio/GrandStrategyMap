#pragma once
#include "SCommonEditorViewportToolbarBase.h"
#include "SEditorViewport.h"

class FAdvancedPreviewScene;
class UMapObject;

class SMapObjectViewport: public SEditorViewport, public ICommonEditorViewportToolbarInfoProvider
{
public:
	SLATE_BEGIN_ARGS(SMapObjectViewport) : _EditingObject(nullptr) {}

	SLATE_ARGUMENT(UMapObject*, EditingObject )
	
SLATE_END_ARGS()
	
void Construct(const FArguments& InArgs);
	void UpdatePreviewActor();

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	
	//Toolbar interface
	virtual TSharedRef<SEditorViewport> GetViewportWidget() override {return SharedThis(this);}
	virtual TSharedPtr<FExtender> GetExtenders() const override {return MakeShareable(new FExtender);}
	virtual void OnFloatingButtonClicked() override {}
	
	TSoftObjectPtr<UMapObject> CustomObject = nullptr;
	AActor* PreviewActor = nullptr;

protected:
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	
private:
	TSharedPtr<FAdvancedPreviewScene> AdvancedPreviewScene = nullptr;
	TSharedPtr<FEditorViewportClient> LevelViewportClient;
	
};