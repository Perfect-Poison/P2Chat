#include "Common/common.h"
#include "mysqlpp/mysql++.h"
#include "DBConnection.h"
using namespace mysqlpp;

sql_create_4(encrypt_repo,
    1, 4,
    sql_varchar, algorithm_name,
    sql_tinyint, is_key_pair,
    sql_blob_null, public_key,
    sql_blob_null, private_key)

int main()
{
    Connection conn(false);
    if (conn.connect("p2chatdb", "localhost", "root", "123123", 3306)) 
    {
        Query query = conn.query();
        encrypt_repo a("RSA", 1, null, null);
//         a.algorithm_name = "RSA";
//         a.is_key_pair = true;
        query << "insert into encrypt_repo (%0:field1,%1:field2) values (%2:field3,%3:field4)";
        query.parse();
        query.store("algorithm_name", "is_key_pair", "RSA", sql_tinyint_unsigned(1));
        query.exec();
        cout << "Query: " << query << endl;
//         if (res) 
//         {
//             for (size_t i = 0; i < res.num_fields(); i++) 
//             {
//                 cout << res.field_name(i) << "\t";
//             }
//             cout << endl;
//             for (size_t i = 0; i < res.num_rows(); i++)
//             {
//                 cout << res[i]["f1"] << "\t" << res[i]["f2"] << endl;
//             }
//             cout << "We have:" << endl;
//             StoreQueryResult::const_iterator it;
//             for (it = res.begin(); it != res.end(); it++) 
//             {
//                 Row row = *it;
//                 cout << '\t' << row[0] << endl;
//             }
//         }
//         else 
//         {
//             cerr << "Failed to get item list: " << query.error() << endl;
//             return 1;
//         }
        return 0;
    }
    else 
    {
        cerr << "DB connection failed: " << conn.error() << endl;
        return 1;
    }
    return 0;
}
