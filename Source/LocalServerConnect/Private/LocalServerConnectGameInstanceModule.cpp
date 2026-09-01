#include "LocalServerConnectGameInstanceModule.h"
#include "LocalServerConnectButton.h"
#include "Patching/WidgetBlueprintHookManager.h"

ULocalServerConnectGameInstanceModule::ULocalServerConnectGameInstanceModule()
{
    bRootModule = true;

    // Configure Widget Blueprint Hook for Main Menu button injection
    UWidgetBlueprintHookData* HookData = CreateDefaultSubobject<UWidgetBlueprintHookData>(TEXT("MainMenuHookData"));
    if (HookData)
    {
        HookData->WidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(TEXT("/Game/FactoryGame/Interface/UI/Menu/MainMenu/BP_MainMenuWidget.BP_MainMenuWidget_C")));
        HookData->NewWidgetClass = ULocalServerConnectButton::StaticClass();
        HookData->NewWidgetName = TEXT("LocalServerConnectButton");
        HookData->ParentWidgetName = TEXT("mMainMenuList");
        HookData->ParentWidgetType = EWidgetBlueprintHookParentType::Direct;
        HookData->ParentSlotIndex = 2;

        UWidgetBlueprintHookSlot_Generic* SlotConfig = CreateDefaultSubobject<UWidgetBlueprintHookSlot_Generic>(TEXT("MainMenuSlotConfig"));
        if (SlotConfig)
        {
            SlotConfig->Padding = FMargin(0.0f, 4.0f, 0.0f, 4.0f);
            SlotConfig->HorizontalAlignment = HAlign_Fill;
            SlotConfig->VerticalAlignment = VAlign_Center;
            HookData->SlotConfiguration = SlotConfig;
        }

        WidgetBlueprintHooks.Add(HookData);
    }
}

void ULocalServerConnectGameInstanceModule::DispatchLifecycleEvent(ELifecyclePhase Phase)
{
    Super::DispatchLifecycleEvent(Phase);

    if (Phase == ELifecyclePhase::CONSTRUCTION)
    {
        UE_LOG(LogTemp, Warning, TEXT("[LocalServerConnect] ULocalServerConnectGameInstanceModule initialized as SML Root Module!"));
    }
}
