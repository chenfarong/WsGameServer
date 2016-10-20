
#include "mcs.h"

std::map<XSOCKET, XMSGSET*> subscriber;

void xmsg_set(XSOCKET s, uint16_t _mid)
{
	XSUBMAP::iterator it = subscriber.find(s);
	if (it != subscriber.end()) {
		xmsg_adset(_mid, it->second);
	}
	else {
		XMSGSET* _set= new XMSGSET();
		subscriber[s] = _set;
		xmsg_adset(_mid, _set);
	}
}


void xmsg_clr(XSOCKET s, uint16_t _mid)
{
	XSUBMAP::iterator it = subscriber.find(s);
	if (it!=subscriber.end()) {
		xmsg_clset(_mid, it->second);
	}
}

bool xmsg_isset(uint16_t _mid, XMSGSET* _set)
{
	if (_set == NULL) return false;
	return (*_set).find(_mid) != _set->end();
}


void xmsg_adset(uint16_t _mid, XMSGSET* _set)
{
	if (_set == NULL) return;
	_set->insert(_mid);
}

void xmsg_clset(uint16_t _mid, XMSGSET* _set)
{
	if (_set == NULL) return;
	_set->erase(_mid);
}

void subscriber_remove(XSOCKET s)
{
	XSUBMAP::iterator it = subscriber.find(s);
	if (it != subscriber.end()) {
		if(it->second) delete it->second;
		subscriber.erase(it);
	}
}

void subscriber_add(XSOCKET s)
{
	XSUBMAP::iterator it = subscriber.find(s);
	if (it == subscriber.end()) {
		XMSGSET* _set = new XMSGSET();
		subscriber[s] = _set;
	}
}

void subscriber_clear()
{
	while (!subscriber.empty()) {
		XSUBMAP::iterator it = subscriber.begin();
		subscriber_remove(it->first);
		subscriber.erase(it);
	}
}

void xmsg_dispatch(XSOCKET s,const void * _buf, int _size)
{
	uint16_t* mid = (uint16_t*)_buf;
	for (XSUBMAP::iterator it=subscriber.begin();
		it!=subscriber.end();it++)
	{
		if(it->first==s) continue;

		//广播给除自己外的所有连接者
		if (*mid == 0)
		{
			send(it->first, (const char*)_buf, _size, 0);
			continue;
		}

		if (xmsg_isset(*mid, it->second)) {
			send(it->first, (const char*)_buf, _size, 0);
		}
	}
}

void xmsg_command(XSOCKET s,const char* _buf, int _size)
{
	int16_t* _cmd = (int16_t*)_buf;
	if (*_cmd == -1) {
		_cmd++;
		xmsg_set(s, *_cmd);
		return;
	}

	if (*_cmd == -2) {
		_cmd++;
		xmsg_clr(s, *_cmd);
		return;
	}

	xmsg_dispatch(s, _buf, _size);

}
