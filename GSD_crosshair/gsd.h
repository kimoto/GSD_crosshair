#ifndef GSDH
#define GSDH

#ifdef __cplusplus
extern "C" {
#endif

#define GSD_APIVER 0x00020001

struct GSD_TextureInfo
{
	BOOL active;    // �e�N�X�`����\�����邩�ۂ� (TRUE�AFALSE)
	DWORD texSize;  // �e�N�X�`���̃T�C�Y�AGSD_InitTexture() �Ŏw�肵���l�Ɠ����łȂ���΂Ȃ�Ȃ�
	int x, y;       // �e�N�X�`���̈ʒu (align�̒l�ɂ���Ċ�ʒu���ς��)
	DWORD color;    // ��{�F (�ʏ�͕s�����̔� 0xffffffff)
	DWORD align;    // �e�N�X�`���̔z�u (DT_LEFT�ADT_CENTER�ADT_RIGHT�ADT_TOP�ADT_VCENTER�ADT_BOTTOM)
	BYTE *data;     // �e�N�X�`�����i�[�����o�b�t�@ �ݒ�̂ݕύX����ꍇ��NULL
};

BOOL WINAPI GSD_Initialize();
void WINAPI GSD_Finalize();
int WINAPI GSD_GetMaxTextureNum();
int WINAPI GSD_GetMaxTextureSize();
DWORD WINAPI GSD_GetLastUpdate();
DWORD WINAPI GSD_GetVersion();
DWORD WINAPI GSD_GetApiVersion();
BOOL WINAPI GSD_CheckApiVersion(DWORD ver);
BOOL WINAPI GSD_InitTexture(const DWORD *texSize, int num);
BOOL WINAPI GSD_SetTexture(int index, const struct GSD_TextureInfo *info);
BOOL WINAPI GSD_GetTexture(int index, struct GSD_TextureInfo *info);
void WINAPI GSD_DelTexture(int index);
void WINAPI GSD_SetTimerOrg(__int64 org);
void WINAPI GSD_SetFpsLimit(double fps);
void WINAPI GSD_SetFpsAlign(DWORD align);
void WINAPI GSD_ShowFps(BOOL show);
void WINAPI GSD_UpdateOnActive(BOOL uoa);
void WINAPI GSD_DataLock();
void WINAPI GSD_DataUnlock();

#ifdef __cplusplus
}

class GSD_DataLocker
{
public:
	GSD_DataLocker()
	{
		GSD_DataLock();
	}

	~GSD_DataLocker()
	{
		GSD_DataUnlock();
	}
};

#endif

#endif
