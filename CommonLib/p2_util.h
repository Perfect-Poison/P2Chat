#pragma once
/*!
 * \file 	p2_util.h
 *
 * \author 	BrianYi
 * \date 	2017/07/30
 *
 * \brief	���ļ�Ϊ������Ŀ��Ҫ�õ��ĳ��ù��ߺ���
 */
#include "p2_common.h"
P2_NAMESPACE_BEG

/**
 *	�ַ���������غ���
 */
char *bin_to_strA(const BYTE* pData, size_t size, char *pStr);
WCHAR *bin_to_strW(const BYTE* pData, size_t size, WCHAR *pStr);
size_t str_to_binA(const char* pStr, BYTE *pData, size_t size);
size_t str_to_binW(const WCHAR* pStr, BYTE *pData, size_t size);
void str_stripA(char *str);
void str_stripW(WCHAR *str);

#ifdef UNICODE
#define bin_to_str  bin_to_strW
#define str_to_bin  str_to_binW
#define str_strip   str_stripW
#else 
#define bin_to_str  bin_to_strA
#define str_to_bin  str_to_binA
#define str_strip   str_stripA
#endif 


/**
 *	�ڴ������غ���
 */
void *memdup(const void *data, size_t size);

/**
 *	ԭ�Ӳ�����غ���
 */
unsigned int atomic_add(unsigned int *area, int val);
unsigned int atomic_or(unsigned int *area, unsigned int val);
unsigned int atomic_sub(unsigned int *area, int val);

unsigned int compare_and_store(unsigned int oval, unsigned int nval, unsigned int *area);

/**
 *	�¼�������غ���
 */
int select_watchevent(struct eventreq *req, int which);
int select_modwatch(struct eventreq *req, int which);
int select_waitevent(struct eventreq *req);

/**
 *	ʱ�����
 */
int64 GetCurrentTimeMicroS();   // ΢�
int64 GetCurrentTimeMilliS();   // ���뼶
int64 GetCurrentTimeS();        // �뼶
TCHAR* FormatCalendarTime(TCHAR *buffer);
TCHAR* FormatLogCalendarTime(TCHAR *buffer);


/**
 *	��־���
 *  debug level: 0~9
 *  0~4 ���Ժ��Ե�
 *  5~6 һ�����
 *  7~9 ���ش���ᵼ��ϵͳ���в����������
 */
bool log_open(const TCHAR *logName, log_flags flags, log_rotation_policy rotationPolicy, int maxLogSize, int historySize, const TCHAR *dailySuffix);
void log_close();
void log_write(log_type logType, const TCHAR *format, ...);
void log_debug(int level, const TCHAR *format, ...);
void log_set_debug_level(int level);
int log_get_debug_level();

/**
 *	�ַ������
 */
size_t utf8_to_mb(const char *src, int srcLen, char *dst, int dstLen);
size_t mb_to_utf8(const char *src, int srcLen, char *dst, int dstLen);

WCHAR *wstr_from_mb(const char *pszString);
WCHAR *wstr_from_utf8(const char *pszString);
char *mb_from_wstr(const WCHAR *pwszString);
char *utf8_from_wstr(const WCHAR *pwszString);

/**
 *	���ݿ����
 */


P2_NAMESPACE_END