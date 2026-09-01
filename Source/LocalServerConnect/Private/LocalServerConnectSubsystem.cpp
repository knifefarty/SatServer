#include "LocalServerConnectSubsystem.h"
#include "LocalServerConnectButton.h"
#include "Patching/NativeHookManager.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
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

    if (UKismetSystemLibrary::IsDedicatedServer(GetWorld()))
    {
        return;
    }

    RegisterMainMenuHook();
}

void ALocalServerConnectSubsystem::RegisterMainMenuHook()
{
    if (bHookRegistered)
    {
        return;
    }

    SUBSCRIBE_UOBJECT_METHOD_AFTER(UWidget, SetVisibility, [](UWidget* Widget, ESlateVisibility InVisibility)
    {
        if (Widget && Widget->GetClass())
        {
            FString ClassName = Widget->GetClass()->GetName();
            if (ClassName.Contains(TEXT("Widget_MainMenu")) || ClassName.Contains(TEXT("BP_MainMenu")) || ClassName.Contains(TEXT("Widget_FrontEnd")))
            {
                UUserWidget* UserWidget = Cast<UUserWidget>(Widget);
                if (UserWidget)
                {
                    InjectButtonIntoMainMenu(UserWidget);
                }
            }
        }
    });

    SUBSCRIBE_UOBJECT_METHOD_AFTER(UWidget, SynchronizeProperties, [](UWidget* Widget)
    {
        if (Widget && Widget->GetClass())
        {
            FString ClassName = Widget->GetClass()->GetName();
            if (ClassName.Contains(TEXT("Widget_MainMenu")) || ClassName.Contains(TEXT("BP_MainMenu")) || ClassName.Contains(TEXT("Widget_FrontEnd")))
            {
                UUserWidget* UserWidget = Cast<UUserWidget>(Widget);
                if (UserWidget)
                {
                    InjectButtonIntoMainMenu(UserWidget);
                }
            }
        }
    });

    bHookRegistered = true;
    UE_LOG(LogTemp, Warning, TEXT("[LocalServerConnect] Successfully registered hooks for Main Menu injection."));
}

void ALocalServerConnectSubsystem::InjectButtonIntoMainMenu(UUserWidget* MainMenuWidget)
{
    if (!MainMenuWidget)
    {
        return;
    }

    // 1. Check known button box names
    UVerticalBox* ButtonBox = Cast<UVerticalBox>(MainMenuWidget->GetWidgetFromName(TEXT("mButtonBox")));
    if (!ButtonBox) ButtonBox = Cast<UVerticalBox>(MainMenuWidget->GetWidgetFromName(TEXT("mButtons")));
    if (!ButtonBox) ButtonBox = Cast<UVerticalBox>(MainMenuWidget->GetWidgetFromName(TEXT("MenuButtons")));
    if (!ButtonBox) ButtonBox = Cast<UVerticalBox>(MainMenuWidget->GetWidgetFromName(TEXT("VerticalBox_Buttons")));
    if (!ButtonBox) ButtonBox = Cast<UVerticalBox>(MainMenuWidget->GetWidgetFromName(TEXT("mNavButtons")));

    // 2. If not found by direct name, scan the widget tree for the primary VerticalBox container
    if (!ButtonBox && MainMenuWidget->WidgetTree)
    {
        TArray<UWidget*> AllWidgets;
        MainMenuWidget->WidgetTree->GetAllWidgets(AllWidgets);
        for (UWidget* Child : AllWidgets)
        {
            if (UVerticalBox* VB = Cast<UVerticalBox>(Child))
            {
                if (VB->GetChildrenCount() >= 2)
                {
                    ButtonBox = VB;
                    break;
                }
            }
        }
    }

    if (!ButtonBox)
    {
        return;
    }

    // Check if the button is already injected to prevent duplicate additions
    for (UWidget* Child : ButtonBox->GetAllChildren())
    {
        if (Child && Child->IsA(ULocalServerConnectButton::StaticClass()))
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
        // Insert at Index 2 (directly below Continue [0] and Load Game [1])
        const int32 TargetSlotIndex = FMath::Clamp(2, 0, ButtonBox->GetChildrenCount());
        UVerticalBoxSlot* BoxSlot = Cast<UVerticalBoxSlot>(ButtonBox->InsertChildAt(TargetSlotIndex, NewButton));
        
        if (BoxSlot)
        {
            BoxSlot->SetPadding(FMargin(0.0f, 6.0f, 0.0f, 6.0f));
            BoxSlot->SetHorizontalAlignment(HAlign_Fill);
            BoxSlot->SetVerticalAlignment(VAlign_Center);
        }

        UE_LOG(LogTemp, Warning, TEXT("[LocalServerConnect] SUCCESS: Injected Connect to Local Host button into %s at slot index %d."), *MainMenuWidget->GetClass()->GetName(), TargetSlotIndex);
    }
}
