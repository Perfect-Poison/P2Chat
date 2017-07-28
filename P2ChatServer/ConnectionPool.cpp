#include "ConnectionPool.h"

P2_NAMESPACE_BEG




//     sql_create_9(group_info,
//         1, 9,
//         sql_varchar, name,
//         sql_varchar, icon,
//         sql_varchar, profile,
//         sql_int, owner_user_info_id,
//         sql_varchar, assistant_user_info_id_list,
//         sql_varchar, file_zone_list,
//         sql_varchar, setting_xml,
//         sql_datetime, created_date,
//         sql_int, encrypt_repo_id)


ConnectionPool::ConnectionPool()
{
    fMaxIdleTime = 1;
}

mysqlpp::Connection * ConnectionPool::create()
{
    return new mysqlpp::Connection;
}

void ConnectionPool::destroy(mysqlpp::Connection* conn)
{
    delete conn;
}

ConnectionPool::~ConnectionPool()
{
    clear();
}

void ConnectionPool::set_max_idle_time(unsigned int maxIdleTime)
{
    fMaxIdleTime = maxIdleTime;
}

P2_NAMESPACE_END