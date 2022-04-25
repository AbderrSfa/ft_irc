/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbari <mbari@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 22:30:33 by mbari             #+#    #+#             */
/*   Updated: 2022/04/25 13:46:27 by mbari            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Channel.hpp"

Channel::Channel(): _operators(), _members(), _voice(), _name(), _prefix(), _onlineUsers(), _key(), _topic() {};
Channel::Channel( const Channel& x ) { *this = x; };
Channel::Channel( std::string channelName, Client *Creator): _name(channelName), _onlineUsers(1), _topic(), _prefix(), _operators(), _members(), _voice()
{
	this->_operators.insert(std::pair<int, Client *>(Creator->getClientfd(), Creator));
	// this->_operators.insert(std::pair<int, Client *>(Creator->getClientfd(), Creator));
	// Creator->joinChannel(channelName, this);
	// std::cout << Creator.JoinedChannels() << std::endl;
};
Channel::Channel( std::string channelName, std::string channelKey, Client *Creator ): _name(channelName), _key(channelKey), _onlineUsers(1), _topic(), _prefix(), _operators(), _members(), _voice()
{
	this->_operators.insert(std::pair<int, Client *>(Creator->getClientfd(), Creator));
	// Creator->joinChannel(channelName, this);
};
Channel::~Channel() {};

Channel& Channel::operator=( const Channel& rhs )
{
	if (this == &rhs)
		return (*this);
	this->_prefix = rhs._prefix;
	this->_onlineUsers = rhs._onlineUsers;
	this->_name = rhs._name;
	this->_members.insert(rhs._members.begin(), rhs._members.end());
	this->_operators.insert(rhs._operators.begin(), rhs._operators.end());
	this->_voice.insert(rhs._voice.begin(), rhs._voice.end());
	return (*this);
};



std::string	Channel::getName() const { return (this->_name); };
Client*	Channel::getOperators( int i ) const { return (this->_operators.at(i)); };
