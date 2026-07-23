#include "Framework/DataAsset/UIDataAsset.h"

#if WITH_EDITOR
#include "UObject/UnrealType.h"
#endif

#if WITH_EDITOR
void UUIDataAsset::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedChainEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedChainEvent);

	FProperty* TopProperty = PropertyChangedChainEvent.PropertyChain.GetHead()->GetValue();

	if (TopProperty && TopProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UUIDataAsset, WidgetInfoList))
	{
		WidgetInfoMap.Empty();
		for (const FUIWidgetInfo& Info : WidgetInfoList)
		{
			if (!Info.WidgetName.IsNone())
			{
				WidgetInfoMap.Add(Info.WidgetName, Info);
			}
		}

		this->Modify();
	}
}
#endif
