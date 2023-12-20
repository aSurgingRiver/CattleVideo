
#pragma once

#include "CoreMinimal.h"
#include "Rendering/RenderingCommon.h"
#include <memory>

class UCattleVideoControl;
namespace cattlevideo {
	struct VideoSize;
}

/**
 * A Slate viewport to display a Web Browser Window
 */
class CATTLEVIDEOCORE_API FCattleVideoViewport : public ISlateViewport
{
public:
	/**
	 * Default Constructor
	 *
	 * @param InWebBrowserWindow The Web Browser Window this viewport will display
	 * @param InViewportWidget The Widget displaying this viewport (needed to capture mouse)
	 * @param InIsPopup Used to initialize a viewport for showing browser popup menus instead of the main window.
	 */
	FCattleVideoViewport();

	/**
	 * Destructor.
	 */
	~FCattleVideoViewport();
	void SetControl(TWeakPtr<UCattleVideoControl, ESPMode::ThreadSafe> C);
	// ISlateViewport interface
	virtual FIntPoint GetSize() const override;
	virtual FSlateShaderResource* GetViewportRenderTargetTexture() const override;
	virtual bool RequiresVsync() const override;
	virtual void Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float DeltaTime) override;

private:
	//std::shared_ptr< UCattleVideoControl> Control;
	TWeakPtr<UCattleVideoControl, ESPMode::ThreadSafe> Control;
	std::shared_ptr<cattlevideo::VideoSize> Size;
	//FIntPoint Size;
};

