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
using namespace mysqlpp;

P2_NAMESPACE_BEG



// class DBConnection
// {
// public:
//     static shared_ptr<DBConnection> GetConnection()
//     {
//         if (fDBConnInstance == nullptr)
//             fDBConnInstance = shared_ptr<DBConnection>(new DBConnection);
//         return fDBConnInstance;
//     }
//     ~DBConnection();
// 
// private:
//     DBConnection();
//     static shared_ptr<DBConnection> fDBConnInstance;
// private:
//     mutex fMutex;
//     otl_connect fDBConn;
// };

P2_NAMESPACE_END