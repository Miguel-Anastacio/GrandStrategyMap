
#include "GenericStructWidgetFactory.h"
#include "GenericStructWidget.h"
#include "Styling/SlateStyle.h"

UGenericStructWidgetFactory::UGenericStructWidgetFactory(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	SupportedClass = UGenericStructWidget::StaticClass();
	bCreateNew = true;
	bEditorImport = false;
	bEditAfterNew = true;
}

UObject* UGenericStructWidgetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	if (!Class || !Class->IsChildOf(UGenericStructWidget::StaticClass()))
	{
		Class = UGenericStructWidget::StaticClass();
	}
	return NewObject<UGenericStructWidget>(InParent, Class, Name, Flags | RF_Transactional);
}

