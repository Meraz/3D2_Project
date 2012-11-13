#include <Windows.h>
#include "Renderer.h"

HWND		g_hWnd;
HINSTANCE	g_hInst;
POINT		gLastMousePosition;

HRESULT             InitWindow( HINSTANCE hInstance, int nCmdShow );
LRESULT CALLBACK    WindowProcedure(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	Renderer renderer;
	
	if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
		return 0;

	renderer.Initialize(&g_hWnd);

	// Main message loop
	MSG msg = {0};
	while(WM_QUIT != msg.message)
	{
		while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE) == TRUE )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		renderer.Update();
		renderer.Draw();
	}
	return (int) msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WindowProcedure;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = 0;
	wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = "Lab3";
	wcex.hIconSm        = 0;
	if(!RegisterClassEx(&wcex))
		return E_FAIL;


	// Create window
	g_hInst = hInstance; 
	RECT rc = { 0, 0, 1024, 768};
	GetCamera().SetLens(0.25f*PI, ScreenAspect, NearPlane, FarPlane);


	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	
	if(!(g_hWnd = CreateWindow("Lab3",
							"Lab3",
							WS_OVERLAPPEDWINDOW,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							rc.right - rc.left,
							rc.bottom - rc.top,
							NULL,
							NULL,
							hInstance,
							NULL)))
	{
		return E_FAIL;
	}

	
	ShowWindow( g_hWnd, nCmdShow );
	return S_OK;
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WindowProcedure( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	HDC hdc;

	float dx = 0;
	float dy = 0;
	POINT lMousePosition;

	switch (message) 
	{
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_LBUTTONDOWN:
			if( wParam & MK_LBUTTON )
			{
				SetCapture(g_hWnd);

				gLastMousePosition.x = LOWORD(lParam);
				gLastMousePosition.y = HIWORD(lParam);
			}
			break;

		case WM_LBUTTONUP:
			ReleaseCapture();
			break;

		case WM_MOUSEMOVE:
			if( wParam & MK_LBUTTON)
			{
				lMousePosition.x = LOWORD( lParam );
				lMousePosition.y = HIWORD( lParam );
		
				dx = lMousePosition.x - gLastMousePosition.x;
				dy = lMousePosition.y - gLastMousePosition.y;

				GetCamera().Yaw( dx*0.8 );
				GetCamera().Pitch( -dy*0.8 );
				gLastMousePosition = lMousePosition;
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}


	return 0;
}