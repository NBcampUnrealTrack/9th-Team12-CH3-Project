#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameFramework/GameUserSettings.h"
#include "KatanaGraphicManagerSubsystem.generated.h"

UENUM(BlueprintType)
enum class EKatanaWindowMode : uint8
{
    Fullscreen         UMETA(DisplayName = "Fullscreen"),
    WindowedFullscreen UMETA(DisplayName = "Borderless (Windowed Fullscreen)"),
    Windowed           UMETA(DisplayName = "Windowed")
};

UENUM(BlueprintType)
enum class EKatanaGraphicQuality : uint8
{
    Low     UMETA(DisplayName = "Low"),
    Medium  UMETA(DisplayName = "Medium"),
    High    UMETA(DisplayName = "High"),
    Epic    UMETA(DisplayName = "Epic")
};

struct FKatanaGraphicSettingDefaults
{
    inline static const FIntPoint Resolution = {1920, 1080};
    static constexpr EKatanaWindowMode WindowMode = EKatanaWindowMode::WindowedFullscreen;
    static constexpr EKatanaGraphicQuality Quality = EKatanaGraphicQuality::High; // 텍스처, 그림자, 안티앨리어싱 등
    static constexpr bool bVSync = false;
    static constexpr float FrameRateLimit = 0.0f; // 0은 제한 없음
};

UCLASS()
class CH3_TEAM12_API UKatanaGraphicManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    static UKatanaGraphicManagerSubsystem* Get(const UObject* WorldContextObject);

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "Katana|Graphics")
    void ApplyAndSaveGraphicSettings();

    UFUNCTION(BlueprintCallable, Category = "Katana|Graphics")
    void LoadGraphicSettings();

    // 해상도
    UFUNCTION(BlueprintCallable, Category = "Katana|Graphics")
    void SetResolution(FIntPoint NewResolution);

    UFUNCTION(BlueprintPure, Category = "Katana|Graphics")
    FIntPoint GetResolution() const;

    // 창 모드
    UFUNCTION(BlueprintCallable, Category = "Katana|Graphics")
    void SetWindowMode(EKatanaWindowMode NewWindowMode);

    UFUNCTION(BlueprintPure, Category = "Katana|Graphics")
    EKatanaWindowMode GetWindowMode() const;

    // 퀄리티
    UFUNCTION(BlueprintCallable, Category = "Katana|Graphics")
    void SetGraphicQuality(EKatanaGraphicQuality NewQuality);

    UFUNCTION(BlueprintPure, Category = "Katana|Graphics")
    EKatanaGraphicQuality GetGraphicQuality() const;

    // VSync
    UFUNCTION(BlueprintCallable, Category = "Katana|Graphics")
    void SetVSyncEnabled(bool bEnable);

    UFUNCTION(BlueprintPure, Category = "Katana|Graphics")
    bool IsVSyncEnabled() const;

    // 프레임 제한
    UFUNCTION(BlueprintCallable, Category = "Katana|Graphics")
    void SetFrameRateLimit(float MaxFPS);

    UFUNCTION(BlueprintPure, Category = "Katana|Graphics")
    float GetFrameRateLimit() const;

private:
    UPROPERTY()
    TObjectPtr<UGameUserSettings> UserSettings;
};