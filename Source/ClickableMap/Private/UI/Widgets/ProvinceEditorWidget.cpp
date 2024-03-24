#include "UI/Widgets/ProvinceEditorWidget.h"
#include "UI/Widgets/CustomEditableText.h"
#include "Components/EditableText.h"
#include "Components/RichTextBlock.h"
#include "Map/InteractiveMap.h"
//#include "ProvinceEditorWidget.h"
UE_DISABLE_OPTIMIZATION
void UProvinceEditorWidget::SetProvinceData(const FProvinceData& data, FName provinceID)
{
	NameCustomInput->SetValues(FText::FromString(data.ProvinceName), FText::FromString(FString("______")));
	OwnerCustomInput->SetValues(FText::FromString(data.Owner.ToString()), FText::FromString(FString("______")));
	ReligionCustomInput->SetValues(FText::FromString(data.Religion.DataName), FText::FromString(FString("______")));
	CultureCustomInput->SetValues(FText::FromString(data.Culture.DataName), FText::FromString(FString("______")));
	PopulationCustomInput->SetValues(FText::AsNumber(data.Population), FText::FromString(FString("______")));

	ProvinceSelectedData = data;
	ProvinceSelectedID = provinceID;
}

void UProvinceEditorWidget::SetInteractiveMapReference(AInteractiveMap* map)
{
	GameMapReference = map;
}

void UProvinceEditorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
		
	OwnerCustomInput->TextCommitDelegate.AddDynamic(this, &UProvinceEditorWidget::UpdateProvinceData);
	NameCustomInput->TextCommitDelegate.AddDynamic(this, &UProvinceEditorWidget::UpdateProvinceData);
	ReligionCustomInput->TextCommitDelegate.AddDynamic(this, &UProvinceEditorWidget::UpdateProvinceData);
	CultureCustomInput->TextCommitDelegate.AddDynamic(this, &UProvinceEditorWidget::UpdateProvinceData);
	PopulationCustomInput->TextCommitDelegate.AddDynamic(this, &UProvinceEditorWidget::UpdateProvinceData);

}

void UProvinceEditorWidget::UpdateProvinceData(UCustomEditableText* editedText, const FText& Text, ETextCommit::Type CommitMethod)
{
	if (editedText == OwnerCustomInput)
	{
		if (CommitMethod == ETextCommit::OnEnter)
		{
			if (GameMapReference)
			{
				FString dummy = Text.ToString();
				ProvinceSelectedData.Owner = FName(*dummy);
				GameMapReference->UpdateProvinceData(ProvinceSelectedData, ProvinceSelectedID);

			}
			editedText->SetValues(Text, FText::FromString(FString("______")));
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
				GameMapReference->UpdateProvinceData(ProvinceSelectedData, ProvinceSelectedID);

				editedText->SetValues(Text, FText::FromString(FString("______")));
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
				ProvinceSelectedData.Religion.DataName = value;
				GameMapReference->UpdateProvinceData(ProvinceSelectedData, ProvinceSelectedID);

				editedText->SetValues(Text, FText::FromString(FString("______")));
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
				ProvinceSelectedData.Culture.DataName = value;
				GameMapReference->UpdateProvinceData(ProvinceSelectedData, ProvinceSelectedID);

				editedText->SetValues(Text, FText::FromString(FString("______")));
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
				GameMapReference->UpdateProvinceData(ProvinceSelectedData, ProvinceSelectedID);
				editedText->SetValues(Text, FText::FromString(FString("______")));
			}
		}
		return;
	}

}


UE_ENABLE_OPTIMIZATION