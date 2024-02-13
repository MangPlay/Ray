// RAI Project 개발자 : Nam Chan woo

#pragma once

#include "CoreMinimal.h"
#include "RayWidgetController.h"
#include "InventorySystem/Libraries/InventoryEnumStruct.h"
#include "InventoryWidgetController.generated.h"

class UInventoryCore;
class UPlayerInventoryComponent;
class URayUserWidget;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGamepadToggled, bool, bGamepadControls);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchedWidget, EWidgetType, NewWidget);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchedTab, EWidgetType, NewTab);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFocusedNewWidget);


/**
 * 
 */
UCLASS()
class RAY_API UInventoryWidgetController : public URayWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;

	virtual void BindCallbacksToDependencies() override;

public:
	UFUNCTION(BlueprintImplementableEvent, Category="Inventory Widget Controller Event")
	void ShowLootBar();

	UFUNCTION(BlueprintImplementableEvent, Category="Inventory Widget Controller Event")
	void HideLootBar();

public:
	UFUNCTION(BlueprintCallable, Category="Inventory Widget Controller Event|Input")
	void InputCloseWidget();

public:
	UFUNCTION(BlueprintCallable, Category="Inventory Widget Controller Event|WidgetManager")
	void OpenNewWidget(EWidgetType Widget);

	UFUNCTION(BlueprintCallable, Category="Inventory Widget Controller Event|WidgetManager")
	virtual void CloseActiveWidget();

	UFUNCTION(BlueprintCallable, Category="Inventory Widget Controller Event|WidgetManager")
	void SwitchWidgetTo(EWidgetType NewWidget);

	UFUNCTION(BlueprintCallable, Category="Inventory Widget Controller Event|WidgetManager")
	void SwitchTabTo(EWidgetType NewTab);

	UFUNCTION(BlueprintCallable, Category="Inventory Widget Controller Event|WidgetManager")
	void SetActiveWidget(EWidgetType Widget);

	UFUNCTION(BlueprintCallable, Category="Inventory Widget Controller Event|WidgetManager")
	void SetActiveTab(EWidgetType Tab);

	UFUNCTION(BlueprintCallable, Category="Inventory Widget Controller Event|WidgetManager")
	void DisplayMessageNotify(const FString& InMessage);

	UFUNCTION(BlueprintCallable, Category="Inventory Widget Controller Event|WidgetManager")
	void CloseActivePopUp();

	UFUNCTION(BlueprintCallable, Category="Inventory Widget Controller Event|WidgetManager")
	void OpenConfirmationPopUp(const FString& QuestionText, const FItemData& ItemData, FItemData InSlotData, UInventoryCore* Sender, UInventoryCore* Receiver, EInputMethod InputMethod,
	                           EItemDestination Initiator, EItemDestination Destination, UUserWidget* SenderWidget);

	UFUNCTION(BlueprintCallable, Category="Inventory Widget Controller Event|WidgetManager")
	void OpenSplitStackPopUp(const FItemData& ItemData, FItemData InSlotData, UInventoryCore* Sender, UInventoryCore* Receiver, EInputMethod InputMethod,
	                         EItemDestination Initiator, EItemDestination Destination, UUserWidget* SenderWidget);

	UFUNCTION(BlueprintCallable, Category="Inventory Widget Controller Event|WidgetManager")
	void OpenTextDocumentPopUp(const FItemData& ItemData,UUserWidget* SenderWidget);

	UFUNCTION(BlueprintPure, Category="Inventory Widget Controller Event|WidgetManager")
	APlayerCapture* GetPlayerCapture();

	UFUNCTION(BlueprintPure, Category="Inventory Widget Controller Event|WidgetManager")
	EWidgetType GetActiveWidget();

	UFUNCTION(BlueprintPure, Category="Inventory Widget Controller Event|WidgetManager")
	EWidgetPopUp GetActivePopUp();

	UFUNCTION(BlueprintPure, Category="Inventory Widget Controller Event|WidgetManager")
	EWidgetType GetActiveTab();

	UFUNCTION(BlueprintPure, Category="Inventory Widget Controller Event|WidgetManager")
	bool IsAnyMainWidgetOpen() { return ActiveWidget != EWidgetType::None; }

public:
	UFUNCTION(BlueprintCallable, Category="Ray Player Controller Event|Player Capture")
	void InitializePlayerCapture();

	UFUNCTION(BlueprintCallable, Category="Ray Player Controller Event|Player Capture")
	void StartPlayerCapture();

	UFUNCTION(BlueprintCallable, Category="Ray Player Controller Event|Player Capture")
	void StopPlayerCapture();

