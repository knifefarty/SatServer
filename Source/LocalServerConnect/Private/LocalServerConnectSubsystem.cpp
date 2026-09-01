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

    // Hook SetVisibility - ONLY target the top-level Main Menu widget (BP_MainMenuWidget_C)
    SUBSCRIBE_UOBJECT_METHOD_AFTER(UWidget, SetVisibility, [](UWidget* Widget, ESlateVisibility InVisibility)
    {
        if (Widget && Widget->GetClass())
        {
            FString ClassName = Widget->GetClass()->GetName();
            // Specifically target the Main Menu screen widget, ignoring individual buttons
            if (ClassName.Equals(TEXT("BP_MainMenuWidget_C")) || ClassName.Equals(TEXT("Widget_MainMenu_C")))
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
    UE_LOG(LogTemp, Warning, TEXT("[LocalServerConnect] Main Menu hook registered for BP_MainMenuWidget_C."));
}

void ALocalServerConnectSubsystem::InjectButtonIntoMainMenu(UUserWidget* MainMenuWidget)
{
    if (!MainMenuWidget || !MainMenuWidget->WidgetTree)
    {
        return;
    }

    // Find the primary button container VerticalBox
    UVerticalBox* ButtonBox = nullptr;
    TArray<UWidget*> AllWidgets;
    MainMenuWidget->WidgetTree->GetAllWidgets(AllWidgets);

    for (UWidget* W : AllWidgets)
    {
        if (UVerticalBox* VB = Cast<UVerticalBox>(W))
        {
            // Look for the VerticalBox that holds the main menu buttons
            if (VB->GetName().Contains(TEXT("Button")) || VB->GetName().Contains(TEXT("Menu")) || VB->GetChildrenCount() >= 4)
            {
                ButtonBox = VB;
                break;
            }
        }
    }

    if (!ButtonBox)
    {
        UE_LOG(LogTemp, Error, TEXT("[LocalServerConnect] Could not locate main menu VerticalBox on %s"), *MainMenuWidget->GetClass()->GetName());
        return;
    }

    // Prevent duplicate injection
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

    // Try to instantiate vanilla Widget_FrontEnd_Button_C for authentic Satisfactory styling
    UClass* VanillaButtonClass = LoadClass<UUserWidget>(nullptr, TEXT("/Game/FactoryGame/Interface/UI/Menu/MainMenu/Widget_FrontEnd_Button.Widget_FrontEnd_Button_C"));
    if (!VanillaButtonClass)
    {
        VanillaButtonClass = LoadClass<UUserWidget>(nullptr, TEXT("/Game/FactoryGame/Interface/UI/Menu/MainMenu/BP_FrontEnd_Button.BP_FrontEnd_Button_C"));
    }

    UUserWidget* InjectedWidget = nullptr;

    if (VanillaButtonClass)
    {
        InjectedWidget = CreateWidget<UUserWidget>(PC ? PC : MainMenuWidget->GetOwningPlayer(), VanillaButtonClass);
        if (InjectedWidget && InjectedWidget->WidgetTree)
        {
            // Set button label text to "Connect to Local Host"
            TArray<UWidget*> SubWidgets;
            InjectedWidget->WidgetTree->GetAllWidgets(SubWidgets);
            for (UWidget* SubW : SubWidgets)
            {
                if (UTextBlock* TB = Cast<UTextBlock>(SubW))
                {
                    TB->SetText(FText::FromString(TEXT("Connect to Local Host")));
                }
                if (UButton* Btn = Cast<UButton>(SubW))
                {
                    Btn->OnClicked.AddDynamic(GetMutableDefault<ULocalServerConnectButton>(), &ULocalServerConnectButton::HandleButtonClicked);
                }
            }
        }
    }

    // Fallback to ULocalServerConnectButton if vanilla button class is not found
    if (!InjectedWidget)
    {
        InjectedWidget = CreateWidget<ULocalServerConnectButton>(PC ? PC : MainMenuWidget->GetOwningPlayer(), ULocalServerConnectButton::StaticClass());
    }

    if (InjectedWidget)
    {
        // Insert at Index 2 (below Continue and New Game / Load)
        const int32 SlotIndex = FMath::Clamp(2, 0, ButtonBox->GetChildrenCount());
        UVerticalBoxSlot* Slot = Cast<UVerticalBoxSlot>(ButtonBox->InsertChildAt(SlotIndex, InjectedWidget));
        if (Slot)
        {
            Slot->SetPadding(FMargin(0.0f, 4.0f, 0.0f, 4.0f));
            Slot->SetHorizontalAlignment(HAlign_Fill);
            Slot->SetVerticalAlignment(VAlign_Center);
        }

        UE_LOG(LogTemp, Warning, TEXT("[LocalServerConnect] >>> INJECTED BUTTON DIRECTLY INTO %s AT SLOT %d! <<<"), *MainMenuWidget->GetClass()->GetName(), SlotIndex);
    }
}
