// GSD_crosshair.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "GSD_crosshair.h"

#pragma comment(lib, "gsd.lib.vc")

#include <math.h>
#include "gsd.h"

#include <CommCtrl.h>

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名

HWND g_hMainDlg = NULL;
HWND g_hWnd = NULL;

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: ここにコードを挿入してください。
	MSG msg;
	HACCEL hAccelTable;

	// グローバル文字列を初期化しています。
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_GSD_CROSSHAIR, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// アプリケーションの初期化を実行します:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GSD_CROSSHAIR));

	// メイン メッセージ ループ:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
//  コメント:
//
//    この関数および使い方は、'RegisterClassEx' 関数が追加された
//    Windows 95 より前の Win32 システムと互換させる場合にのみ必要です。
//    アプリケーションが、関連付けられた
//    正しい形式の小さいアイコンを取得できるようにするには、
//    この関数を呼び出してください。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GSD_CROSSHAIR));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_GSD_CROSSHAIR);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   g_hWnd = hWnd;

   if (!hWnd)
   {
      return FALSE;
   }

   //ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void GUI_gsd_start(int red, int green, int blue, int alpha)
{
	const DWORD size = 40;

	GSD_Initialize();
	GSD_InitTexture(&size, 1);

	// main
	GSD_TextureInfo info;

	GSD_DataLock();
	GSD_GetTexture(0, &info);

	int x = 0;
	int y = 0;
	int k = 0;

	int crosshair_red = red;
	int crosshair_green = green;
	int crosshair_blue = blue;
	int crosshair_alpha = alpha;
		
	int bold = 4;
	k = 0;
	for(int i=0; i<(int)info.texSize; i++){
		for(int j=0; j<(int)bold; j++){
			k = (i * info.texSize + j + (info.texSize / 2 - bold)) * 4;
			
			info.data[k+0] = (BYTE)crosshair_blue; // B
			info.data[k+1] = (BYTE)crosshair_green; // G
			info.data[k+2] = (BYTE)crosshair_red; // R
			info.data[k+3] = (BYTE)crosshair_alpha; // A
		}
	}
	for(int i=0; i<bold; i++){
		for(int j=0; j<(int)info.texSize; j++){
			k = ((info.texSize / 2 - bold + i) * info.texSize + j) * 4;
			
			info.data[k+0] = (BYTE)crosshair_blue; // B
			info.data[k+1] = (BYTE)crosshair_green; // G
			info.data[k+2] = (BYTE)crosshair_red; // R
			info.data[k+3] = (BYTE)crosshair_alpha; // A
		}
	}
	
	info.active = TRUE;
	info.x = 0;
	info.y = 0;
	info.color = 0xffffffff;
	info.align = DT_CENTER | DT_VCENTER;

	GSD_SetTexture(0, &info);
	GSD_DataUnlock();
}

void GUI_gsd_stop()
{
	::GSD_Finalize();
}

void GUI_gsd_finalize()
{
	::GSD_Finalize();
}


// バージョン情報ボックスのメッセージ ハンドラーです。
bool apiHookEnable = false;
INT_PTR CALLBACK Main(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		// 目盛り幅のせってい
		SendDlgItemMessage(hDlg, IDC_SLIDER_R, TBM_SETRANGE, NULL, MAKELPARAM(0, 255) );
		SendDlgItemMessage(hDlg, IDC_SLIDER_G, TBM_SETRANGE, NULL, MAKELPARAM(0, 255) );
		SendDlgItemMessage(hDlg, IDC_SLIDER_B, TBM_SETRANGE, NULL, MAKELPARAM(0, 255) );
		SendDlgItemMessage(hDlg, IDC_SLIDER_A, TBM_SETRANGE, NULL, MAKELPARAM(0, 255) );

		// スライダーの数値部分の初期化
		::SetDlgItemInt(hDlg, IDC_EDIT_R, 0, TRUE);
		::SetDlgItemInt(hDlg, IDC_EDIT_G, 255, TRUE);
		::SetDlgItemInt(hDlg, IDC_EDIT_B, 0, TRUE);
		::SetDlgItemInt(hDlg, IDC_EDIT_A, 255, TRUE); // default = 100%不透明

		// スライダーのUI部分の初期化
		SendDlgItemMessage(hDlg, IDC_SLIDER_R, TBM_SETPOS, TRUE, 0); 
		SendDlgItemMessage(hDlg, IDC_SLIDER_G, TBM_SETPOS, TRUE, 255); 
		SendDlgItemMessage(hDlg, IDC_SLIDER_B, TBM_SETPOS, TRUE, 0); 
		SendDlgItemMessage(hDlg, IDC_SLIDER_A, TBM_SETPOS, TRUE, 255); 
		return (INT_PTR)TRUE;

	case WM_HSCROLL:
		if( (HWND)lParam == GetDlgItem(hDlg, IDC_SLIDER_R) ){
			int pos = SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
			::SetDlgItemInt(hDlg, IDC_EDIT_R, pos, TRUE);
		}
		if( (HWND)lParam == GetDlgItem(hDlg, IDC_SLIDER_G) ){
			int pos = SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
			::SetDlgItemInt(hDlg, IDC_EDIT_G, pos, TRUE);
		}
		if( (HWND)lParam == GetDlgItem(hDlg, IDC_SLIDER_B) ){
			int pos = SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
			::SetDlgItemInt(hDlg, IDC_EDIT_B, pos, TRUE);
		}
		if( (HWND)lParam == GetDlgItem(hDlg, IDC_SLIDER_A) ){
			int pos = SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
			::SetDlgItemInt(hDlg, IDC_EDIT_A, pos, TRUE);
		}
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			if(apiHookEnable == false)
			{
				// Dlgから色を取得する
				int red = ::GetDlgItemInt(hDlg, IDC_EDIT_R, NULL, TRUE);
				int green = ::GetDlgItemInt(hDlg, IDC_EDIT_G, NULL, TRUE);
				int blue = ::GetDlgItemInt(hDlg, IDC_EDIT_B, NULL, TRUE);
				int alpha = ::GetDlgItemInt(hDlg, IDC_EDIT_A, NULL, TRUE);
				
				GUI_gsd_start(red, green, blue, alpha);

				::SetWindowText(g_hMainDlg, L"GSD Crosshair (APIフック中)");
				::SetDlgItemText(g_hMainDlg, IDOK, L"中止");

				apiHookEnable = true;
				return (INT_PTR)TRUE;
			}
			else
			{
				GUI_gsd_stop();
				
				::SetWindowText(g_hMainDlg, L"GSD Crosshair");
				::SetDlgItemText(g_hMainDlg, IDOK, L"開始");

				apiHookEnable = false;
				return (INT_PTR)TRUE;
			}
		}
		else if(LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			DestroyWindow(hDlg);
			DestroyWindow(g_hWnd);
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:  メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND	- アプリケーション メニューの処理
//  WM_PAINT	- メイン ウィンドウの描画
//  WM_DESTROY	- 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		g_hMainDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_MAIN_DIALOG), hWnd, Main);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 描画コードをここに追加してください...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		GUI_gsd_finalize();

		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
