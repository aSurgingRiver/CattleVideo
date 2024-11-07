// Copyright aSurgingRiver, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "Input/Reply.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Framework/SlateDelegates.h"
#include "Styling/SlateTypes.h"


class UCattleVideoControl;
class UCattleVideoWidgetImp;

class CATTLEVIDEOCORE_API SCattleVideoWidget
	: public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCattleVideoWidget)
	{}

	SLATE_END_ARGS()

public:
	/** Default constructor. */
	SCattleVideoWidget();
	~SCattleVideoWidget();
	/**
	 * Construct the widget.
	 * @param InArgs  Declaration from which to construct the widget.
	 */
	void Construct(const FArguments& InArgs);

	void UpdateTexture(UTexture2D* Texture2D);

	void SetControl(TWeakPtr<UCattleVideoControl, ESPMode::ThreadSafe> C);

	virtual void Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float DeltaTime) override;
private:
	TSharedPtr<UCattleVideoWidgetImp> _imp;
};