public:
	UFUNCTION(BlueprintCallable, Category="Inventory Widget Controller Event|Widgets")
	void OpenInventoryWidget();

	UFUNCTION(BlueprintCallable, Category="Inventory Widget Controller Event|Widgets")
	void CloseInventoryWidget();

	UFUNCTION(BlueprintCallable, Category="Inventory Widget Controller Event|Widgets")
	void OpenLootBarWidget();

	UFUNCTION(BlueprintCallable, Category="Inventory Widget Controller Event|Widgets")
	void CloseLootBarWidget();

	UFUNCTION(BlueprintCallable, Category="Inventory Widget Controller Event|Widgets")
	void OpenVendorWidget();

	UFUNCTION(BlueprintCallable, Category="Inventory Widget Controller Event|Widgets")
	void CloseVendorWidget();

	UFUNCTION(BlueprintCallable, Category="Inventory Widget Controller Event|Widgets")
	void OpenStorageWidget();

	UFUNCTION(BlueprintCallable, Category="Inventory Widget Controller Event|Widgets")
	void CloseStorageWidget();

protected:
	void ActivateWidgetWithUIConfig(URayUserWidget* OpeningWidget);

	void DeactivateWidgetWithUIReset(URayUserWidget* ClosingWidget);

public:
	UFUNCTION(BlueprintCallable, Category="Inventory Widget Controller Event|Sound Effects")
	void PlaySoundOnOpeningWidget();

	UFUNCTION(BlueprintCallable, Category="Inventory Widget Controller Event|Sound Effects")
	void PlaySoundOnItemPickedUp();

	UFUNCTION(BlueprintCallable, Category="Inventory Widget Controller Event|Sound Effects")
	void PlaySoundOnTabSwitched();

	UFUNCTION(BlueprintCallable, Category="Inventory Widget Controller Event|Sound Effects")
	void PlaySoundOnItemDropped();

public:
	UPROPERTY(EditAnywhere, Category="Inventory Widget Controller|Setup")
	TSubclassOf<URayUserWidget> InventoryWidgetClass;

	UPROPERTY(EditAnywhere, Category="Inventory Widget Controller|Setup")
	TSubclassOf<URayUserWidget> VendorWidgetClass;

	UPROPERTY(EditAnywhere, Category="Inventory Widget Controller|Setup")
	TSubclassOf<URayUserWidget> StorageWidgetClass;

	UPROPERTY(EditAnywhere, Category="Inventory Widget Controller|Setup")
	USoundBase* InventoryOpenWidgetSound;

	UPROPERTY(EditAnywhere, Category="Inventory Widget Controller|Setup")
	USoundBase* PickupSound;

	UPROPERTY(EditAnywhere, Category="Inventory Widget Controller|Setup")
	USoundBase* TabSwitchedSound;

	UPROPERTY(EditAnywhere, Category="Inventory Widget Controller|Setup")
	USoundBase* ItemDroppedSound;


	UPROPERTY(BlueprintReadWrite, Category="Inventory Widget Controller|Widget")
	TObjectPtr<URayUserWidget> InventoryWidgetRef;

	UPROPERTY(BlueprintReadWrite, Category="Inventory Widget Controller|Widget")
	TObjectPtr<URayUserWidget> VendorWidgetRef;

	UPROPERTY(BlueprintReadWrite, Category="Inventory Widget Controller|Widget")
	TObjectPtr<URayUserWidget> StorageWidgetRef;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory Widget Controller|Widget")
	bool bShowLootBar = true;

	UPROPERTY(BlueprintReadWrite, Category="Inventory Widget Controller|Widget")
	bool bIsLootBarOpen = false;

	UPROPERTY(BlueprintReadWrite, Category="Inventory Widget Controller|Widget")
	bool bIsInteractableActorWidgetOpen = false;


	UPROPERTY(BlueprintReadWrite, Category="Inventory Widget Controller|UI Manager")
	EWidgetType ActiveWidget = EWidgetType::None;

	UPROPERTY(BlueprintReadWrite, Category="Inventory Widget Controller|UI Manager")
	EWidgetType ActiveTab = EWidgetType::None;

	UPROPERTY(BlueprintReadWrite, Category="Inventory Widget Controller|UI Manager")
	EWidgetType FocusedWidget = EWidgetType::None;

	UPROPERTY(BlueprintReadWrite, Category="Inventory Widget Controller|UI Manager")
	EWidgetPopUp ActivePopUp = EWidgetPopUp::None;


	UPROPERTY(BlueprintReadWrite, Category="Inventory Widget Controller|Reference")
	TObjectPtr<UPlayerInventoryComponent> PlayerInventory;

	UPROPERTY(BlueprintReadWrite, Category="Inventory Widget Controller|Reference")
	TObjectPtr<UPlayerEquipmentComponent> PlayerEquipment;

	UPROPERTY(BlueprintReadWrite, Category="Inventory Widget Controller|Reference")
	TObjectPtr<APlayerCapture> PlayerCapture;

	UPROPERTY(BlueprintReadWrite, Category="Inventory Widget Controller|Reference")
	TObjectPtr<UUserWidget> PopUpRef;

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Ray PlayerController|Delegate|UI Manager")
	FOnSwitchedWidget OnSwitchedWidget;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Ray PlayerController|Delegate|UI Manager")
	FOnSwitchedTab OnSwitchedTab;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Ray PlayerController|Delegate|UI Manager")
	FOnFocusedNewWidget OnFocusedNewWidget;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Ray PlayerController|Delegate|Gamepad")
	FOnGamepadToggled OnGamepadToggled;
};
