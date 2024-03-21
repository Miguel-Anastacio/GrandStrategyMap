// Fill out your copyright notice in the Description page of Project Settings.

//
#include "LayerUI.h"
#include "Blueprint/UserWidget.h"

//
UE_DISABLE_OPTIMIZATION
void ULayerUI::PushToStack(UUserWidget* widget)
{
	if (widget)
	{
		OnWidgetPushed(widget);
		WidgetStack.Push(widget);
	}
	//WidgetPushedDelegate.Broadcast();
}

UUserWidget* ULayerUI::PopFromStack()
{
	if(WidgetStack.IsEmpty())
		return nullptr;

	UUserWidget* widget = WidgetStack.Pop();
	//widget->SetVisibility(ESlateVisibility::Collapsed);
	OnWidgetPopped(widget);

	return widget;
}

UUserWidget* ULayerUI::PeakStack() const
{
	if (WidgetStack.IsEmpty())
		return nullptr;

	return WidgetStack.Top();
}

bool ULayerUI::IsLayerEmpty() const
{
	return WidgetStack.IsEmpty();
}

void ULayerUI::SetVisibilityOfLayer(ESlateVisibility visibility)
{
	for (auto& widget : WidgetStack)
	{
		if (widget)
		{
			widget->SetVisibility(visibility);
		}
	}
	//LayerVisibilityChangedDelegate.Broadcast(visibility);
}

ESlateVisibility ULayerUI::GetVisibilityOfLayer() const
{
	if (WidgetStack.IsEmpty())
		return ESlateVisibility::Collapsed;

	return PeakStack()->GetVisibility();
}

void ULayerUI::ClearStack()
{
	OnLayerCleared();
	WidgetStack.Empty();
}

void ULayerUI::OnWidgetPushedOthers_Implementation(const TArray<ULayerUI*>& otherLayers)
{

}

void ULayerUI::OnWidgetPoppedOthers_Implementation(const TArray<ULayerUI*>& otherLayers)
{

}

void ULayerUI::OnLayerCleared_Implementation()
{
	SetVisibilityOfLayer(HiddenState);
}

void ULayerUI::OnWidgetPushed_Implementation(UUserWidget* widget)
{
	if(widget)
		widget->SetVisibility(VisibleState);

	switch (Type)
	{
	case LayerType::SINGLE:
		SetVisibilityOfLayer(HiddenState);
		//ClearStack();
		break;
	case LayerType::MULTIPLE:
		break;
	default:
		break;
	}
}
void ULayerUI::OnWidgetPopped_Implementation(UUserWidget* widget)
{
	if(widget)
	{
		widget->SetVisibility(HiddenState);
	}

	switch (Type)
	{
	case LayerType::SINGLE:
		// hide popped widget
		// how widget on top of the stack
		if (WidgetStack.IsEmpty())
			return;
		WidgetStack.Top()->SetVisibility(VisibleState);
		break;
	case LayerType::MULTIPLE:

		break;
	default:
		break;
	}
}
UE_ENABLE_OPTIMIZATION