/* $Id$ */
#include "stdafx.h"

#include "ronet/packet_factory.h"

#define HANDLER(x) bool ronet::PacketFactory::Handle_ ##x (ronet::ucBuffer& b) { \
	pkt ##x *p; \
	p = new pkt ##x (); \
	if (!p->Decode(b)) { delete(p); return(false); } \
	push(p); \
	return(true); }

#define CALLER(x) m_dispatcher.Register(pkt ##x ##ID, &ronet::PacketFactory::Handle_ ##x )
// List of packets that we know how to decode

HANDLER(ServerList)
HANDLER(CharList)
HANDLER(CharCreateError)

ronet::PacketFactory::PacketFactory() : m_dispatcher(this) {
	CALLER(ServerList);
	CALLER(CharList);
	CALLER(CharCreateError);
	//m_dispatcher.Register(0x0069, &ronet::PacketFactory::Handle_ServerList);
}

ronet::PacketFactory::~PacketFactory() {
	Packet* p = pop();
	while (p != NULL) {
		delete(p);
		p = pop();
	}
}

/*
bool ronet::PacketFactory::Handle_ServerList(ronet::ucBuffer& b) {
	pktServerList *p;
	p = new pktServerList();
	if (!p->Decode(b)) {
		delete(p);
		return(false);
	}

	push(p);
	return(true);
}
*/

void ronet::PacketFactory::push(ronet::Packet* p) {
	packets.push_back(p);
}

ronet::Packet* ronet::PacketFactory::pop() {
	std::vector<Packet*>::iterator itr;
	itr = packets.begin();
	if (itr == packets.end())
		return(NULL);
	Packet* ret = *itr;
	packets.erase(itr);
	return(ret);
}

unsigned int ronet::PacketFactory::count() const {
	return(packets.size());
}

void ronet::PacketFactory::generatePackets(ucBuffer& buffer) {
	unsigned short id;
	while (buffer.dataSize() > 0) {
		buffer.peek((unsigned char*)&id, sizeof(short));
		if(!m_dispatcher.Call(id, buffer))
			return;
	}
}

ronet::PacketFactory& ronet::PacketFactory::operator << (ucBuffer& buffer) {
	generatePackets(buffer);
	return(*this);
}

ronet::PacketFactory& ronet::PacketFactory::operator << (Connection& c) {
	generatePackets(c.bufInput);
	return(*this);
}

ronet::PacketFactory::Dispatcher::Dispatcher(ronet::PacketFactory* f) {
	m_factory = f;
}

void ronet::PacketFactory::Dispatcher::Register(unsigned short id, ronet::PacketFactory::Handler call) {
	m_calls[id] = call;
}

bool ronet::PacketFactory::Dispatcher::Call(unsigned short method, ucBuffer& buf) {
	std::map<unsigned short, Handler>::iterator itr;
	itr = m_calls.find(method);
	if (itr == m_calls.end()) {
		fprintf(stderr, "Undefined call to method %04x\n", method);
		return(false);
	}
	
	Handler c = itr->second;
	return((m_factory->*c)(buf)); 
}