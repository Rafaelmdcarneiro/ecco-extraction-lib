#include "w_interface.h"

LRESULT CALLBACK WindowProc(
	HWND hwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	if (uMsg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 1;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

t_w_interface *create_w_interface(HINSTANCE hInstance)
{
	t_w_interface *w_interface = (t_w_interface *)malloc(sizeof(t_w_interface));

	if (w_interface == NULL) return NULL;

	w_interface->hInstance = hInstance;
	w_interface->d2d_factory = NULL;
	w_interface->d2d_render_target = NULL;
	w_interface->d2d_bitmap = NULL;

	return w_interface;
}

void release_w_interface(t_w_interface *w_interface)
{
	if (w_interface == NULL) return;

	if (NOT_NULL(w_interface->d2d_factory))
	{
		if (NOT_NULL(w_interface->d2d_bitmap))
		{
			w_interface->d2d_bitmap->Release();
			w_interface->d2d_bitmap = NULL;
		}

		if (NOT_NULL(w_interface->d2d_render_target))
		{
			w_interface->d2d_render_target->Release();
			w_interface->d2d_render_target = NULL;
		}

		w_interface->d2d_factory->Release();
		w_interface->d2d_factory = NULL;
	}

	free(w_interface);
	CoUninitialize();
}

int wi_create_window(t_w_interface *w_interface, int nCmdShow)
{
	WNDCLASS wc = { 0 };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance;
	wc.lpszClassName = PROGRAM_VERSION;

	RegisterClass(&wc);

	w_interface->hwnd = CreateWindowEx
	(
		0,
		PROGRAM_VERSION,
		PROGRAM_NAME,
		(WS_POPUPWINDOW | WS_CAPTION | WS_SIZEBOX),
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WINDOW_W,
		WINDOW_H,
		NULL,
		NULL,
		w_interface->hInstance,
		NULL
	);

	if (w_interface->hwnd == NULL)
	{
		release_w_interface(w_interface);
		return -1;
	}

	ShowWindow(w_interface->hwnd, nCmdShow);
	UpdateWindow(w_interface->hwnd);

	// show_taskbar(0);
	return 0;
}

int wi_create_direct2d(t_w_interface *w_interface)
{
	HRESULT hr = S_OK;

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &w_interface->d2d_factory);
	if (SUCCEEDED(hr) == FALSE)
	{
		release_w_interface(w_interface);
		return -1;
	}

	RECT wr = { 0 };
	GetClientRect(w_interface->hwnd, &wr);

	D2D1_SIZE_U ws = D2D1::SizeU
	(
		wr.right - wr.left,
		wr.bottom - wr.top
	);

	hr = w_interface->d2d_factory->CreateHwndRenderTarget
	(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(w_interface->hwnd, ws),
		&w_interface->d2d_render_target
	);

	if (SUCCEEDED(hr) == FALSE)
	{
		release_w_interface(w_interface);
		return -2;
	}

	D2D1_BITMAP_PROPERTIES bitmap_properties;

	bitmap_properties.pixelFormat = D2D1::PixelFormat
	(
		DXGI_FORMAT_B8G8R8A8_UNORM,
		D2D1_ALPHA_MODE_IGNORE
	);

	bitmap_properties.dpiX = 96.0f;
	bitmap_properties.dpiY = 96.0f;

	hr = w_interface->d2d_render_target->CreateBitmap
	(
		D2D1::SizeU(VIEWPORT_W, VIEWPORT_H),
		bitmap_properties,
		&w_interface->d2d_bitmap
	);

	if (SUCCEEDED(hr) == FALSE)
	{
		release_w_interface(w_interface);
		return -3;
	}

	return 0;
}

void show_taskbar(int visible)
{
	HWND taskbar = FindWindow(L"Shell_TrayWnd", NULL);
	HWND start_button = FindWindowEx(taskbar, 0, L"Button", NULL);

	int nCmdShow = SW_SHOW;
	if (visible == 0)
		nCmdShow = SW_HIDE;

	ShowWindow(taskbar, nCmdShow);
	UpdateWindow(taskbar);

	ShowWindow(start_button, nCmdShow);
	UpdateWindow(start_button);
}