#include "blue-dream.h"
#include "w_interface.h"

int SaveD2DBitmap(BYTE *source, int width, int height, LPCWSTR filename)
{
	IWICImagingFactory *piFactory = NULL;
	IWICBitmapEncoder *piEncoder = NULL;
	IWICBitmapFrameEncode *piBitmapFrame = NULL;
	IPropertyBag2 *pPropertybag = NULL;

	IWICStream *piStream = NULL;
	UINT uiWidth = width;
	UINT uiHeight = height;

	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		(LPVOID *)&piFactory);

	if (SUCCEEDED(hr))
	{
		hr = piFactory->CreateStream(&piStream);
	}

	if (SUCCEEDED(hr))
	{
		hr = piStream->InitializeFromFilename(filename, GENERIC_WRITE);
	}

	if (SUCCEEDED(hr))
	{
		hr = piFactory->CreateEncoder(GUID_ContainerFormatPng, NULL, &piEncoder);
	}

	if (SUCCEEDED(hr))
	{
		hr = piEncoder->Initialize(piStream, WICBitmapEncoderNoCache);
	}

	if (SUCCEEDED(hr))
	{
		hr = piEncoder->CreateNewFrame(&piBitmapFrame, NULL);
	}


	hr = piBitmapFrame->Initialize(NULL);
	if (SUCCEEDED(hr))
	{
		hr = piBitmapFrame->SetSize(uiWidth, uiHeight);
	}

	WICPixelFormatGUID formatGUID = GUID_WICPixelFormat32bppBGRA;
	if (SUCCEEDED(hr))
	{
		hr = piBitmapFrame->SetPixelFormat(&formatGUID);
	}

	if (SUCCEEDED(hr))
	{
		// We're expecting to write out 24bppRGB. Fail if the encoder cannot do it.
		hr = IsEqualGUID(formatGUID, GUID_WICPixelFormat32bppBGRA) ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		UINT cbStride = (uiWidth * 32) / 8/***WICGetStride***/;
		UINT cbBufferSize = uiHeight * cbStride;

		BYTE *pbBuffer = source;

		if (pbBuffer != NULL)
		{
			hr = piBitmapFrame->WritePixels(uiHeight, cbStride, cbBufferSize, pbBuffer);
		}
		else
		{
			hr = E_OUTOFMEMORY;

		}
	}

	if (SUCCEEDED(hr))
	{
		hr = piBitmapFrame->Commit();
	}

	if (SUCCEEDED(hr))
	{
		hr = piEncoder->Commit();
	}

	if (piFactory)
		piFactory->Release();

	if (piEncoder)
		piEncoder->Release();

	if (piBitmapFrame)
		piBitmapFrame->Release();

	if (pPropertybag)
		pPropertybag->Release();

	if (piStream)
		piStream->Release();

	return hr;
}

void Render(t_w_interface *w_interface, t_dword *viewport)
{
	if (w_interface == NULL) return;
	if (w_interface->d2d_render_target == NULL) return;
	if (w_interface->d2d_bitmap == NULL) return;

	RECT wr = { 0 };
	GetClientRect(w_interface->hwnd, &wr);

	D2D1_SIZE_U ws = D2D1::SizeU
	(
		wr.right - wr.left,
		wr.bottom - wr.top
	);

	HRESULT hr = w_interface->d2d_factory->CreateHwndRenderTarget
	(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(w_interface->hwnd, ws),
		&w_interface->d2d_render_target
	);


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

	D2D1_RECT_U region = { 0, 0, VIEWPORT_W, VIEWPORT_H };
	w_interface->d2d_bitmap->CopyFromMemory(&region, viewport, VIEWPORT_W * 4);
	w_interface->d2d_render_target->BeginDraw();
	w_interface->d2d_render_target->SetTransform(D2D1::Matrix3x2F::Identity());
	w_interface->d2d_render_target->Clear(D2D1::ColorF(D2D1::ColorF::Black));

	D2D1_SIZE_F rtSize = w_interface->d2d_render_target->GetSize();
	
	w_interface->d2d_render_target->DrawBitmap
	(
		w_interface->d2d_bitmap,
		D2D1::RectF(0.0f, 0.0f, WINDOW_W, WINDOW_H),
		1.0,
		D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
	);

	w_interface->d2d_render_target->EndDraw();
}

int WINAPI WinMain
(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR pCmdLine,
	int nCmdShow
)
{
	t_w_interface *w_interface = NULL;

	HRESULT hr = S_OK;

	HeapSetInformation(
		NULL,
		HeapEnableTerminationOnCorruption,
		NULL,
		0);

	if (SUCCEEDED(CoInitialize(NULL)) == FALSE)
	{
		// LNG_COM_INITIALIZE_F
		return 0;
	}

	w_interface = create_w_interface(hInstance);
	if (w_interface == NULL) return 0;

	if (wi_create_window(w_interface, nCmdShow) < 0) return 0;
	if (wi_create_direct2d(w_interface) < 0) return 0;

	// the display bitmap that's copied to the window
	t_bitmap *viewport = create_bitmap(VIEWPORT_W, VIEWPORT_H);

	// returns a bitmap with the level map

	// t_bitmap *source = plz_extract_stage_map("BIGSHIP.PLZ", 0, 0, 0, 0, PLZ_FOREGROUND);
	t_bitmap *source = ecco2_extract_stage_map(ECCO2_US_ROM, E2_TRELLIASBAY, 0,0,800,800, EC_FOREGROUND);

	// copies level map data to the display bitmap

	if (source == NULL) return 0;

	draw_bitmap_direct(source, viewport, 0,0,0);

	// just keeping the following there as reference
	// D2D1_RECT_U region = { 0, 0, VIEWPORT_W, VIEWPORT_H};
	// w_interface->d2d_bitmap->CopyFromMemory(&region, core->viewport, VIEWPORT_W * 4);

	// outputs the level map
	if (NOT_NULL(source))
		SaveD2DBitmap((BYTE *)source->container,
			source->w,
			source->h,
			L"output.png");

	// copy the display bitmap to the window
	Render(w_interface, viewport->container);

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		InvalidateRect(w_interface->hwnd, NULL, TRUE);
	}

	release_w_interface(w_interface);

	show_taskbar(1);
	return 0;
}