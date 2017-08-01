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
char *bin_to_str(const BYTE* pData, size_t size, char *pStr, bool hasSeparator = true);
size_t str_to_bin(const char* pStr, BYTE *pData, size_t size);


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
char* FormatCalendarTime(char *buffer);
char* FormatLogCalendarTime(char *buffer);


/**
 *	��־���
 */
void log_write(log_type logType, const char *format, ...);
void log_debug(const char *format, ...);

P2_NAMESPACE_END