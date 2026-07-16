#include "Framework/Subsystem/KatanaGraphicManagerSubsystem.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"

UKatanaGraphicManagerSubsystem* UKatanaGraphicManagerSubsystem::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
		return nullptr;

	const UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaGraphicManagerSubsystem: World is null"));
		return nullptr;
	}

	const UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaGraphicManagerSubsystem: GameInstance is null"));
		return nullptr;
	}

	return GameInstance->GetSubsystem<UKatanaGraphicManagerSubsystem>();
}

void UKatanaGraphicManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UserSettings = UGameUserSettings::GetGameUserSettings();
	if (!UserSettings)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaGraphicManagerSubsystem: GameUserSettings is null"));
		return;
	}

	LoadGraphicSettings();
}

void UKatanaGraphicManagerSubsystem::ApplyAndSaveGraphicSettings()
{
	if (!UserSettings)
		return;

	UserSettings->ApplyResolutionSettings(false);
	UserSettings->ApplySettings(false);
	UserSettings->SaveSettings();
	UE_LOG(LogTemp, Log, TEXT("KatanaGraphicManagerSubsystem: 그래픽 설정이 적용 및 저장되었습니다."));
}

void UKatanaGraphicManagerSubsystem::LoadGraphicSettings()
{
	if (!UserSettings)
		return;

	UserSettings->LoadSettings(false);
	UserSettings->ApplySettings(false);
}

void UKatanaGraphicManagerSubsystem::SetResolution(FIntPoint NewResolution)
{
	if (!UserSettings)
		return;

	UserSettings->SetScreenResolution(NewResolution);
}

FIntPoint UKatanaGraphicManagerSubsystem::GetResolution() const
{
	if (!UserSettings)
		return FKatanaGraphicSettingDefaults::Resolution;

	return UserSettings->GetScreenResolution();
}

void UKatanaGraphicManagerSubsystem::SetWindowMode(EKatanaWindowMode NewWindowMode)
{
	if (!UserSettings)
		return;

	EWindowMode::Type EngineWindowMode = EWindowMode::Windowed;

	switch (NewWindowMode)
	{
	case EKatanaWindowMode::Fullscreen:
		EngineWindowMode = EWindowMode::Fullscreen;
		break;
	case EKatanaWindowMode::WindowedFullscreen:
		EngineWindowMode = EWindowMode::WindowedFullscreen;
		break;
	case EKatanaWindowMode::Windowed:
		EngineWindowMode = EWindowMode::Windowed;
		break;
	}

	UserSettings->SetFullscreenMode(EngineWindowMode);
}

EKatanaWindowMode UKatanaGraphicManagerSubsystem::GetWindowMode() const
{
	if (!UserSettings)
		return FKatanaGraphicSettingDefaults::WindowMode;

	const EWindowMode::Type EngineWindowMode = UserSettings->GetFullscreenMode();

	if (EngineWindowMode == EWindowMode::Fullscreen)
		return EKatanaWindowMode::Fullscreen;

	if (EngineWindowMode == EWindowMode::WindowedFullscreen)
		return EKatanaWindowMode::WindowedFullscreen;

	return EKatanaWindowMode::Windowed;
}

void UKatanaGraphicManagerSubsystem::SetGraphicQuality(EKatanaGraphicQuality NewQuality)
{
	if (!UserSettings)
		return;

	const int32 QualityLevel = static_cast<int32>(NewQuality);
	UserSettings->SetOverallScalabilityLevel(QualityLevel);
}

EKatanaGraphicQuality UKatanaGraphicManagerSubsystem::GetGraphicQuality() const
{
	if (!UserSettings)
		return FKatanaGraphicSettingDefaults::Quality;

	int32 QualityLevel = UserSettings->GetOverallScalabilityLevel();

	QualityLevel = FMath::Clamp(QualityLevel, 0, 3);
	return static_cast<EKatanaGraphicQuality>(QualityLevel);
}

void UKatanaGraphicManagerSubsystem::SetVSyncEnabled(bool bEnable)
{
	if (!UserSettings)
		return;

	UserSettings->SetVSyncEnabled(bEnable);
}

bool UKatanaGraphicManagerSubsystem::IsVSyncEnabled() const
{
	if (!UserSettings)
		return FKatanaGraphicSettingDefaults::bVSync;

	return UserSettings->IsVSyncEnabled();
}

void UKatanaGraphicManagerSubsystem::SetFrameRateLimit(float MaxFPS)
{
	if (!UserSettings)
		return;

	UserSettings->SetFrameRateLimit(MaxFPS);
}

float UKatanaGraphicManagerSubsystem::GetFrameRateLimit() const
{
	if (!UserSettings)
		return FKatanaGraphicSettingDefaults::FrameRateLimit;

	return UserSettings->GetFrameRateLimit();
}
