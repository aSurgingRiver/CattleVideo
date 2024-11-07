// Copyright Epic Games, Inc. All Rights Reserved.

#include "CattleVideoWidget.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SThrobber.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerInput.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "RenderingThread.h"
#include "CattleVideoCore.h"
#include "Textures/SlateUpdatableTexture.h"
#include "CattleVideoControl.h"
#include "Widgets/Images/SImage.h"
#include "Engine/Texture2D.h"
#include "Brushes/SlateDynamicImageBrush.h"
//#include "Launch/Resources/Version.h"
//#include "Resources/Version.h"
//#include "Version.h"
//#if PLATFORM_WINDOWS
//#include <windows/WindowsWindow.h>
//#endif
#define LOCTEXT_NAMESPACE "CattleVideoWidget"

namespace cattlevideo {
	class CGlobalDefault {
	public:
		CGlobalDefault();
		~CGlobalDefault();
		static CGlobalDefault* Get();

		UTexture2D* GetTexture();
	private:
		UTexture2D* Texture2D;
	};
	CGlobalDefault::CGlobalDefault() {
		Texture2D = UTexture2D::CreateTransient(1, 1, PF_B8G8R8A8);
		Texture2D->AddToRoot();
#if (UE_FF_VERSION) < 50000
		auto PlatformData = Texture2D->PlatformData;
#else
		auto PlatformData = Texture2D->GetPlatformData();
#endif
		void* TextrueData = PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
		uint32_t color=0xFF000000;
		FMemory::Memcpy(TextrueData, &color, 4);
		PlatformData->Mips[0].BulkData.Unlock();
		Texture2D->UpdateResource(); //
	}
	CGlobalDefault::~CGlobalDefault() {
		if (Texture2D)Texture2D->RemoveFromRoot();
	}

	CGlobalDefault* CGlobalDefault::Get() {
		static CGlobalDefault* instance=nullptr;
		if (instance == nullptr)instance = new CGlobalDefault();
		return instance;
	}
	UTexture2D* CGlobalDefault::GetTexture() {
		return Texture2D;
	}
}

class UCattleVideoWidgetImp {
public:
	UCattleVideoWidgetImp();
	~UCattleVideoWidgetImp();
	void UpdateBrush(TAttribute<const FSlateBrush*> Brush);
	void SetControl(TWeakPtr<UCattleVideoControl, ESPMode::ThreadSafe> C);
	void UpdateSize(int x,int y);
	void SetImage(TSharedPtr<SImage> i);
public:

private:
	TSharedPtr<SImage> image;
	TWeakPtr<UCattleVideoControl, ESPMode::ThreadSafe> Control;
	std::shared_ptr<cattlevideo::VideoSize> Size;
};

void UCattleVideoWidgetImp::SetImage(TSharedPtr<SImage> i) {
	image = i;
}

UCattleVideoWidgetImp::UCattleVideoWidgetImp() {
	Size = std::make_shared<cattlevideo::VideoSize>();
	image = nullptr;
	Control = nullptr;
}
UCattleVideoWidgetImp::~UCattleVideoWidgetImp() {
	if (Control.IsValid())Control.Pin()->DelSize(Size);
	Size = nullptr;
}
void UCattleVideoWidgetImp::SetControl(TWeakPtr<UCattleVideoControl, ESPMode::ThreadSafe> C) {
	if(Control.IsValid())Control.Pin()->DelSize(Size);
	Size->width = Size->height = 1;
	Control = C;
	Control.Pin()->AddSize(Size);
}

void UCattleVideoWidgetImp::UpdateSize(int x, int y) {
	if (Size->width != x)Size->width = x;
	if (Size->height != y)Size->height = y;
}

void UCattleVideoWidgetImp::UpdateBrush(TAttribute<const FSlateBrush*> Brush) {
	image->Invalidate(EInvalidateWidgetReason::PaintAndVolatility);
	image->SetImage(Brush);
}

SCattleVideoWidget::SCattleVideoWidget(){
	SetCanTick(true);
	_imp = MakeShared<UCattleVideoWidgetImp>();
}

SCattleVideoWidget::~SCattleVideoWidget() {
	_imp = nullptr;
}

void SCattleVideoWidget::Construct(const FArguments& InArgs){
	TAttribute<const FSlateBrush*> Brush = new FSlateDynamicImageBrush(cattlevideo::CGlobalDefault::Get()->GetTexture(), FVector2D(1, 1),TEXT("CattleVideo_Brush_Default"));
	TSharedPtr<SImage> image;
	ChildSlot
	[
		SAssignNew(image, SImage)
		.Visibility(EVisibility::Visible)
		.Image(Brush)
	];
	if (_imp)_imp->SetImage(image);
}

void SCattleVideoWidget::UpdateTexture(UTexture2D* Texture2D) {
	TAttribute<const FSlateBrush*> Brush = nullptr;
	if (nullptr == Texture2D) {
		Texture2D = cattlevideo::CGlobalDefault::Get()->GetTexture();
	}
	if (Texture2D) {
		auto X = Texture2D->GetSizeX();
		auto Y = Texture2D->GetSizeY();
		FName n = *FString::Printf(TEXT("Cattle_%p_%d_%d"), this, X, Y);
		Brush = new FSlateDynamicImageBrush(Texture2D, FVector2D(X, Y), n);
	}
	if (_imp)_imp->UpdateBrush(Brush);
}

void SCattleVideoWidget::Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float DeltaTime) {
	auto S = AllottedGeometry.GetLocalSize() * AllottedGeometry.Scale;
	if (_imp)_imp->UpdateSize(S.X, S.Y);
}

void SCattleVideoWidget::SetControl(TWeakPtr<UCattleVideoControl, ESPMode::ThreadSafe> C) {
	if (_imp)_imp->SetControl(C);
}

#undef LOCTEXT_NAMESPACE
