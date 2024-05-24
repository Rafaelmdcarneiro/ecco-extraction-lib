#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#include "blue-dream.h"

#define WINDOW_W VIEWPORT_W * 2
#define WINDOW_H VIEWPORT_H * 2

int SaveD2DBitmap(BYTE *source, int width, int height, LPCWSTR filename);

typedef struct t_w_interface
{
	HWND hwnd;
	HINSTANCE hInstance;

	ID2D1Factory *d2d_factory;
	ID2D1HwndRenderTarget *d2d_render_target;
	ID2D1Bitmap *d2d_bitmap;

} t_w_interface;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
t_w_interface *create_w_interface(HINSTANCE hInstance);
void release_w_interface(t_w_interface *w_interface);

int wi_create_window(t_w_interface *w_interface, int nCmdShow);
int wi_create_direct2d(t_w_interface *w_interface);

void show_taskbar(int visible);