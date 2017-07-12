#include "Common/common.h"
#include "mysqlpp/mysql++.h"
using namespace mysqlpp;

int main()
{
    Connection conn(false);
    if (conn.connect("p2chatdb", "localhost", "root", "123123", 3306)) 
    {
        Query query = conn.query("select * from test_tab");
        StoreQueryResult res = query.store();
        if (res) 
        {
            for (size_t i = 0; i < res.num_fields(); i++) 
            {
                cout << res.field_name(i) << "\t";
            }
            cout << endl;
            for (size_t i = 0; i < res.num_rows(); i++)
            {
                cout << res[i]["f1"] << "\t" << res[i]["f2"] << endl;
            }
//             cout << "We have:" << endl;
//             StoreQueryResult::const_iterator it;
//             for (it = res.begin(); it != res.end(); it++) 
//             {
//                 Row row = *it;
//                 cout << '\t' << row[0] << endl;
//             }
        }
        else 
        {
            cerr << "Failed to get item list: " << query.error() << endl;
            return 1;
        }
        return 0;
    }
    else 
    {
        cerr << "DB connection failed: " << conn.error() << endl;
        return 1;
    }
    return 0;
}
