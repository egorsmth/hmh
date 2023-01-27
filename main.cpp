#include <windows.h>
#include <stdint.h>

#define internal static
#define local_persist static
#define global_variable static

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

global_variable bool Running;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;
global_variable	int BytesPerPixel = 4;

internal void
DrawGradient(int XOffset, int YOffset)
{
	uint8 *Row = (uint8 *)BitmapMemory;
	int Pitch = BitmapWidth*BytesPerPixel;

	for (int Y = 0; Y < BitmapHeight; ++Y)
	{
		uint32 *Pixel = (uint32 *)Row;
		for (int X = 0; X < BitmapWidth; ++X)
		{
			// uint32 pixel is 00 00 00 00 -> BB GG RR xx
			uint8 B = X + XOffset;
			uint8 G = Y + YOffset;
			uint8 R = XOffset;
			
			*Pixel = (R << 16 | G << 8 | B);
			++Pixel;
		}

		Row += Pitch;
	}
}

internal void
ResizeDIBSection(int Width, int Height)
{
	if (BitmapMemory)
	{
		VirtualFree(BitmapMemory, 0, MEM_RELEASE);
	}

	BitmapWidth = Width;
	BitmapHeight = Height;

	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biWidth = BitmapWidth;
	BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;

	int BitmapMemorySize = BytesPerPixel * BitmapWidth * BitmapHeight;
	BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}

internal void 
Win32UpdateWindow(HDC DeviceContext, RECT *WindowRect, int X, int Y, int Width, int Height)
{
	int WindowWidth = WindowRect->right - WindowRect->left;
	int WindowHeight = WindowRect->bottom - WindowRect->top;

	StretchDIBits(
		DeviceContext,
		0, 0, BitmapWidth, BitmapHeight,
		0, 0, WindowWidth, WindowHeight,	
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
			RECT rect;
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
			
			RECT rect;
			GetClientRect(Window, &rect);

			Win32UpdateWindow(deviceContext, &rect,  X, Y, Width, Height);
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
			int XOffset = 0;
			int YOffset = 0;
			while(Running) 
			{
				MSG Message;
				while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}

				DrawGradient(XOffset, YOffset);

				HDC DeviceContext = GetDC(WindowHandle);
				RECT Rect;
				GetClientRect(WindowHandle, &Rect);
				int WindowWidth = Rect.right - Rect.left;
				int WindowHeight = Rect.bottom - Rect.top;
				Win32UpdateWindow(DeviceContext, &Rect, 0, 0, WindowWidth, WindowHeight);
				ReleaseDC(WindowHandle, DeviceContext);

				++XOffset;
				++YOffset;
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