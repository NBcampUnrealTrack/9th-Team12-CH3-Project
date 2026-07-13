#include "Framework/DataAsset/UIDataAsset.h"

#if WITH_EDITOR
#include "UObject/UnrealType.h"
#endif

#if WITH_EDITOR
void UUIDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// 에디터에서 WidgetInfoList 데이터가 수정되거나 순서가 바뀌면 실행됨
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UUIDataAsset, WidgetInfoList))
	{
		WidgetInfoMap.Empty();
		for (const FUIWidgetInfo& Info : WidgetInfoList)
		{
			// 구조체 내부의 이름 변수를 Key로 삼아 Map에 재배치
			WidgetInfoMap.Add(Info.WidgetName, Info);
		}
	}
}
#endif
