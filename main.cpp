#include <windows.h>

#define internal static
#define local_persist static
#define global_variable static

global_variable bool Running;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable HBITMAP BitmapHandle;
global_variable HDC BitmapDC;

internal void
ResizeDIBSection(int Width, int Height)
{

	if (BitmapHandle) 
	{
		DeleteObject(BitmapHandle);
	}
	
	if (!BitmapDC)
	{
		BitmapDC = CreateCompatibleDC(0);
	}

	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biWidth = Width;
	BitmapInfo.bmiHeader.biHeight = Height;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;

	BitmapHandle = CreateDIBSection(
		BitmapDC,
		&BitmapInfo,
		DIB_RGB_COLORS,
		&BitmapMemory,
		0, 0);
}

internal void 
Win32UpdateWindow(HDC DeviceContext, int X, int Y, int Width, int Height)
{
	
	StretchDIBits(
		DeviceContext,
		X, Y, Width, Height,
		X, Y, Width, Height,	
		BitmapMemory,
		&BitmapInfo,
		DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK 
MainWindowCallback(HWND Window,
					UINT Message,
					WPARAM wParam,
					LPARAM lParam) 
{
	LRESULT result = 0;

	switch(Message) {
		case WM_SIZE: 
		{
			RECT rect = {};
			GetClientRect(Window, &rect);
			int Width = rect.right - rect.left;
			int Height = rect.bottom - rect.top;
			ResizeDIBSection(Width, Height);
			OutputDebugStringA("WM_SIZE\n");
		} break;
		case WM_DESTROY:
		{
			OutputDebugStringA("WM_DESTROY\n");
			Running = false;
		} break;
		case WM_CLOSE: 
		{
			OutputDebugStringA("WM_CLOSE\n");
			Running = false;
		} break;
		case WM_ACTIVATEAPP: 
		{
			OutputDebugStringA("WM_ACTIVATEAPP\n");
		} break;
		case WM_PAINT: 
		{
			PAINTSTRUCT paint;
			HDC deviceContext = BeginPaint(Window, &paint);
			int X = paint.rcPaint.left;
			int Y = paint.rcPaint.top;
			int Width = paint.rcPaint.right - paint.rcPaint.left;
			int Height = paint.rcPaint.bottom - paint.rcPaint.top;
			Win32UpdateWindow(deviceContext, X, Y, Width, Height);
			EndPaint(Window, &paint);
		} break;
		default: 
		{
			// OutputDebugStringA("default\n");
			result = DefWindowProc(Window, Message, wParam, lParam);
		} break;
	}

	return(result);
}

int CALLBACK
WinMain(HINSTANCE hInstance,
		HINSTANCE hPrevInstance, 
		LPSTR hlpCmdLine, 
		int nCmdShow)
{
	// MessageBoxW(0, L"This is message box", L"Basdsf", MB_OK|MB_ICONINFORMATION);
	


	WNDCLASS WindowClass = {};

	WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	WindowClass.lpfnWndProc = MainWindowCallback;
	WindowClass.hInstance = hInstance;
	// WindowClass.hIcon = ;
	WindowClass.lpszClassName = "YourWindow";

	if (RegisterClass(&WindowClass)) 
	{
		// ok
		HWND WindowHandle = CreateWindowEx(
			0,
			WindowClass.lpszClassName,
		  	"Window Name",
		  	WS_OVERLAPPEDWINDOW|WS_VISIBLE,
		  	CW_USEDEFAULT,
		  	CW_USEDEFAULT,
		  	CW_USEDEFAULT,
		  	CW_USEDEFAULT,
		  	0,
		  	0,
		  	hInstance,
		  	0
		);
		if (WindowHandle) 
		{
			Running = true;
			// ok
			while(Running) 
			{
				MSG Message;
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
		else 
		{
			// error
		}
	} 
	else 
	{
		// error
	}

	return(0);
}