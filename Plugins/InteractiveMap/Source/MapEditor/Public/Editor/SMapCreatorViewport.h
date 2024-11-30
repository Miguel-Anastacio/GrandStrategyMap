#pragma once
#include "SCommonEditorViewportToolbarBase.h"
#include "SEditorViewport.h"

class FAdvancedPreviewScene;
class UMapEditorPreset;

class SMapCreatorViewport: public SEditorViewport, public ICommonEditorViewportToolbarInfoProvider
{
public:
	SLATE_BEGIN_ARGS(SMapCreatorViewport) : _EditingObject(nullptr) {}
		SLATE_ARGUMENT(UMapEditorPreset*, EditingObject )
	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs);

	//Toolbar interface
	virtual TSharedRef<SEditorViewport> GetViewportWidget() override {return SharedThis(this);}
	virtual TSharedPtr<FExtender> GetExtenders() const override {return MakeShareable(new FExtender);}
	virtual void OnFloatingButtonClicked() override {}
	
	TSharedPtr<FAdvancedPreviewScene> AdvancedPreviewScene = nullptr;
	TSoftObjectPtr<UMapEditorPreset> StylePreset = nullptr;

protected:
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	TSharedPtr<FEditorViewportClient> LevelViewportClient;
};