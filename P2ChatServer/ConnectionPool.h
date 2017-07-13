#pragma once
/*!
 * \file	DBConnection.h
 *
 * \author	BrianYi
 * \date	2017/07/11
 *
 * \brief	数据库访问类
 */

#include "Common/common.h"
#include "mysqlpp/mysql++.h"
#include "mysqlpp/ssqls.h"
#include <mutex>


P2_NAMESPACE_BEG

#ifndef _SS
#define _SS
sql_create_4(encrypt_repo,
    1, 4,
    sql_varchar, algorithm_name,
    sql_tinyint, is_key_pair,
    sql_blob, public_key,
    sql_blob, private_key)
#endif

class ConnectionPool : public mysqlpp::ConnectionPool
{
public:
    /// \brief （单例模式）获取线程池，整个项目共用一个线程池
    ///
    /// \date 2017/07/13
    ///
    /// \author BrianYi
    ///
    /// \retval 返回一个唯一的线程池
    static ConnectionPool *GetConnectionPool()
    {
        lock_guard<mutex> locker(fMutex);
        if (fConnPoolInstance == nullptr)
        {
            fConnPoolInstance = new ConnectionPool;
        }
        return fConnPoolInstance;
    }

    ~ConnectionPool();

    /// \brief 返回当前线程池的最大空闲时间，当某个连接超过该时间未使用时，则自动进行destroyed
    ///
    /// \date 2017/07/13
    ///
    /// \author BrianYi
    ///
    /// \retval 最大空闲时间
    unsigned int max_idle_time() { return fMaxIdleTime; }

    /// \brief 设置最大空闲时间，当一个连接超过该时间未使用时，则自动进行destroyed
    ///
    /// \date 2017/07/13
    ///
    /// \author BrianYi
    void set_max_idle_time(unsigned int maxIdleTime);
private:
    ConnectionPool();
    mysqlpp::Connection *create();
    void destroy(mysqlpp::Connection*);
private:
    static ConnectionPool *fConnPoolInstance;
    static mutex fMutex;
    unsigned int fMaxIdleTime;
};

P2_NAMESPACE_END