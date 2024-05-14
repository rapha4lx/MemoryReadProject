#pragma once

namespace UI {
	// Data
	inline LPDIRECT3D9              g_pD3D = nullptr;
	inline LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
	inline UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
	inline D3DPRESENT_PARAMETERS    g_d3dpp = {};

	// Forward declarations of helper functions
	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void ResetDevice();
	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}