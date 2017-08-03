#pragma once
/*!
 * \file 	p2_util.h
 *
 * \author 	BrianYi
 * \date 	2017/07/30
 *
 * \brief	此文件为整个项目需要用到的常用工具函数
 */
#include "p2_common.h"

P2_NAMESPACE_BEG

/**
 *	字符串操作相关函数
 */
char *bin_to_str(const BYTE* pData, size_t size, char *pStr, bool hasSeparator = true);
size_t str_to_bin(const char* pStr, BYTE *pData, size_t size);


/**
 *	内存操作相关函数
 */
void *memdup(const void *data, size_t size);

/**
 *	原子操作相关函数
 */
unsigned int atomic_add(unsigned int *area, int val);
unsigned int atomic_or(unsigned int *area, unsigned int val);
unsigned int atomic_sub(unsigned int *area, int val);

unsigned int compare_and_store(unsigned int oval, unsigned int nval, unsigned int *area);

/**
 *	事件操作相关函数
 */
int select_watchevent(struct eventreq *req, int which);
int select_modwatch(struct eventreq *req, int which);
int select_waitevent(struct eventreq *req);

/**
 *	时间相关
 */
int64 GetCurrentTimeMicroS();   // 微妙级
int64 GetCurrentTimeMilliS();   // 毫秒级
int64 GetCurrentTimeS();        // 秒级
char* FormatCalendarTime(char *buffer);
char* FormatLogCalendarTime(char *buffer);


/**
 *	日志相关
 *  debug level: 0~9
 *  0~4 可以忽略的
 *  5~6 一般错误
 *  7~9 严重错误会导致系统运行不正常或崩溃
 */
bool log_open(const char *logName, log_flags flags, log_rotation_policy rotationPolicy, int maxLogSize, int historySize, const char *dailySuffix);
void log_close();
void log_write(log_type logType, const char *format, ...);
void log_debug(int level, const char *format, ...);
void log_set_debug_level(int level);
int log_get_debug_level();
P2_NAMESPACE_END