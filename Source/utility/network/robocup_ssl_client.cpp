//========================================================================
//  This software is free: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License Version 3,
//  as published by the Free Software Foundation.
//
//  This software is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  Version 3 in the file COPYING that came with this distribution.
//  If not, see <http://www.gnu.org/licenses/>.
//========================================================================
/*!
  \file    robocup_ssl_client.cpp
  \brief   C++ Implementation: robocup_ssl_client
  \author  Stefan Zickler, 2009
*/
//========================================================================
#include "robocup_ssl_client.h"

#include "udp_client.h"

RoboCupSSLClient::RoboCupSSLClient(int port,
	string net_address,
	string net_interface)
{
	_port = port;
	_net_address = net_address;
	_net_interface = net_interface;
	in_buffer = new char[65536];
}


RoboCupSSLClient::~RoboCupSSLClient()
{
	delete[] in_buffer;
}

void RoboCupSSLClient::close() {
	udp.reset();
}

bool RoboCupSSLClient::open(bool blocking) {
	close();

	udp = std::make_unique<UdpClient>(_net_address, _port);

	if (!udp->isConnected()) {
		fprintf(stderr, "Unable to open UDP network port: %d\n", _port);
		fflush(stderr);
		return(false);
	}

	return(true);
}

bool RoboCupSSLClient::receive(SSL_WrapperPacket & packet) {
	const bool result = udp->receive(&packet);
	if (result) {
		fflush(stdout);
	}
	return result;
}
