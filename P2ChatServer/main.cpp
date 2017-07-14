// #include "Common/common.h"
// #include "mysqlpp/mysql++.h"
// #include "ConnectionPool.h"
// using namespace mysqlpp;
// 
// 
// int main()
// {
//     Connection conn(false);
//     if (conn.connect("p2chatdb", "localhost", "root", "123123", 3306)) 
//     {
//         Query query = conn.query();
//         p2::encrypt_repo a("RSA", 1, sql_blob("abc"), sql_blob("abcd"));
// //         a.algorithm_name = "RSA";
// //         a.is_key_pair = true;
//         //query << "insert into encrypt_repo ('algorithm_name','is_key_pair','public_key','private_key') values ('" + a.algorithm_name + "',1,'abc','abcd')";
//         query.insert(a);
//         cout << "Query: " << query << endl;
//         query.exec();
// //         if (res) 
// //         {
// //             for (size_t i = 0; i < res.num_fields(); i++) 
// //             {
// //                 cout << res.field_name(i) << "\t";
// //             }
// //             cout << endl;
// //             for (size_t i = 0; i < res.num_rows(); i++)
// //             {
// //                 cout << res[i]["f1"] << "\t" << res[i]["f2"] << endl;
// //             }
// //             cout << "We have:" << endl;
// //             StoreQueryResult::const_iterator it;
// //             for (it = res.begin(); it != res.end(); it++) 
// //             {
// //                 Row row = *it;
// //                 cout << '\t' << row[0] << endl;
// //             }
// //         }
// //         else 
// //         {
// //             cerr << "Failed to get item list: " << query.error() << endl;
// //             return 1;
// //         }
//         return 0;
//     }
//     else 
//     {
//         cerr << "DB connection failed: " << conn.error() << endl;
//         return 1;
//     }
//     return 0;
// }



#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>

#ifndef WIN32
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#include <sys/socket.h>
#endif

#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "event2/listener.h"
#include "event2/util.h"
#include "event2/event.h"

#include <WinSock2.h>


static const char MESSAGE[] = "Hello, World!\n";

static const int PORT = 9995;

static void conn_writecb(struct bufferevent *bev, void *user_data)
{
    struct evbuffer *output = bufferevent_get_output(bev);
    if (evbuffer_get_length(output) == 0)
    {
        printf("flushed answer\n");
        bufferevent_free(bev);
    }
}

static void conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{
    if (events & BEV_EVENT_EOF)
    {
        printf("Connection closed.\n");
    }
    else if (events & BEV_EVENT_ERROR)
    {
        printf("Got an error on the connection: %s\n",
            strerror(errno));/*XXX win32*/
    }
    /* None of the other events can happen here, since we haven't enabled
    * timeouts */
    bufferevent_free(bev);
}

static void signal_cb(evutil_socket_t sig, short events, void *user_data)
{
    struct event_base *base = (struct event_base *)user_data;
    struct timeval delay = { 2, 0 };

    printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");

    event_base_loopexit(base, &delay);
}

static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
    struct sockaddr *sa, int socklen, void *user_data)
{
    struct event_base *base = (struct event_base *)user_data;
    struct bufferevent *bev;

    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    if (!bev)
    {
        fprintf(stderr, "Error constructing bufferevent!");
        event_base_loopbreak(base);
        return;
    }
    bufferevent_setcb(bev, NULL, conn_writecb, conn_eventcb, NULL);
    bufferevent_enable(bev, EV_WRITE);
    bufferevent_disable(bev, EV_READ);

    bufferevent_write(bev, MESSAGE, strlen(MESSAGE));
}

void cb(int sock, short what, void *arg)
{
    struct event_base *base = (event_base*)arg;
    event_base_loopbreak(base);
}

void main_loop(struct event_base *base, evutil_socket_t watchdog_fd)
{
    struct event *watchdog_event;

    watchdog_event = event_new(base, watchdog_fd, EV_READ, cb, base);

    event_add(watchdog_event, NULL);

    event_base_dispatch(base);
}

int main(int argc, char **argv)
{
    struct event_base *base;
    struct evconnlistener *listener;
    struct event *signal_event;

    struct sockaddr_in sin;
#ifdef WIN32
    WSADATA wsa_data;
    WSAStartup(0x0201, &wsa_data);
#endif

    base = event_base_new();
    if (!base)
    {
        fprintf(stderr, "Could not initialize libevent!\n");
        return 1;
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORT);

    listener = evconnlistener_new_bind(base, listener_cb, (void *)base,
        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
        (struct sockaddr*)&sin,
        sizeof(sin));

    if (!listener)
    {
        fprintf(stderr, "Could not create a listener!\n");
        return 1;
    }

    signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);

    if (!signal_event || event_add(signal_event, NULL)<0)
    {
        fprintf(stderr, "Could not create/add a signal event!\n");
        return 1;
    }

    event_base_dispatch(base);

    evconnlistener_free(listener);
    event_free(signal_event);
    event_base_free(base);

    printf("done\n");
    return 0;
}
