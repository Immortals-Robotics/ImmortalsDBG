#include "udp_client.h"

UdpClient::UdpClient(std::string_view ip, uint16_t port)
{
	m_context = std::make_unique<asio::io_context>();
	m_socket = std::make_unique<asio::ip::udp::socket>(*m_context);

	m_address = asio::ip::make_address(ip);
	m_listen_endpoint = asio::ip::udp::endpoint{asio::ip::udp::v4(), port};

	m_socket->open(m_listen_endpoint.protocol());
	m_socket->set_option(asio::ip::udp::socket::reuse_address(true));
	m_socket->bind(m_listen_endpoint);

	if (m_address.is_multicast())
	{
		m_socket->set_option(asio::ip::multicast::join_group(m_address));
	}
}

bool UdpClient::receive(google::protobuf::MessageLite *const t_message)
{
	const size_t received_size = m_socket->receive_from(asio::buffer(m_buffer), m_last_receive_endpoint);

	if (received_size > 0)
	{
		return t_message->ParseFromArray(m_buffer.data(), received_size);
	}

	return false;
}

std::span<char> UdpClient::receiveRaw()
{
	const size_t received_size = m_socket->receive_from(asio::buffer(m_buffer), m_last_receive_endpoint);

	if (received_size > 0)
	{
		return std::span<char>(m_buffer.data(), received_size);
	}

	return {};
}
