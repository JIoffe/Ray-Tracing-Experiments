/////////////////////////////////////////////////////
// (c) 2011 Foofles , Jason Ioffe
// Pure CPU based Ray Tracer. Slow as shit.
// Do not redistribute.
/////////////////////////////////////////////////////

#include "FTCommons.h"
#include <Commdlg.h>
#include <commctrl.h>
#include "resource.h"

#pragma comment (lib,"Comctl32.lib ")
#pragma comment (lib,"Comdlg32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

using namespace std; //ew STDs
using namespace Gdiplus;

HWND hViewWindow;
HWND HoldUpBitch = NULL;
cRayTracer RayTracer;
cCamera MainCamera;
OPENFILENAME ofn;
char cFileName[MAX_PATH];

BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_COMMAND:{
            switch(LOWORD(wParam))
            {
                case IDOK:{
                    EndDialog(hwnd, IDOK);
					break;
				}
				default:
					break;
            }
			break;
		}
		case WM_CLOSE:{
            EndDialog(hwnd, IDOK);
			break;
		}
        default:
            return FALSE;
    }
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg){
		case WM_CLOSE:{
			DestroyWindow(hwnd);
			break;
		}
		case WM_DESTROY:{
			RayTracer.Cleanup();
			PostQuitMessage(0);
			break;
		}
		case WM_PAINT:{
			//PAINTSTRUCT ps;
			//HDC hdc = BeginPaint(hwnd, &ps);
			HDC hdc = GetDC(hwnd);
			Graphics g(hdc);
			g.DrawImage(RayTracer.GetRenderTarget(),0,0);		
			g.ReleaseHDC(hdc);
			//EndPaint(hwnd, &ps);
			break;
		}
		case WM_COMMAND:{
		    switch(LOWORD(wParam)){
				case IDM_SAVE___1:{
					if(GetSaveFileName(&ofn) != 0){
						RayTracer.SaveImage(cFileName);
					}
					break;
				}
				case IDM_ABOUT1:{
					DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT), hwnd, AboutDlgProc);
					break;
				}
				default:
					break;
			}
			break;
		}
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam); //Defer processing since we ignored this event.
	}
	return 0;
}

INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){
	MSG msg;
	WNDCLASSEX wc;
	int width = 0;
	int height = 0;
	{
		int Answer = MessageBox(NULL, "Check out this awesome interface.\n Yes = 320 x 240, No = 640 x 480, Cancel = 1024 x 780 \n Higher resolution increases render time!", "Choose Resolution", MB_YESNOCANCEL | MB_ICONQUESTION);
		switch(Answer){
			case IDYES:{
				width = 320;
				height = 240;
				break;
			}
			case IDNO:{
				width = 640;
				height = 480;
				break;
			}
			case IDCANCEL:{
				width = 1024;
				height = 780;
				break;
			}
		}
	}


	RECT WindowRect;
	WindowRect.bottom = height;
	WindowRect.top = 0;
	WindowRect.left = 0;
	WindowRect.right = width;
	//Init GDI+ and common controls
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
  
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	InitCommonControls();

							ofn.hInstance = hInstance;
						ofn.lStructSize = sizeof(OPENFILENAME);
						ofn.hwndOwner = NULL  ;
						ofn.lpstrFile = cFileName ;
						ofn.nMaxFile = MAX_PATH;
						ofn.lpstrFilter = "PNG Image\0*.png\0\0";
						ofn.nFilterIndex = 1;
						ofn.nMaxFileTitle = MAX_PATH ;
						ofn.lpstrInitialDir= NULL ;
						ofn.Flags = OFN_OVERWRITEPROMPT;
						ofn.lpstrTitle = NULL;
						ofn.lpstrFileTitle = NULL;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    //wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszClassName = "FoofTrace";
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
	wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU1);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Couldn't create preview window! Press OK to exit.", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
	{
		RECT clientRect;
		clientRect.left = 0;
		clientRect.top = 0;
		clientRect.right = width;
		clientRect.bottom = height;
		AdjustWindowRectEx(&clientRect, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, true, 0);
		hViewWindow = CreateWindowEx(WS_EX_CLIENTEDGE,"FoofTrace","Foof Ray Tracer", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU /*|  WS_HSCROLL | WS_VSCROLL*/,
			CW_USEDEFAULT, CW_USEDEFAULT, clientRect.right, clientRect.bottom, NULL, NULL, hInstance, NULL);
	}
	
		HoldUpBitch = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, NULL);
		SetForegroundWindow(HoldUpBitch);
		//Setup the progress bar
		HWND PBar = GetDlgItem(HoldUpBitch, IDC_PROGRESS1);
		SendMessage(PBar, PBM_SETRANGE, 0, MAKELPARAM(0, height));
		SendMessage(PBar, PBM_SETSTEP, (WPARAM) 1, 0); 
		//SendMessage(PBar, PBM_SETPOS, (WPARAM) 0, 0); 
		RayTracer.ProgressDlg = PBar;
		RayTracer.Initialize(width, height);
		RayTracer.RenderScene(&MainCamera);
		DestroyWindow(HoldUpBitch);
		UpdateWindow(hViewWindow);
		ShowWindow(hViewWindow, SW_SHOWDEFAULT);
		
	while(GetMessage(&msg, NULL, 0, 0) > 0) // if NOT WM_QUIT (0) or errors (-1)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
	UnregisterClass("FoofTrace", hInstance);
	return 0;
}