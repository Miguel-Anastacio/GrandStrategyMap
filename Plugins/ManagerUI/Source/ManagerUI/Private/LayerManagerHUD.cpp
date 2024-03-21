// Fill out your copyright notice in the Description page of Project Settings.


#include "LayerManagerHUD.h"
#include "ManagerUI.h"
#include "LayerUI.h"
#include "Blueprint/UserWidget.h"
void ALayerManagerHUD::RegisterDefaultLayer(const FString& name)
{
	LayersUI.Emplace(name, NewObject<ULayerUI>());
}
UE_DISABLE_OPTIMIZATION
void ALayerManagerHUD::RegisterLayer(const FString& name, ULayerUI* layer, bool current)
{
	if (layer)
	{
		// If registering a layer that already exists prevent it and alert the user
		// Register layer is supposed to be used before PushToLayer
		ULayerUI** existingLayer = LayersUI.Find(name);
		if (existingLayer)
		{
			(*existingLayer)->Type = layer->Type;
			UE_LOG(LogManagerUI, Warning, TEXT("Tried to register layer that already exists."));

			return;
		}

		LayersUI.Emplace(name, layer);
		if (current)
			CurrentLayerID = name;
	}
	else
	{
		UE_LOG(LogManagerUI, Error, TEXT("Tried to register invalid layer"));
	}
}

void ALayerManagerHUD::RemoveLayer(const FString& name)
{
	ULayerUI* layer;
	if (LayersUI.RemoveAndCopyValue(name, layer))
	{
		layer->ClearStack();
	}
	//LayersUI.Remove(name);
}

void ALayerManagerHUD::PushToLayer(const FString& name, UUserWidget* widget)
{
	if(!IsValid(widget))
	{
		UE_LOG(LogManagerUI, Error, TEXT("Pushing Null widget"));
	}

	ULayerUI* layer = nullptr;
	TArray<ULayerUI*> layersValue;
	// if it already exists
	// save data remove it, generate array from map, and add layer again
	if (LayersUI.RemoveAndCopyValue(name, layer))
	{
		LayersUI.GenerateValueArray(layersValue);
		LayersUI.Add(name, layer);
	}
	else
	{
		LayersUI.GenerateValueArray(layersValue);
	}

	// layersValue now contains all layers except the one we are pushing to

	// did not find a layer so add a generic one
	if (!IsValid(layer))
	{
		layer = NewObject<ULayerUI>();
		LayersUI.Add(name, layer);
	}

	layer->PushToStack(widget);
	// pass all other layers 
	layer->OnWidgetPushedOthers(layersValue);

	if (CurrentLayerID != name)
	{
		LayerOrder.Add(name);
	}

	CurrentLayerID = name;
}
UUserWidget* ALayerManagerHUD::PopFromLayer(const FString& name)
{
	ULayerUI* layer = NULL;
	TArray<ULayerUI*> layersValue;
	// if it already exists
	// save data remove it, generate array from map, and add layer again
	if (LayersUI.RemoveAndCopyValue(name, layer))
	{
		LayersUI.GenerateValueArray(layersValue);
		LayersUI.Add(name, layer);
	}
	else
	{
		LayersUI.GenerateValueArray(layersValue);
	}

	if (layer)
	{
		layer->OnWidgetPoppedOthers(layersValue);
		UUserWidget* widget = layer->PopFromStack();

		if (layer->IsLayerEmpty() && LayerOrder.Num() > 0)
		{
			LayerOrder.Pop();
			if (LayerOrder.Num() > 0)
			{
				CurrentLayerID = LayerOrder.Top();
			}
			else
			{
				CurrentLayerID = FString("");
			}
		}

		return widget;
	}
	
	return nullptr;
}

UUserWidget* ALayerManagerHUD::PopFromCurrentLayer()
{
	return PopFromLayer(CurrentLayerID);
}

void ALayerManagerHUD::ClearAllLayers()
{
	for (auto& elem : LayersUI)
	{
		elem.Value->ClearStack();
	}
	LayersUI.Empty();
}

void ALayerManagerHUD::ClearLayer(const FString& name)
{
	auto layer = LayersUI.Find(name);
	if (layer)
	{
		(*layer)->ClearStack();
	}
}

FString ALayerManagerHUD::GetCurrentLayerTag() const
{
	return CurrentLayerID;
}

ULayerUI* ALayerManagerHUD::GetLayer(const FString& name) const
{
	return (*LayersUI.Find(name));
}

UUserWidget* ALayerManagerHUD::PeakLayer(const FString& name) const 
{
	const auto layer = LayersUI.Find(name);
	if (layer)
	{
		return (*layer)->PeakStack();
	}

	return nullptr;
}
bool ALayerManagerHUD::IsWidgetOnTopOfLayer(const FString& name, UUserWidget* widget) const
{
	const auto layer = LayersUI.Find(name);
	if (layer)
	{
		if ((*layer)->IsLayerEmpty())
			return false;

		if((*layer)->PeakStack() == widget)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}
UE_ENABLE_OPTIMIZATION

