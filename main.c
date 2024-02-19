#include <windows.h>

// #pragma comment(lib, "user32.lib")


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
			OutputDebugString(L"WM_SIZE\n");
		} break;

		case WM_DESTROY:
		{
			OutputDebugString(L"WM_DESTROY\n");
		} break;
		
		case WM_CLOSE:
		{
			OutputDebugString(L"WM_CLOSE\n");
		} break;

		case WM_ACTIVATEAPP:
		{
			OutputDebugString(L"WM_ACTIVATEAPP\n");
		} break;
		
		case WM_PAINT:
		{
			OutputDebugString(L"PAINT\n");
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);
			
			int X = Paint.rcPaint.left;
			int Y = Paint.rcPaint.top;
			int Heigth = Paint.rcPaint.bottom - Paint.rcPaint.top;
			int Width = Paint.rcPaint.right - Paint.rcPaint.left;

			PatBlt(
				DeviceContext,
				X,
				Y,
				Width,
				Heigth,
				WHITENESS
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
			MSG Message;
			while (1)
			{
				BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
				if (MessageResult > 0)
				{
					TranslateMessage(&Message);
					DispatchMessage(&Message);
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
