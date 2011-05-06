#include "stdafx.h"
#include "GSD_crosshair.h"
#pragma comment(lib, "gsd.lib.vc")
#include <math.h>
#include "gsd.h"
#include <WindowsX.h>
#include <CommCtrl.h>
#include "Util.h"

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
	if (!InitInstance (hInstance, nCmdShow)){
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GSD_CROSSHAIR));

	// メイン メッセージ ループ:
	while (GetMessage(&msg, NULL, 0, 0)){
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int) msg.wParam;
}

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

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	hInst = hInstance; // グローバル変数にインスタンス処理を格納します

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	g_hWnd = hWnd;

	if(!hWnd){
		return FALSE;
	}

	UpdateWindow(hWnd);
	return TRUE;
}

// GSDフック開始します
void GUI_GSD_Start(int red, int green, int blue, int alpha)
{
	const DWORD size = 40;

	// 利用準備
	GSD_DataLock();

	// GSDの初期化
	if( !GSD_Initialize() ){
		ShowLastError();
		return;
	}
	if( !GSD_InitTexture(&size, 1) ){
		ShowLastError();
		::GSD_DataUnlock();
		return;
	}

	GSD_TextureInfo info;
	if( !GSD_GetTexture(0, &info) ){
		ShowLastError();
		::GSD_DataUnlock();
		return;
	}

	// 描画処理メイン
	// 十字のテクスチャ生成
	int bold = 4;
	int k = 0;
	for(int i=0; i<(int)info.texSize; i++){
		for(int j=0; j<(int)bold; j++){
			k = (i * info.texSize + j + (info.texSize / 2 - bold)) * 4; // 4 mean 4byte (R,G,B,A)

			info.data[k+0] = (BYTE)blue; // B
			info.data[k+1] = (BYTE)green; // G
			info.data[k+2] = (BYTE)red; // R
			info.data[k+3] = (BYTE)alpha; // A
		}
	}
	for(int i=0; i<bold; i++){
		for(int j=0; j<(int)info.texSize; j++){
			k = ((info.texSize / 2 - bold + i) * info.texSize + j) * 4; // 4 mean 4byte (R,G,B,A)

			info.data[k+0] = (BYTE)blue; // B
			info.data[k+1] = (BYTE)green; // G
			info.data[k+2] = (BYTE)red; // R
			info.data[k+3] = (BYTE)alpha; // A
		}
	}

	info.active = TRUE;
	info.x = info.y = 0;
	info.color = 0xffffffff;
	info.align = DT_CENTER | DT_VCENTER;

	if( !GSD_SetTexture(0, &info) ){
		::ShowLastError();
		::GSD_DataUnlock();
		return;
	}

	GSD_DataUnlock();
}

// GSDフックを終了します
void GUI_GSD_Stop()
{
	::GSD_Finalize();
}

// GSDフックの後片付け処理をします
void GUI_GSD_Finalize()
{
	::GSD_Finalize();
}

void Main_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	if( hwndCtl == GetDlgItem(hwnd, IDC_SLIDER_R) ){
		pos = (int)::SendDlgItemMessage(hwnd, IDC_SLIDER_R, TBM_GETPOS, 0, 0);
		::SetDlgItemInt(hwnd, IDC_EDIT_R, pos, TRUE);
	}
	if( hwndCtl == GetDlgItem(hwnd, IDC_SLIDER_G) ){
		pos = (int)::SendDlgItemMessage(hwnd, IDC_SLIDER_G, TBM_GETPOS, 0, 0);
		::SetDlgItemInt(hwnd, IDC_EDIT_G, pos, TRUE);
	}
	if( hwndCtl == GetDlgItem(hwnd, IDC_SLIDER_B) ){
		pos = (int)::SendDlgItemMessage(hwnd, IDC_SLIDER_B, TBM_GETPOS, 0, 0);
		::SetDlgItemInt(hwnd, IDC_EDIT_B, pos, TRUE);
	}
	if( hwndCtl == GetDlgItem(hwnd, IDC_SLIDER_A) ){
		pos = (int)::SendDlgItemMessage(hwnd, IDC_SLIDER_A, TBM_GETPOS, 0, 0);
		::SetDlgItemInt(hwnd, IDC_EDIT_A, pos, TRUE);
	}
}

