
#include "CattleVideoViewport.h"
#include "CattleVideoControl.h"

FCattleVideoViewport::FCattleVideoViewport()
	: Control(nullptr)
	, Size(1,1)
{
}

FCattleVideoViewport::~FCattleVideoViewport() {
	Control = nullptr;
}

void FCattleVideoViewport::SetControl(TWeakPtr<UCattleVideoControl, ESPMode::ThreadSafe> C) {
	Control = C;
}
FIntPoint FCattleVideoViewport::GetSize() const {
	return Size;
}

FSlateShaderResource* FCattleVideoViewport::GetViewportRenderTargetTexture() const {
	return Control.IsValid() ? Control.Pin()->GetTargetTexture() : nullptr;
}

bool FCattleVideoViewport::RequiresVsync() const {
	return false;
}

void FCattleVideoViewport::Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float DeltaTime) {
	auto S = AllottedGeometry.GetLocalSize()* AllottedGeometry.Scale;
	Size = FIntPoint(S.X,S.Y);
}