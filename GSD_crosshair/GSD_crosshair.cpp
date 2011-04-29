// GSD_crosshair.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "GSD_crosshair.h"

#pragma comment(lib, "gsd.lib.vc")

#include <math.h>
#include "gsd.h"

#include <CommCtrl.h>

#define MAX_LOADSTRING 100

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
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GSD_CROSSHAIR));

	// ���C�� ���b�Z�[�W ���[�v:
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
//  �֐�: MyRegisterClass()
//
//  �ړI: �E�B���h�E �N���X��o�^���܂��B
//
//  �R�����g:
//
//    ���̊֐�����юg�����́A'RegisterClassEx' �֐����ǉ����ꂽ
//    Windows 95 ���O�� Win32 �V�X�e���ƌ݊�������ꍇ�ɂ̂ݕK�v�ł��B
//    �A�v���P�[�V�������A�֘A�t����ꂽ
//    �������`���̏������A�C�R�����擾�ł���悤�ɂ���ɂ́A
//    ���̊֐����Ăяo���Ă��������B
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
//   �֐�: InitInstance(HINSTANCE, int)
//
//   �ړI: �C���X�^���X �n���h����ۑ����āA���C�� �E�B���h�E���쐬���܂��B
//
//   �R�����g:
//
//        ���̊֐��ŁA�O���[�o���ϐ��ŃC���X�^���X �n���h����ۑ����A
//        ���C�� �v���O���� �E�B���h�E���쐬����ѕ\�����܂��B
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // �O���[�o���ϐ��ɃC���X�^���X�������i�[���܂��B

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


// �o�[�W�������{�b�N�X�̃��b�Z�[�W �n���h���[�ł��B
bool apiHookEnable = false;
INT_PTR CALLBACK Main(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		// �ڐ��蕝�̂����Ă�
		SendDlgItemMessage(hDlg, IDC_SLIDER_R, TBM_SETRANGE, NULL, MAKELPARAM(0, 255) );
		SendDlgItemMessage(hDlg, IDC_SLIDER_G, TBM_SETRANGE, NULL, MAKELPARAM(0, 255) );
		SendDlgItemMessage(hDlg, IDC_SLIDER_B, TBM_SETRANGE, NULL, MAKELPARAM(0, 255) );
		SendDlgItemMessage(hDlg, IDC_SLIDER_A, TBM_SETRANGE, NULL, MAKELPARAM(0, 255) );

		// �X���C�_�[�̐��l�����̏�����
		::SetDlgItemInt(hDlg, IDC_EDIT_R, 0, TRUE);
		::SetDlgItemInt(hDlg, IDC_EDIT_G, 255, TRUE);
		::SetDlgItemInt(hDlg, IDC_EDIT_B, 0, TRUE);
		::SetDlgItemInt(hDlg, IDC_EDIT_A, 255, TRUE); // default = 100%�s����

		// �X���C�_�[��UI�����̏�����
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
				// Dlg����F���擾����
				int red = ::GetDlgItemInt(hDlg, IDC_EDIT_R, NULL, TRUE);
				int green = ::GetDlgItemInt(hDlg, IDC_EDIT_G, NULL, TRUE);
				int blue = ::GetDlgItemInt(hDlg, IDC_EDIT_B, NULL, TRUE);
				int alpha = ::GetDlgItemInt(hDlg, IDC_EDIT_A, NULL, TRUE);
				
				GUI_gsd_start(red, green, blue, alpha);

				::SetWindowText(g_hMainDlg, L"GSD Crosshair (API�t�b�N��)");
				::SetDlgItemText(g_hMainDlg, IDOK, L"���~");

				apiHookEnable = true;
				return (INT_PTR)TRUE;
			}
			else
			{
				GUI_gsd_stop();
				
				::SetWindowText(g_hMainDlg, L"GSD Crosshair");
				::SetDlgItemText(g_hMainDlg, IDOK, L"�J�n");

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
//  �֐�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  �ړI:  ���C�� �E�B���h�E�̃��b�Z�[�W���������܂��B
//
//  WM_COMMAND	- �A�v���P�[�V���� ���j���[�̏���
//  WM_PAINT	- ���C�� �E�B���h�E�̕`��
//  WM_DESTROY	- ���~���b�Z�[�W��\�����Ė߂�
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
		// �I�����ꂽ���j���[�̉��:
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
		// TODO: �`��R�[�h�������ɒǉ����Ă�������...
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
