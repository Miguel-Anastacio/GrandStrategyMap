// Copyright 2025 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
// One mode for each set of tabs that are associated with your application
class FMapEditorGenAppMode : public FApplicationMode
{
public:
	FMapEditorGenAppMode(TSharedPtr<class FMapEditorApp> app);

	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;

protected:
	TWeakPtr<class FMapEditorApp> App;
	FWorkflowAllowedTabSet TabSet;
};