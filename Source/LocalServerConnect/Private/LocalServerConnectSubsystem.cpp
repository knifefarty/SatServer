#include "LocalServerConnectSubsystem.h"
#include "LocalServerConnectButton.h"
#include "LocalServerConnectConfig.h"
#include "Patching/NativeHookManager.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
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

    SUBSCRIBE_UOBJECT_METHOD_AFTER(UUserWidget, SetDesiredFocusWidget, [HookHandler](UUserWidget* UserWidget, UWidget* Widget)
    {
        HookHandler(UserWidget);
    });

    bHookRegistered = true;
    UE_LOG(LogTemp, Warning, TEXT("[LocalServerConnect] Multi-hook registered for Main Menu injection."));
}

void ALocalServerConnectSubsystem::InjectButtonIntoMainMenu(UUserWidget* MainMenuWidget)
{
    if (!MainMenuWidget || !MainMenuWidget->WidgetTree)
    {
        return;
    }

    // Locate the vertical box containing the buttons
    UVerticalBox* ButtonBox = nullptr;
    TArray<UWidget*> AllWidgets;
    MainMenuWidget->WidgetTree->GetAllWidgets(AllWidgets);

    for (UWidget* W : AllWidgets)
    {
        if (UVerticalBox* VB = Cast<UVerticalBox>(W))
        {
            if (VB->GetName().Contains(TEXT("Button")) || VB->GetName().Contains(TEXT("Menu")) || VB->GetChildrenCount() >= 3)
            {
                ButtonBox = VB;
                break;
            }
        }
    }

    if (!ButtonBox)
    {
        return;
    }

    // Check for existing button to prevent duplicates
    for (UWidget* Child : ButtonBox->GetAllChildren())
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
        const int32 SlotIndex = FMath::Clamp(2, 0, ButtonBox->GetChildrenCount());
        UVerticalBoxSlot* Slot = Cast<UVerticalBoxSlot>(ButtonBox->InsertChildAt(SlotIndex, NewButton));
        if (Slot)
        {
            Slot->SetPadding(FMargin(0.0f, 6.0f, 0.0f, 6.0f));
            Slot->SetHorizontalAlignment(HAlign_Fill);
            Slot->SetVerticalAlignment(VAlign_Center);
        }

        UE_LOG(LogTemp, Warning, TEXT("[LocalServerConnect] >>> INJECTED LOCAL CONNECT BUTTON INTO %s AT SLOT %d! <<<"), *MainMenuWidget->GetClass()->GetName(), SlotIndex);
    }
}
