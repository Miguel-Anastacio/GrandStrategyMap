// Copyright 2024 An@stacioDev All rights reserved.
#include "Editor/InstancedStructWrapper.h"

#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "MapEditorApp.h"

void UInstancedStructWrapper::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	// Block updates of changes in structInstance
	if(Previous.IsValid() && Previous.GetScriptStruct() != StructInstance.GetScriptStruct())
	{
		StructInstance = Previous;
		const FText Title = FText::FromString(TEXT("Error: Not allowed change"));
		const FText Message = FText::FromString(TEXT("Type of struct of data of entry cannot be changed after creation"));
		EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::Ok, Message, Title);
	}
	else if(App.IsValid())
	{
		Previous = StructInstance;
		App.Pin().Get()->UpdateMapData(StructInstance);
	}
}

void UInstancedStructWrapper::SetApp(TWeakPtr<class FMapEditorApp> app)
{
	App = app;
}

void UInstancedStructWrapper::SetStructInstance(const FInstancedStruct& Struct)
{
	StructInstance = Struct;
	Previous = StructInstance;
}
