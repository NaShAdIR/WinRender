#include <windows.h>
#include <stdbool.h>


#define in	   static
#define global static
#define local  static


global bool Running;

global BITMAPINFO BitmapInfo;
global void* BitmapMemory;
global HBITMAP BitmapHandle;
global HDC BitmapDeviceContext;


in void 
Win32ResizeDIBSection(int width, int height)
{
	if (BitmapHandle)
	{
		DeleteObject(BitmapHandle);
	}
	if (!BitmapDeviceContext)
	{
		BitmapDeviceContext = CreateCompatibleDC(0);
	}
	BITMAPINFOHEADER _bmiHeader = {
		.biSize			= sizeof(BitmapInfo.bmiHeader),
		.biWidth		= width,
		.biHeight		= height,
		.biPlanes		= 1,
		.biBitCount		= 32,
		.biCompression	= BI_RGB
	};
	BitmapInfo.bmiHeader = _bmiHeader;

	BitmapHandle = CreateDIBSection(
		BitmapDeviceContext,
		&BitmapInfo,
		DIB_RGB_COLORS,
		&BitmapMemory,
		0,
		0
	);
	ReleaseDC(0, BitmapDeviceContext);
}


in void 
Win32UpdateWindow(HDC DeviceContext, int x, int y, int width, int height)
{
	StretchDIBits(
		DeviceContext,
		x,
		y,
		width,
		height,
		x,
		y,
		width,
		height,
		BitmapMemory,
		&BitmapInfo,
		DIB_RGB_COLORS,
		SRCCOPY
	);
}


LRESULT MainWindowCallBack(
	HWND Window,
	UINT Message,
	WPARAM WParam,
	LPARAM LParam
)
{
	LRESULT result = 0;

	switch (Message)
	{
	case WM_SIZE:
	{
		RECT ClientRect;
		if (GetClientRect(Window, &ClientRect))
		{
			int height = ClientRect.bottom - ClientRect.top;
			int width = ClientRect.right - ClientRect.left;
			
			Win32ResizeDIBSection(width, height);
		}
	} break;

	case WM_DESTROY:
	{
		Running = false;
	} break;

	case WM_CLOSE:
	{
		Running = false;
	} break;

	case WM_ACTIVATEAPP:
	{
		OutputDebugString(L"WM_ACTIVATEAPP\n");
	} break;

	case WM_PAINT:
	{
		PAINTSTRUCT Paint;
		HDC DeviceContext = BeginPaint(Window, &Paint);

		int X = Paint.rcPaint.left;
		int Y = Paint.rcPaint.top;
		int height = Paint.rcPaint.bottom - Paint.rcPaint.top;
		int width = Paint.rcPaint.right - Paint.rcPaint.left;

		Win32UpdateWindow(
			DeviceContext, 
			X, 
			Y,
			width, 
			height
		);
	} break;

	default:
	{
		result = DefWindowProc(
			Window,
			Message,
			WParam,
			LParam
		);
	} break;
	}
	

	return result;
}


int WINAPI wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine,
	int nShowCmd
)
{
	WNDCLASS WindowClass = {};

	WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = MainWindowCallBack;
	WindowClass.hInstance = hInstance;
	WindowClass.lpszClassName = L"SeyranClass";

	if (RegisterClass(&WindowClass))
	{
		HWND WindowHandler = CreateWindowEx(
			0,
			WindowClass.lpszClassName,
			L"Seyran",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			0,
			0,
			hInstance,
			0
		);
		if (WindowHandler)
		{
			Running = true;
			MSG Message;

			while (Running)
			{
				BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
				if (MessageResult > 0)
				{
					TranslateMessage(&Message);
					DispatchMessage(&Message);

					/*
					MainWindowCallBack(
						Message.hwnd,
						Message.message,
						Message.wParam,
						Message.lParam
					);
					*/
				}
				else
				{
					break;
				}
			}
		}
	}

	return 0;
}
