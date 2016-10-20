


#ifndef serverBin_h__
#define serverBin_h__

#include "server.h"

#include <vector>
#include <set>
#include <map>
#include <stdint.h>

typedef std::set<uint16_t> XMSGSET; //订阅的消息

typedef std::map<XSOCKET, XMSGSET*> XSUBMAP;
extern std::map<XSOCKET, XMSGSET*> subscriber;

void xmsg_set(XSOCKET, uint16_t); //订阅消息
void xmsg_clr(XSOCKET, uint16_t); //取消订阅

bool xmsg_isset(uint16_t, XMSGSET*);
void xmsg_clset(uint16_t, XMSGSET*);
void xmsg_adset(uint16_t, XMSGSET*);

void subscriber_remove(XSOCKET s); //删除订阅者
void subscriber_add(XSOCKET s); //增加订阅者
void subscriber_clear();

//派发消息
void xmsg_dispatch(XSOCKET s, const void* _buf, int _size);

//命令分析
void xmsg_command(XSOCKET s, const char* _buf, int _size);

#endif // serverBin_h__



