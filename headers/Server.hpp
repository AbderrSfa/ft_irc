/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbari <mbari@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/08 23:32:10 by mbari             #+#    #+#             */
/*   Updated: 2022/05/12 12:46:36 by mbari            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SERVER_HPP
#define __SERVER_HPP

/*  This class is temporarily		*/
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <cstring>
#include <algorithm>
#include <utility>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>

class Request
{
	public:
		std::vector<std::string>	args;
		std::string					command;
		bool						invalidMessage;

	Request() : args(), command(), invalidMessage(false) {}
};

class Client;
class Channel;

class Server
{
	private:
		std::string							_name;
		int									_socketfd;
		// Client								*_clients;				// switch to vector or map
		std::map<int, Client *>				_clients;
		struct pollfd						*_pfds;					// find a way to reallocate without using realloc() (realloc is allowed)
		int									_online_c;
		int									_max_online_c;
		std::string							_prefix;
		std::map<std::string, Channel *>	_allChannels;
		std::vector<std::string>			_unavailableUserName;
		std::vector<std::string>			_clientNicknames;

	private:
		Server();
		std::string						_welcomemsg(void);
		void							_getSocket(std::string Port);
		void							_addToPoll(int newfd);
		void							_removeFromPoll(int i);
		void							_newClient(void);
		void							_ClientRequest(int i);
		int								_sendall(int destfd, std::string message);
		void							_broadcastmsg(int sender_fd, std::string buf, int nbytes);
		std::string						_setUserName(Request request, int i);
		std::string						_setNickName(Request request, int i);
		std::string						_setFullName(Request request, int i);
		std::string						_setPassWord(Request request, int i);
		std::string						_setOper(Request request, int i);
		std::string						_setMode(Request request, int i);
		std::string						_joinChannel(Request request, int i);
		bool							_validMode(Request request);
		std::string						_quit(Request request, int i);
		std::string						_part(Request request, int i);
		std::string						_topic(Request request, int i);
		std::string						_kick(Request request, int i);
		std::string						_sendMessage(std::string message, int i);
		std::string						_printError(int num, std::string reply, std::string message);
		// std::string						_setUsername( std::string username, int i );
		std::string						_parsing(std::string message, int i);
		Request							_splitRequest(std::string req);
		std::string						_printUserInfo(int i);
		std::string						_printHelpInfo(int i);
		std::string						_printReply(int num, std::string reply, std::string message);
		std::string						_printUserModes(std::string ret, int i);
		std::vector<std::string>		_commaSeparator(std::string arg);
		int								_createPrvChannel( std::string ChannelName, std::string ChannelKey, int	 CreatorFd );
		int								_createChannel( std::string channelName, int CreatorFd );
		int								_partChannel( std::string ChannelName, int i );
		std::string						_DeezNuts( Request request, int i );
		std::string						_getBotMessage();

	// private:
	// 	class ArgsError: public std::exception
	// 	{
	// 		virtual const char* what() const throw();
	// 	};
	public:
		Server(std::string Name, int max_online, std::string Port);
		Server(const Server & x);
		~Server();
		Server & operator=( const Server & rhs );

	public:
		void	startServer(void);
};

#include "Channel.hpp"
#include "Client.hpp"

#endif

