#include "UI/Widgets/ProvinceEditorWidget.h"
#include "Components/EditableText.h"
#include "Components/RichTextBlock.h"
#include "Map/InteractiveMap.h"
//#include "ProvinceEditorWidget.h"

void UProvinceEditorWidget::SetProvinceData(const FProvinceData& data, FName provinceID)
{
	if (ProvinceName)
	{
		ProvinceName->SetText(FText::FromString(data.ProvinceName));
	}
	if (OwnerName)
	{
		OwnerName->SetText(FText::FromName(data.Owner));
	}
	if (NameInput)
		NameInput->SetText(FText::FromString(FString("______")));
	if (OwnerInput)
		OwnerInput->SetText(FText::FromString(FString("______")));
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
	
	if (NameInput)
		NameInput->OnTextCommitted.AddDynamic(this, &UProvinceEditorWidget::UpdateProvinceName);
	if (OwnerInput)
		OwnerInput->OnTextCommitted.AddDynamic(this, &UProvinceEditorWidget::UpdateProvinceOwner);
	//if (OwnerInput)
	//	OwnerInput->OnTextCommitted.AddDynamic(this, &UUWTableParameters::UpdateTableLegHeight);
}

void UProvinceEditorWidget::UpdateProvinceName(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		if (ProvinceName)
		{
			ProvinceName->SetText(Text);
		}

		if (GameMapReference)
		{
			ProvinceSelectedData.ProvinceName = Text.ToString();
			GameMapReference->UpdateProvinceData(ProvinceSelectedData, ProvinceSelectedID);
		}


		/*FString string = Text.ToString();
		if (string.IsNumeric())
		{
			float width = FCString::Atof(*string);
			for (auto& table : m_AllTablesSelected)
				table->UpdateMeshWidth(width);
		}*/
		NameInput->SetText(FText::FromString(FString("______")));
	}
}

void UProvinceEditorWidget::UpdateProvinceOwner(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		if (OwnerName)
		{
			OwnerName->SetText(Text);
		}

		if (GameMapReference)
		{
			FString dummy = Text.ToString();
			ProvinceSelectedData.Owner = FName(*dummy);
			GameMapReference->UpdateProvinceData(ProvinceSelectedData, ProvinceSelectedID);
		}

		OwnerInput->SetText(FText::FromString(FString("______")));
	}


		/*FString string = Text.ToString();
		if (string.IsNumeric())
		{
			float width = FCString::Atof(*string);
			for (auto& table : m_AllTablesSelected)
				table->UpdateMeshWidth(width);
		}*/
}

