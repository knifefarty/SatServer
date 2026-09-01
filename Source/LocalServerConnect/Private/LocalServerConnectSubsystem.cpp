#include "LocalServerConnectSubsystem.h"
#include "LocalServerConnectButton.h"
#include "LocalServerConnectConfig.h"
#include "Patching/WidgetBlueprintHookManager.h"
#include "Patching/NativeHookManager.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/PanelWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

bool ALocalServerConnectSubsystem::bHookRegistered = false;

ALocalServerConnectSubsystem::ALocalServerConnectSubsystem()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ALocalServerConnectSubsystem::BeginPlay()
{
    Super::BeginPlay();
    RegisterMainMenuHook();
}

void ALocalServerConnectSubsystem::RegisterMainMenuHook()
{
    if (bHookRegistered)
    {
        return;
    }

    // Runtime hook safety net
    auto HookHandler = [](UWidget* Widget)
    {
        if (Widget && Widget->GetClass())
        {
            FString ClassName = Widget->GetClass()->GetName();
            if (ClassName.Contains(TEXT("BP_MainMenuWidget")) || ClassName.Contains(TEXT("Widget_MainMenu")))
            {
                UUserWidget* UserWidget = Cast<UUserWidget>(Widget);
                if (UserWidget)
                {
                    InjectButtonIntoMainMenu(UserWidget);
                }
            }
        }
    };

    SUBSCRIBE_UOBJECT_METHOD_AFTER(UWidget, SynchronizeProperties, [HookHandler](UWidget* Widget)
    {
        HookHandler(Widget);
    });

    SUBSCRIBE_UOBJECT_METHOD_AFTER(UWidget, SetVisibility, [HookHandler](UWidget* Widget, ESlateVisibility InVisibility)
    {
        HookHandler(Widget);
    });

    bHookRegistered = true;
    UE_LOG(LogTemp, Warning, TEXT("[LocalServerConnect] Main Menu hooks registered."));
}

void ALocalServerConnectSubsystem::InjectButtonIntoMainMenu(UUserWidget* MainMenuWidget)
{
    if (!MainMenuWidget || !MainMenuWidget->WidgetTree)
    {
        return;
    }

    UPanelWidget* TargetPanel = nullptr;

    if (UWidget* ListWidget = MainMenuWidget->WidgetTree->FindWidget(FName(TEXT("mMainMenuList"))))
    {
        TargetPanel = Cast<UPanelWidget>(ListWidget);
    }

    if (!TargetPanel)
    {
        TArray<UWidget*> AllWidgets;
        MainMenuWidget->WidgetTree->GetAllWidgets(AllWidgets);
        for (UWidget* W : AllWidgets)
        {
            if (UVerticalBox* VB = Cast<UVerticalBox>(W))
            {
                if (VB->GetName().Contains(TEXT("Button")) || VB->GetName().Contains(TEXT("Menu")) || VB->GetChildrenCount() >= 3)
                {
                    TargetPanel = VB;
                    break;
                }
            }
        }
    }

    if (!TargetPanel)
    {
        return;
    }

    // Prevent duplicate injection
    for (UWidget* Child : TargetPanel->GetAllChildren())
    {
        if (Child && (Child->IsA(ULocalServerConnectButton::StaticClass()) || Child->GetName().Contains(TEXT("LocalConnect"))))
        {
            return;
        }
    }

    APlayerController* PC = MainMenuWidget->GetOwningPlayer();
    if (!PC)
    {
        PC = UGameplayStatics::GetPlayerController(MainMenuWidget->GetWorld(), 0);
    }

    ULocalServerConnectButton* NewButton = CreateWidget<ULocalServerConnectButton>(PC ? PC : MainMenuWidget->GetOwningPlayer(), ULocalServerConnectButton::StaticClass());

    if (NewButton)
    {
        const int32 SlotIndex = FMath::Clamp(2, 0, TargetPanel->GetChildrenCount());
        UPanelSlot* Slot = TargetPanel->InsertChildAt(SlotIndex, NewButton);
        if (UVerticalBoxSlot* VBSlot = Cast<UVerticalBoxSlot>(Slot))
        {
            VBSlot->SetPadding(FMargin(0.0f, 4.0f, 0.0f, 4.0f));
            VBSlot->SetHorizontalAlignment(HAlign_Fill);
            VBSlot->SetVerticalAlignment(VAlign_Center);
        }

        UE_LOG(LogTemp, Warning, TEXT("[LocalServerConnect] >>> INJECTED AUTHENTIC FRONTEND BUTTON INTO %s AT SLOT %d! <<<"), *MainMenuWidget->GetClass()->GetName(), SlotIndex);
    }
}
