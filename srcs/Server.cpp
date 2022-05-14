/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbari <mbari@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/08 23:36:07 by mbari             #+#    #+#             */
/*   Updated: 2022/05/14 12:06:15 by mbari            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Server.hpp"


Server::Server() : _name(), _unavailableUserName(), _socketfd(0), _pfds(nullptr), _online_c(0), _max_online_c(0), _prefix(":"), _clientNicknames(), _allChannels() {};

Server::Server(std::string Name, int max_online, std::string Port): _clients()
{
	this->_name = Name;
	this->_max_online_c = max_online + 1;
	this->_online_c = 0;
	this->_pfds = new struct pollfd[max_online];
	// this->_clients = new Client[max_online];
	_getSocket(Port);
	this->_pfds[0].fd = this->_socketfd;
	this->_pfds[0].events = POLLIN;
	this->_online_c++;
};

Server::~Server() {}

std::string	Server::_printError(int num, std::string reply, std::string message)
{
	// std::string ret = num + " " + reply + "\n\t" + message + "\n";
	return (std::to_string(num) + " " + reply + "\t" + message + "\n");
	// std::cout << num << " " << reply << "\n\t" << message << std::endl;
}

void	Server::_newClient(void)
{
	/* all those varibles will be deleted when adding client class */
	struct sockaddr_storage	remotaddr;
	socklen_t				addrlen;
	int newfd;
	/* *********************************************************** */

	addrlen = sizeof remotaddr;
	newfd = accept(this->_socketfd, (struct sockaddr*)&remotaddr, &addrlen);
	if (newfd == -1)
		std::cout << "accept() error: " << strerror(errno) << std::endl;
	else
	{
		_addToPoll(newfd);
		std::string welcome = _welcomemsg();
		if (send(newfd, welcome.c_str(), welcome.length(), 0) == -1)
			std::cout << "send() error: " << strerror(errno) << std::endl;
		std::cout << "server: new connection from "
			<< inet_ntoa(((struct sockaddr_in*)&remotaddr)->sin_addr)
			<< " on socket " << newfd << std::endl;
	}
};

void Server::startServer(void)
{
	struct sockaddr_storage	remotaddr;
	socklen_t				addrlen;
	int newfd;
	char buf[7777];

	while (77)
	{
		int poll_count = poll(this->_pfds, this->_online_c, -1);
		if (poll_count == -1)
		{
			std::cout << "poll() error: " << strerror(errno) << std::endl;
			exit(-1);
		}

		for (int i = 0; i < this->_online_c; i++)
		{
			if (this->_pfds[i].revents & POLLIN)
			{
				if (this->_pfds[i].fd == this->_socketfd)
					_newClient();			// If listener is ready to read, handle new connection
				else
					_ClientRequest(i);		// If not the listener, we're just a regular client
			}
		}
	}
};
