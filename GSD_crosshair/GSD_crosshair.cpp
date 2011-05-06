#include "stdafx.h"
#include "GSD_crosshair.h"
#pragma comment(lib, "gsd.lib.vc")
#include <math.h>
#include "gsd.h"
#include <WindowsX.h>
#include <CommCtrl.h>
#include "Util.h"

#define MAX_LOADSTRING 100

#define HANDLE_DLG_MSG(hwnd, msg, fn) \
    case(msg): \
	return SetDlgMsgResult(hwnd, msg, HANDLE_##msg(hwnd, wParam, lParam,fn)) 

// �O���[�o���ϐ�:
HINSTANCE hInst;								// ���݂̃C���^�[�t�F�C�X
TCHAR szTitle[MAX_LOADSTRING];					// �^�C�g�� �o�[�̃e�L�X�g
TCHAR szWindowClass[MAX_LOADSTRING];			// ���C�� �E�B���h�E �N���X��

HWND g_hMainDlg = NULL;
HWND g_hWnd = NULL;

// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̐錾��]�����܂�:
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

	// TODO: �����ɃR�[�h��}�����Ă��������B
	MSG msg;
	HACCEL hAccelTable;

	// �O���[�o������������������Ă��܂��B
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_GSD_CROSSHAIR, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// �A�v���P�[�V�����̏����������s���܂�:
	if (!InitInstance (hInstance, nCmdShow)){
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GSD_CROSSHAIR));

	// ���C�� ���b�Z�[�W ���[�v:
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
	hInst = hInstance; // �O���[�o���ϐ��ɃC���X�^���X�������i�[���܂�

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	g_hWnd = hWnd;

	if(!hWnd){
		return FALSE;
	}

	UpdateWindow(hWnd);

	return TRUE;
}

// GSD�t�b�N�J�n���܂�
void GUI_GSD_Start(int red, int green, int blue, int alpha)
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

// GSD�t�b�N���I�����܂�
void GUI_GSD_Stop()
{
	::GSD_Finalize();
}

// GSD�t�b�N�̌�Еt�����������܂�
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
	SendDlgItemMessage(hwnd, IDC_SLIDER_R, TBM_SETRANGE, NULL, MAKELPARAM(0, 255) );
	SendDlgItemMessage(hwnd, IDC_SLIDER_G, TBM_SETRANGE, NULL, MAKELPARAM(0, 255) );
	SendDlgItemMessage(hwnd, IDC_SLIDER_B, TBM_SETRANGE, NULL, MAKELPARAM(0, 255) );
	SendDlgItemMessage(hwnd, IDC_SLIDER_A, TBM_SETRANGE, NULL, MAKELPARAM(0, 255) );

	// �X���C�_�[�̐��l�����̏�����
	::SetDlgItemInt(hwnd, IDC_EDIT_R, 0, TRUE);
	::SetDlgItemInt(hwnd, IDC_EDIT_G, 255, TRUE);
	::SetDlgItemInt(hwnd, IDC_EDIT_B, 0, TRUE);
	::SetDlgItemInt(hwnd, IDC_EDIT_A, 255, TRUE); // default = 100%�s����

	// �X���C�_�[��UI�����̏�����
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
			// API�t�b�N���������Ƃ���API�t�b�N�̒�~�������s���܂�
			GUI_GSD_Stop();

			::SetWindowText(g_hMainDlg, L"GSD Crosshair");
			::SetDlgItemText(g_hMainDlg, IDOK, L"�J�n");
		}else{
			// API�t�b�N���ĂȂ������Ƃ��́AAPI�t�b�N�������s���܂�
			int red = ::GetDlgItemInt(hwnd, IDC_EDIT_R, NULL, TRUE);
			int green = ::GetDlgItemInt(hwnd, IDC_EDIT_G, NULL, TRUE);
			int blue = ::GetDlgItemInt(hwnd, IDC_EDIT_B, NULL, TRUE);
			int alpha = ::GetDlgItemInt(hwnd, IDC_EDIT_A, NULL, TRUE);

			GUI_GSD_Start(red, green, blue, alpha);

			::SetWindowText(g_hMainDlg, L"GSD Crosshair (API�t�b�N��)");
			::SetDlgItemText(g_hMainDlg, IDOK, L"���~");
		}
		apiHookEnable = !apiHookEnable;
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
		HANDLE_MSG(hDlg, WM_HSCROLL, Main_OnHScroll);
		HANDLE_MSG(hDlg, WM_INITDIALOG, Main_OnInitDialog);
		HANDLE_MSG(hDlg, WM_COMMAND, Main_OnCommand);
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
