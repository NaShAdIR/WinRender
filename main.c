#include <stdbool.h>
#include <windows.h>
#include <stdint.h>


#define in	   static
#define global static
#define local  static


typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;


global bool Running;

global BITMAPINFO BitmapInfo;
global void* BitmapMemory;

global int BitmapWidth;
global int BitmapHeight;


in void
RenderGradient(int XOfSet, int YOfSet)
{
	int Pitch = BitmapWidth * 4;
	uint8* Row = (uint8*)BitmapMemory;

	for (int Y = 0; Y < BitmapHeight; Y++)
	{
		uint32* Pixel = (uint32*)Row;
		for (int X = 0; X < BitmapWidth; X++)
		{
			uint8 Green = (X + XOfSet);
			uint8 Blue  = (Y + YOfSet);

			*Pixel++ = (Green << 8) | Blue;
		}
		Row += Pitch;
	}
}


in void 
Win32ResizeDIBSection(int width, int height)
{
	BitmapWidth = width;
	BitmapHeight = height;

	if (BitmapMemory)
	{
		VirtualFree(
			BitmapMemory, 
			0, 
			MEM_RELEASE
		);
	}
	BITMAPINFOHEADER _bmiHeader = {
		.biSize			= sizeof(BitmapInfo.bmiHeader),
		.biWidth		= BitmapWidth,
		.biHeight		= -BitmapHeight,
		.biPlanes		= 1,
		.biBitCount		= 32,
		.biCompression	= BI_RGB
	};
	BitmapInfo.bmiHeader = _bmiHeader;

	BitmapMemory = VirtualAlloc(
		0, 
		((BitmapWidth * BitmapHeight) * 4),
		MEM_COMMIT, 
		PAGE_READWRITE
	);
}


in void 
Win32UpdateWindow(HDC DeviceContext, RECT* WindowRect)
{
	int WindowWidth = WindowRect->right - WindowRect->left;
	int WindowHeight = WindowRect->bottom - WindowRect->top;
	
	StretchDIBits(
		DeviceContext,
		0,
		0,
		BitmapWidth,
		BitmapHeight,
		0,
		0,
		WindowWidth,
		WindowHeight,
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

		RECT ClientRect;
		GetClientRect(Window, &ClientRect);

		Win32UpdateWindow(
			DeviceContext, 
			&ClientRect
		);
		EndPaint(Window, &Paint);

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
		HWND Window = CreateWindowEx(
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

		if (Window)
		{
			Running = true;
			MSG Message;

			uint8 XOfSet = 0;
			uint8 YOfSet = 0;

			while (Running)
			{
				while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
				{
					if (Message.message == WM_QUIT)
					{
						Running = false;
					}
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
				
				RECT ClientRect;
				GetClientRect(Window, &ClientRect);

				HDC dc = GetDC(Window);
				Win32UpdateWindow(
					dc,
					&ClientRect
				);
				RenderGradient(XOfSet++, YOfSet);
				ReleaseDC(Window, dc);
			}
		}
	}

	return 0;
}
