// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
DECLARE_LOG_CATEGORY_EXTERN(LogDataBasedWidget, Log, All);
class FDataBasedWidgetModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
