#ifndef UNICODE
#define UNICODE
#endif

#include <stdbool.h>
#include <windows.h>
#include <stdint.h>


#define internal static
#define global   static
#define local    static


typedef uint8_t	 uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;


typedef struct
tagWindowSize
{
	int Width;
	int Height;

} WindowSize;


typedef struct
tagWin32OffScreenBuffer
{
	BITMAPINFO BitmapInfo;
	void* BitmapMemory;

	WindowSize windowSize;

} Win32OffScreenBuffer;


typedef struct
tabButton
{
	int x;
	int y;
	int Width;
	int Height;
	
	struct
	{
		uint8 Red;
		uint8 Green;
		uint8 Blue;
	} Color;
		
} Button;


global bool Running;
global Win32OffScreenBuffer GlobalBackBuffer;


internal WindowSize
Win32GetWindowSize(HWND Window)
{
	WindowSize windowSize = {};
	RECT ClientRect;

	if (GetClientRect(Window, &ClientRect))
	{
		windowSize.Width = ClientRect.right - ClientRect.left; 
		windowSize.Height = ClientRect.bottom - ClientRect.top;
	}
	
	return windowSize;
}


/*
internal void
RenderButton(Win32OffScreenBuffer* Buffer, Button button)
{
	uint32* BitmapMemory = (uint32*)Buffer->BitmapMemory;
	bool rend = false;

	for (int y = 0; y < Buffer->windowSize.Height; y++)
	{
		for (int x = 0; x < Buffer->windowSize.Width; x++)
		{
			if (button.x == x && button.y == y)
			{
				rend = true;
			}
			else
			{
				*BitmapMemory = 0x00FFFFFF;
			}
			if (rend)
			{
				if (x >= button.Width && 
					y >= button.Height && 
					x <= button.x + button.Width && 
					y <= button.y + button.Height)
				{
					*BitmapMemory = (
						(((button.Color.Red << 8) | button.Color.Green) << 8) | 
						button.Color.Blue
					);
				}
			}
			BitmapMemory++;
		}
	}
}
*/

internal void
RenderGradient(Win32OffScreenBuffer Buffer, int XOfSet, int YOfSet)
{
	/*
	Button button;
	button.Color.Red = 144;
	button.Color.Green = 61;
	button.Color.Blue = 61;

	button.x = 50;
	button.y = 50;
	button.Height = 100;
	button.Width = 100;

	RenderButton(&Buffer, button);
	*/
	
	int Pitch = Buffer.windowSize.Width * 4;
	uint8* Row = (uint8*)Buffer.BitmapMemory;

	for (int Y = 0; Y < Buffer.windowSize.Height; Y++)
	{
		uint32* Pixel = (uint32*)Row;
		for (int X = 0; X < Buffer.windowSize.Width; X++)
		{
			uint8 Green = (X + XOfSet);
			uint8 Blue  = (Y + YOfSet);

			*Pixel++ = (Green << 8) | Blue;
		}
		Row += Pitch;
	}
}


internal Win32OffScreenBuffer*
Win32ResizeDIBSection(Win32OffScreenBuffer* Buffer, WindowSize windowSize)
{
	if (Buffer->BitmapMemory)
	{
		VirtualFree(
			Buffer->BitmapMemory, 
			0, 
			MEM_RELEASE
		);
	}
	Buffer->windowSize = windowSize;

	BITMAPINFOHEADER _bmiHeader = {
		.biSize			= sizeof(Buffer->BitmapInfo.bmiHeader),
		.biWidth		= windowSize.Width,
		.biHeight		= -windowSize.Height,
		.biPlanes		= 1,
		.biBitCount		= 32,
		.biCompression	= BI_RGB
	};
	Buffer->BitmapInfo.bmiHeader = _bmiHeader;
	Buffer->BitmapMemory = VirtualAlloc(
		0, 
		((windowSize.Width * windowSize.Height) * 4),
		MEM_COMMIT, 
		PAGE_READWRITE
	);

	return Buffer;
}


internal void
Win32DisplayBufferInWindow(
	Win32OffScreenBuffer Buffer,
	HDC DeviceContext,
	WindowSize windowSize
)
{
	StretchDIBits(
		DeviceContext,
		0,
		0,
		windowSize.Width,
		windowSize.Height,
		0,
		0,
		Buffer.windowSize.Width,
		Buffer.windowSize.Height,
		Buffer.BitmapMemory,
		&Buffer.BitmapInfo,
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
		Win32DisplayBufferInWindow(
			GlobalBackBuffer,
			DeviceContext, 
			Win32GetWindowSize(Window)
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


int __stdcall wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine,
	int nShowCmd
)
{
	WindowSize windowSize = {
		1280,
		720
	};
	Win32ResizeDIBSection(&GlobalBackBuffer, windowSize);

	WNDCLASS WindowClass = {};
	
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
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

			uint8 XOfSet = 0;
			uint8 YOfSet = 0;

			while (Running)
			{
				MSG Message;
				while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
				{
					if (Message.message == WM_QUIT)
					{
						Running = false;
					}
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
				
				HDC dc = GetDC(Window);
				Win32DisplayBufferInWindow(
					GlobalBackBuffer,
					dc,
					Win32GetWindowSize(Window)
				);
				RenderGradient(GlobalBackBuffer, XOfSet++, YOfSet);
				ReleaseDC(Window, dc);
			}
		}
	}

	return 0;
}