BOOL Main_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	// 最前面に表示
	::SetWindowTopMost(hwnd);

	// スライダーの数値範囲制限
	SendDlgItemMessage(hwnd, IDC_SLIDER_R, TBM_SETRANGE, NULL, MAKELPARAM(0, 255) );
	SendDlgItemMessage(hwnd, IDC_SLIDER_G, TBM_SETRANGE, NULL, MAKELPARAM(0, 255) );
	SendDlgItemMessage(hwnd, IDC_SLIDER_B, TBM_SETRANGE, NULL, MAKELPARAM(0, 255) );
	SendDlgItemMessage(hwnd, IDC_SLIDER_A, TBM_SETRANGE, NULL, MAKELPARAM(0, 255) );

	// スライダーの数値部分の初期化
	::SetDlgItemInt(hwnd, IDC_EDIT_R, 0, TRUE);
	::SetDlgItemInt(hwnd, IDC_EDIT_G, 255, TRUE);
	::SetDlgItemInt(hwnd, IDC_EDIT_B, 0, TRUE);
	::SetDlgItemInt(hwnd, IDC_EDIT_A, 255, TRUE); // default = 100%不透明

	// スライダーのUI部分の初期化
	SendDlgItemMessage(hwnd, IDC_SLIDER_R, TBM_SETPOS, TRUE, 0); 
	SendDlgItemMessage(hwnd, IDC_SLIDER_G, TBM_SETPOS, TRUE, 255); 
	SendDlgItemMessage(hwnd, IDC_SLIDER_B, TBM_SETPOS, TRUE, 0); 
	SendDlgItemMessage(hwnd, IDC_SLIDER_A, TBM_SETPOS, TRUE, 255);

	return TRUE;
}

void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT code)
{
	static bool apiHookEnable = false;

	switch(id)
	{
	case IDOK:
		if(apiHookEnable){
			// APIフック中だったときはAPIフックの停止処理を行います
			GUI_GSD_Stop();
		}else{
			// APIフックしてなかったときは、APIフック処理を行います
			int red = ::GetDlgItemInt(hwnd, IDC_EDIT_R, NULL, TRUE);
			int green = ::GetDlgItemInt(hwnd, IDC_EDIT_G, NULL, TRUE);
			int blue = ::GetDlgItemInt(hwnd, IDC_EDIT_B, NULL, TRUE);
			int alpha = ::GetDlgItemInt(hwnd, IDC_EDIT_A, NULL, TRUE);

			GUI_GSD_Start(red, green, blue, alpha);
		}
		apiHookEnable = !apiHookEnable;

		::SetWindowTextFormat(g_hMainDlg, L"%s%s", L"GSD Crosshair", apiHookEnable ? L"(APIフック中)" : L"");
		::SetDlgItemText(g_hMainDlg, IDOK, apiHookEnable ? L"中止" : L"開始");
		break;
	case IDCANCEL:
		EndDialog(hwnd, code);
		DestroyWindow(hwnd);
		DestroyWindow(g_hWnd);
		break;
	}
}

INT_PTR CALLBACK Main(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_DLG_MSG(hDlg, WM_HSCROLL, Main_OnHScroll);
		HANDLE_DLG_MSG(hDlg, WM_INITDIALOG, Main_OnInitDialog);
		HANDLE_DLG_MSG(hDlg, WM_COMMAND, Main_OnCommand);
	}
	return (INT_PTR)FALSE;
}

BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	g_hMainDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_MAIN_DIALOG), hWnd, Main);
	if(g_hMainDlg == NULL){
		::ShowLastError();
		return FALSE;
	}
	return TRUE;
}

BOOL OnDestroy(HWND hWnd)
{
	GUI_GSD_Finalize();
	PostQuitMessage(0);
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message){
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
