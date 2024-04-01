#include "UI/Widgets/VisualPropertyEditorWidget.h"
//#include "UI/Widgets/CustomEditableText.h"
//#include "Components/EditableText.h"
//#include "Components/RichTextBlock.h"
//#include "Map/InteractiveMap.h"
////#include "ProvinceEditorWidget.h"
//UE_DISABLE_OPTIMIZATION

/*
void UVisualPropertyEditorWidget::SetInteractiveMapReference(AInteractiveMap* map)
{
	GameMapReference = map;
}

void UVisualPropertyEditorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
		
	OwnerCustomInput->TextCommitDelegate.AddDynamic(this, &UVisualPropertyEditorWidget::UpdateProvinceData);
	NameCustomInput->TextCommitDelegate.AddDynamic(this, &UVisualPropertyEditorWidget::UpdateProvinceData);
	ReligionCustomInput->TextCommitDelegate.AddDynamic(this, &UVisualPropertyEditorWidget::UpdateProvinceData);
	CultureCustomInput->TextCommitDelegate.AddDynamic(this, &UVisualPropertyEditorWidget::UpdateProvinceData);
	PopulationCustomInput->TextCommitDelegate.AddDynamic(this, &UVisualPropertyEditorWidget::UpdateProvinceData);

}

void UVisualPropertyEditorWidget::UpdateProvinceData(UCustomEditableText* editedText, const FText& Text, ETextCommit::Type CommitMethod)
{
	if (editedText == OwnerCustomInput)
	{
		if (CommitMethod == ETextCommit::OnEnter)
		{
			if (GameMapReference)
			{
				FString dummy = Text.ToString();
				ProvinceSelectedData.Owner = FName(*dummy);
				if (GameMapReference->UpdateProvinceData(ProvinceSelectedData, ProvinceSelectedID))
				{
					editedText->SetValues(Text, FText::FromString(FString("______")));
				}

			}
		}
		return;
	}

	if (editedText == NameCustomInput)
	{
		if (CommitMethod == ETextCommit::OnEnter)
		{
			if (GameMapReference)
			{
				FString value = Text.ToString();
				ProvinceSelectedData.ProvinceName = value;
				if (GameMapReference->UpdateProvinceData(ProvinceSelectedData, ProvinceSelectedID))
				{
					editedText->SetValues(Text, FText::FromString(FString("______")));
				}

			}
		}
		return;
	}

	if (editedText == ReligionCustomInput)
	{
		if (CommitMethod == ETextCommit::OnEnter)
		{
			if (GameMapReference)
			{
				FString value = Text.ToString();
				ProvinceSelectedData.Religion = FName(*value);
				if (GameMapReference->UpdateProvinceData(ProvinceSelectedData, ProvinceSelectedID))
				{
					editedText->SetValues(Text, FText::FromString(FString("______")));
				}

			}
		}
		return;
	}

	if (editedText == CultureCustomInput)
	{
		if (CommitMethod == ETextCommit::OnEnter)
		{
			if (GameMapReference)
			{
				FString value = Text.ToString();
				ProvinceSelectedData.Culture = FName(*value);
				if(GameMapReference->UpdateProvinceData(ProvinceSelectedData, ProvinceSelectedID))
				{
					editedText->SetValues(Text, FText::FromString(FString("______")));
				}
			}
		}
		return;
	}

	if (editedText == PopulationCustomInput)
	{
		if (CommitMethod == ETextCommit::OnEnter)
		{
			if (!Text.IsNumeric())
			{
				UE_LOG(LogTemp, Warning, TEXT("Invalid value in population, please insert a number"));
				editedText->SetValues(Text, FText::FromString(FString("______")));
			}
			if (GameMapReference)
			{
				FString value = Text.ToString();
				ProvinceSelectedData.Population = FCString::Atoi(*value);
				if (GameMapReference->UpdateProvinceData(ProvinceSelectedData, ProvinceSelectedID))
				{
					editedText->SetValues(Text, FText::FromString(FString("______")));

				}
			}
		}
		return;
	}

}*/


//UE_ENABLE_OPTIMIZATION