#include <windows.h>
#include <wingdi.h>

LRESULT CALLBACK 
MainWindowCallback(HWND Window,
					UINT Message,
					WPARAM wParam,
					LPARAM lParam) 
{
	LRESULT result = 0;

	switch(Message) {
		case WM_SIZE: {
			OutputDebugStringA("WM_SIZE\n");
		} break;
		case WM_DESTROY: {
			OutputDebugStringA("WM_DESTROY\n");
		} break;
		case WM_CLOSE: {
			OutputDebugStringA("WM_CLOSE\n");
			DestroyWindow(Window);
		} break;
		case WM_ACTIVATEAPP: {
			OutputDebugStringA("WM_ACTIVATEAPP\n");
		} break;
		case WM_PAINT: {
			PAINTSTRUCT paint;
			static DWORD bgColor = WHITENESS;
			HDC deviceContext = BeginPaint(Window, &paint);
			int X = paint.rcPaint.left;
			int Y = paint.rcPaint.top;
			int Width = paint.rcPaint.right - paint.rcPaint.left;
			int Height = paint.rcPaint.bottom - paint.rcPaint.top;
			PatBlt(deviceContext,
				X,
				Y,
				Width,
				Height,
				bgColor);
			if (bgColor == WHITENESS) {
				bgColor = BLACKNESS;
			} else {
				bgColor = WHITENESS;
			}
			EndPaint(Window, &paint);

		} break;
		default: {
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

	if(RegisterClass(&WindowClass)) {
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
		if (WindowHandle) {
			// ok
			for(;;) {
				MSG Message;
				BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
				if (MessageResult > 0) {
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				} else {
					break;
				}
			}
		} else {
			// error
		}
	} else {
		// fail
	}

	return(0);
}