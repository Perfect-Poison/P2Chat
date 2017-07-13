#pragma once
/*!
 * \file	DBConnection.h
 *
 * \author	BrianYi
 * \date	2017/07/11
 *
 * \brief	���ݿ������
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
    /// \brief ������ģʽ����ȡ�̳߳أ�������Ŀ����һ���̳߳�
    ///
    /// \date 2017/07/13
    ///
    /// \author BrianYi
    ///
    /// \retval ����һ��Ψһ���̳߳�
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

    /// \brief ���ص�ǰ�̳߳ص�������ʱ�䣬��ĳ�����ӳ�����ʱ��δʹ��ʱ�����Զ�����destroyed
    ///
    /// \date 2017/07/13
    ///
    /// \author BrianYi
    ///
    /// \retval ������ʱ��
    unsigned int max_idle_time() { return fMaxIdleTime; }

    /// \brief ����������ʱ�䣬��һ�����ӳ�����ʱ��δʹ��ʱ�����Զ�����destroyed
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