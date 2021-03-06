/* $Id$ */
/*
    ------------------------------------------------------------------------------------
    LICENSE:
    ------------------------------------------------------------------------------------
    This file is part of The Open Ragnarok Project
    Copyright 2007 - 2009 The Open Ragnarok Team
    For the latest information visit http://www.open-ragnarok.org
    ------------------------------------------------------------------------------------
    This program is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any later
    version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
    this program; if not, write to the Free Software Foundation, Inc., 59 Temple
    Place - Suite 330, Boston, MA 02111-1307, USA, or go to
    http://www.gnu.org/copyleft/lesser.txt.
    ------------------------------------------------------------------------------------
*/
#include "stdafx.h"

#include "ronet/tcp_connection.h"

#ifndef MSG_DONTWAIT // This is for windows.
#	define MSG_DONTWAIT 0
#endif

#define RECBUFSIZE (1024*5)

ronet::TcpConnection::TcpConnection() {
	m_state = Ready;
	m_socket = -1;
	m_direction = Outgoing;
	m_protocol = TCP;
}

ronet::TcpConnection::~TcpConnection() {
}

bool ronet::TcpConnection::isConnected() const {
	return(m_state == Connected);
}


bool ronet::TcpConnection::Connect(const char* hostname, const unsigned int port) {
	if (m_socket != -1)
		return(false);
	unsigned long ip = inet_addr(hostname);
	return(Connect(ip, port));
}

bool ronet::TcpConnection::Connect(const unsigned long ip, const unsigned int port) {
	if (m_socket != -1)
		return(false);

	m_direction = Outgoing;

    struct sockaddr_in server_sin;
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket < 0) {
    	m_state = Ready;
    	return(false);
    }

	server_sin.sin_family = AF_INET;
	server_sin.sin_addr.s_addr = ip;
	server_sin.sin_port = htons(port);

	int ret;
#ifdef WIN32
	ret = connect(m_socket, (PSOCKADDR) &server_sin, sizeof(server_sin));
#else
	ret = connect(m_socket, (struct sockaddr *) &server_sin, sizeof(server_sin));
#endif

	if (ret) {
		// Something went wrong.
#ifdef WIN32
		closesocket(m_socket);
#else
		close(m_socket);
#endif
		m_state = Ready;
		m_socket = 0;
		return(false);
	}

	// set nonblocking socket
#ifdef WIN32
	unsigned long nonblocking = 1;
	ioctlsocket(m_socket, FIONBIO, &nonblocking);
#else
	fcntl(m_socket, F_SETFL, O_NONBLOCK);
#endif

	m_ip = ip;
	m_port = port;
	m_state = Connected;

	return(true);
}

bool ronet::TcpConnection::Process() {
	if (!isConnected())
		return(false);
	bool ret;

	ret = SendData();
	if (ret == false)
		return(false);
	ret = RecvData();
	return(ret);
}

bool ronet::TcpConnection::SendData() {
	if (m_socket == -1) {
		_log(RONET__ERROR, "Trying to write data on a not connected socket!");
		return(false);
	}

	if (bufOutput.dataSize() == 0) {
		// Nothing to send.
		return(true);
	}

	int datawritten;
#ifdef WIN32
	datawritten = send(m_socket, (const char*) bufOutput.getBuffer(), bufOutput.dataSize(), 0);
#else
	datawritten = send(m_socket, bufOutput.getBuffer(), bufOutput.dataSize(), MSG_NOSIGNAL);
	if(errno==EPIPE) datawritten = -1; // TODO: Check this in Linux
#endif	

	if (datawritten < 0) {
		_log(RONET__ERROR, "Writing data to socket");
		return (0);
	}
	if (datawritten != 0) {
		_log(RONET__TRACE, "send %d bytes of data", datawritten);
		_hexlog(RONET__TRACE, (const unsigned char*)bufOutput.getBuffer(), datawritten);

		bufOutput.ignore(datawritten);
	}

	return(true);
}

bool ronet::TcpConnection::RecvData() {
	if (m_socket == -1) {
		_log(RONET__ERROR, "Trying to read data on a not connected socket!");
		return(false);
	}
	int bufsize;
	unsigned char recbuf[RECBUFSIZE];

	bufsize = recv(m_socket, (char*)recbuf, RECBUFSIZE, MSG_DONTWAIT);
	if (bufsize > 0) {
		_log(RONET__TRACE, "received %d bytes of data", bufsize);
		_hexlog(RONET__TRACE, recbuf, bufsize);

		bufInput.write(recbuf, bufsize);
	}
	else if (bufsize == 0) {
		return(false);  
	}
	else if 
#ifdef WIN32
	(WSAGetLastError() != WSAEWOULDBLOCK)
#else
	(errno != EAGAIN)
#endif
	{
		_log(RONET__ERROR, "WSAGetLastError() != WSAEWOULDBLOCK");
		return false;
	}
	
	return (true);
}


bool ronet::TcpConnection::Close() {
	if (m_socket == -1)
		return(false);

	m_state = Disconnected;

#ifdef WIN32
	closesocket(m_socket);
#else
	close(m_socket);
#endif

	m_socket = -1;
	return(true);
}
