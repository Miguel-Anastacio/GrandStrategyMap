// Copyright 2025 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Editor/SMapTextureViewer.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"
// One tab factory for each tab you want to have in application
class  FMapDataEditorTabFactory : public  FWorkflowTabFactory
{
public:
	FMapDataEditorTabFactory(TSharedPtr<class FMapEditorApp> app);
	
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

protected:
	TWeakPtr<class FMapEditorApp> App;
	FTextureChanged TextureChangedDelegate;
};
