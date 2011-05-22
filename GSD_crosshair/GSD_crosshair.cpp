#include <Windows.h>
#include <WindowsX.h>
#include "GSD_crosshair.h"
#pragma comment(lib, "gsd.lib.vc")
#include <math.h>
#include "gsd.h"
#include <CommCtrl.h>
#include "Util.h"
#include <exception>
#include <GdiPlus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

#define MAX_LOADSTRING 100
#define MUTEX_NAME L"GSD_crosshair"

// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名

HWND g_hMainDlg = NULL;
HWND g_hWnd = NULL;

#define WM_TASKTRAY (WM_APP + 1)
#define ID_TASKTRAY 1

#define S_TASKTRAY_TIPS L"GSD_Crosshair"

BOOL GetClsidEncoderFromMimeType(LPCTSTR format, LPCLSID lpClsid)
{
	UINT num, size;
	if( ::GetImageEncodersSize(&num, &size) != Ok ){
		::ShowLastError();
		return FALSE;
	}

	// バッファを確保
	ImageCodecInfo *info = (ImageCodecInfo *)::GlobalAlloc(GMEM_FIXED, size);

	// エンコーダーの情報を転送
	if( ::GetImageEncoders(num, size, info) != Ok ){
		::GlobalFree(info);
		return FALSE;
	}

	for(UINT i=0; i<num; i++){
		if( wcscmp(info[i].MimeType, format) == 0 ){
			*lpClsid = info[i].Clsid;
			::GlobalFree(info); // found
			return TRUE;
		}
	}

	::GlobalFree(info); // not found
	return FALSE;
}


BOOL GetClsidEncoderFromFileName(LPCTSTR fileName, LPCLSID lpClsid)
{
	UINT num, size;
	if( ::GetImageEncodersSize(&num, &size) != Ok ){
		::ShowLastError();
		return FALSE;
	}

	// バッファを確保
	ImageCodecInfo *info = (ImageCodecInfo *)::GlobalAlloc(GMEM_FIXED, size);

	// エンコーダーの情報を転送
	if( ::GetImageEncoders(num, size, info) != Ok ){
		::GlobalFree(info);
		return FALSE;
	}

	for(UINT i=0; i<num; i++){
		if( PathMatchSpecW(fileName, info[i].FilenameExtension)){
			*lpClsid = info[i].Clsid;
			::GlobalFree(info); // found
			return TRUE;
		}
	}

	::GlobalFree(info); // not found
	return FALSE;
}

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

	// 多重起動防止
	CMutex mutex;
	try{
		mutex.createMutex(MUTEX_NAME);
	}catch(std::exception e){
		::ErrorMessageBox(L"多重起動です");
		exit(0);
	}

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

	::TasktrayAddIcon(hInstance, WM_TASKTRAY, ID_TASKTRAY,
		IDI_GSD_CROSSHAIR, S_TASKTRAY_TIPS, hWnd);

	UpdateWindow(hWnd);
	return TRUE;
}

// GSDフック開始します
// 任意のbitmapを描画します
BOOL GUI_GSD_Start(LPCTSTR imageFilePath)
{	
	if(!::PathFileExists(imageFilePath)){
		::ErrorMessageBox(L"ファイルが存在しません: %s", imageFilePath);
		return FALSE;
	}

	ULONG_PTR token;
	::GdiplusStartupInput input;
	::GdiplusStartup(&token, &input, NULL);

	Bitmap *b = new Bitmap(imageFilePath, FALSE);
	if(b == NULL){
		::ShowLastError();
		return FALSE;
	}
	
	// 利用準備
	GSD_DataLock();
	
	// GSDの初期化
	if( !GSD_Initialize() ){
		ShowLastError();
		return FALSE;
	}
	
	int textureIndex = 0;

	trace(L"max texture num: %d\n", GSD_GetMaxTextureNum());
	trace(L"max texture size: %d\n", ::GSD_GetMaxTextureSize());

	DWORD size = b->GetWidth(); // 縦size x 横sizeってこと、現状正方形しか無理っぽい
	GSD_InitTexture(&size, 1);

	::GSD_DataLock();
	::GSD_TextureInfo info;
	::GSD_GetTexture(0, &info);

	trace(L"size: %d\n", info.texSize);
		
	UINT w = size;
	UINT h = size;
	for(UINT x=0; x<w; x++){
		for(UINT y=0; y<h; y++){
			int k = (y * w + x) * 4;
			
			Color color;
			if(b->GetPixel(x, y, &color) == Ok){
				// ピクセル取得に成功したときはそのまま描画
				info.data[k+0] = (BYTE)color.GetB();	// B
				info.data[k+1] = (BYTE)color.GetG();	// G
				info.data[k+2] = (BYTE)color.GetR();	// R
				info.data[k+3] = (BYTE)color.GetA();	// A
			}else{
				// 失敗したときは透過100%で、本来表示されるべき画素を表示
				info.data[k+0] = (BYTE)0;	// B
				info.data[k+1] = (BYTE)0;	// G
				info.data[k+2] = (BYTE)0;	// R
				info.data[k+3] = (BYTE)0;	// A
			}
		}
	}

	info.active = TRUE;
	info.x = info.y = 0;
	info.color = 0xffffffff;
	info.align = DT_CENTER | DT_VCENTER; // 縦中央、横中央に描画
	
	if( !GSD_SetTexture(textureIndex, &info) ){
		::ShowLastError();
		::GSD_DataUnlock();
		return FALSE;
	}
	trace(L"set texture\n");

	GSD_DataUnlock();

	delete b;
	::GdiplusShutdown(token);
	return TRUE;
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

UINT taskBarMsg = 0;
BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	// タスクトレイ復帰用
	taskBarMsg = RegisterWindowMessage(TEXT("TaskbarCreated"));

	if( !::GUI_GSD_Start(L"crosshair.png") ){
		::ShowLastError();
		return FALSE;
	}
	return TRUE;
}

BOOL OnDestroy(HWND hWnd)
{
	GUI_GSD_Finalize();
	::TasktrayDeleteIcon(hWnd, ID_TASKTRAY);
	PostQuitMessage(0);
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message){
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);

	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDM_EXIT:
			::GUI_GSD_Stop();
			::GUI_GSD_Finalize();
			::DestroyWindow(hWnd);
			break;
		}
		return TRUE;

	case WM_TASKTRAY:
		switch(lParam){
		case WM_RBUTTONDOWN:
			::ShowContextMenu(hWnd, IDR_MENU);
			break;
		}
		return TRUE;
	default:
		if(message == taskBarMsg){
			TasktrayAddIcon(hInst, WM_TASKTRAY, ID_TASKTRAY, IDI_GSD_CROSSHAIR, S_TASKTRAY_TIPS, hWnd);
			return 0;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
