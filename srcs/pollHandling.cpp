/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pollHandling.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbari <mbari@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/18 23:54:07 by mbari             #+#    #+#             */
/*   Updated: 2022/04/18 23:54:45 by mbari            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Server.hpp"

void	Server::_addToPoll(int newfd)
{
	if (this->_online_c == this->_max_online_c)
	{
		this->_max_online_c *= 2;
		this->_pfds = (struct pollfd *)realloc(this->_pfds, this->_max_online_c);
		this->_clients = (Client *)realloc(this->_clients, this->_max_online_c);
	}
	this->_pfds[this->_online_c].fd = newfd;
	this->_pfds[this->_online_c].events = POLLIN;
	this->_clients[this->_online_c].setClientfd(newfd);

	this->_online_c++;
};

void	Server::_removeFromPoll(int i)
{
	close(this->_pfds[i].fd);
	this->_pfds[i] = this->_pfds[this->_online_c - 1];
	// this->_clients[i] = nullptr;

	this->_online_c--;
};
