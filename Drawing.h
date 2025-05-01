#pragma once
#include <d3d11.h>
extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pd3dDeviceContext;
extern IDXGISwapChain* g_pSwapChain;
extern bool                         g_SwapChainOccluded;
extern UINT                         g_ResizeWidth, g_ResizeHeight;
extern ID3D11RenderTargetView* g_mainRenderTargetView;
extern HWND                         g_windowHandle;
extern WNDCLASSEXW                  g_windowClass;
class Drawing {
	private:

	public:
		void InitDrawing();
		bool CreateDeviceD3D(HWND hWnd);
		void CleanupDeviceD3D();
		void CreateRenderTarget();
		void CleanupRenderTarget();
		void InitImGui();
		void HandleMessage(bool &);
		void DrawTopRightCorner();
		void RenderHook();

		void OnSettingWindow();
		void OnStartFrame(), OnEndFrame();
		void OnCleanUp();
};