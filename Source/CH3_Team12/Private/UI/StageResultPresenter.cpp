#include "UI/StageResultPresenter.h"

#include "Framework/Subsystem/KatanaLevelSubsystem.h"
#include "Framework/Subsystem/KatanaStageRecordManagerSubsystem.h"
#include "Framework/Subsystem/KatanaUIManagerSubsystem.h"
#include "UI/StageResultWidget.h"

void UStageResultPresenter::Initialize(UStageResultWidget* InWidget)
{
	StageResultWidget = InWidget;

	UKatanaStageRecordManagerSubsystem* StageRecordManager = UKatanaStageRecordManagerSubsystem::Get(this);
	if (!StageRecordManager)
		return;

	float BestStageTime;
	if (!StageRecordManager->TryGetBestStageTime(UKatanaLevelSubsystem::GetTargetMapName(this), BestStageTime))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get best stage time"));
		BestStageTime = 0.0f;
	}

	const float FinishStageTime = StageRecordManager->GetFinishStageTime();

	StageResultWidget->UpdateWidget(BestStageTime, FinishStageTime);
	StageResultWidget->OnBtnDoneClicked.BindDynamic(this, &UStageResultPresenter::HandleBtnDoneClicked);
}

void UStageResultPresenter::Dispose()
{
	if (!StageResultWidget.IsValid())
		return;

	StageResultWidget->OnBtnDoneClicked.Unbind();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UStageResultPresenter::HandleBtnDoneClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("HandleBtnDoneClicked"));
	UKatanaUIManagerSubsystem* UIManager = UKatanaUIManagerSubsystem::Get(GetWorld());
	if (!UIManager)
		return;

	UE_LOG(LogTemp, Warning, TEXT("HideStageResultWidget"));
	UIManager->HideStageResultWidget();

	UKatanaLevelSubsystem* LevelManager = UKatanaLevelSubsystem::Get(GetWorld());
	if (!LevelManager)
		return;

	UE_LOG(LogTemp, Warning, TEXT("LoadMainMenuLevel"));
	LevelManager->LoadLevel("MainMenuLevel"); // 타이틀 화면으로 이동
}
