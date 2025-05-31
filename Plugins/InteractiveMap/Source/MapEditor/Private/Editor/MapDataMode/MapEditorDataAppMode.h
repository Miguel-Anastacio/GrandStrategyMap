// Copyright 2025 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
struct FInstancedStruct;
class SCustomInstancedStructList;

// One mode for each set of tabs that are associated with your application
class FMapEditorDataAppMode : public FApplicationMode, public FGCObject
{
public:
	FMapEditorDataAppMode(TSharedPtr<class FMapEditorApp> app);

	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;

	void Init();
	
	TSharedPtr<SCustomInstancedStructList> EditableStructListDisplay = nullptr;
	TArray<TSharedPtr<FInstancedStruct>> MyListItems;
	TArray<UScriptStruct*> StructTypes;
	TSet<FName> PropertyNamesNotEditable;
	TSet<FName> PropertiesWithDropDown;
	
	TObjectPtr<class UInstancedStructWrapper> EntryWrapper = nullptr;

	void UpdateMap(const FInstancedStruct& Data, int ID);
protected:
	void RefreshDataList();
	/** FGCObject interface */
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		if(EntryWrapper)
		{
			Collector.AddReferencedObject(EntryWrapper);
		}
	}
	virtual FString GetReferencerName() const override
	{
		return TEXT("FMapEditorDataAppMode");
	}
	
	TWeakPtr<class FMapEditorApp> App;
	FWorkflowAllowedTabSet TabSet;
	FName DataEditorListTabID;
	FName DataEditorEntryTabID;

};