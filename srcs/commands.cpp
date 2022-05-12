/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbari <mbari@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/18 23:46:52 by mbari             #+#    #+#             */
/*   Updated: 2022/05/12 17:15:24 by mbari            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Server.hpp"


std::string	Server::_parsing(std::string message, int i)
{
	Request	request(_splitRequest(message));

	if (request.invalidMessage)
		return ("Invalid message!\n");
	if (request.command == "PASS")
		return (_setPassWord(request, i));
	else if (request.command == "NICK")
		return (_setNickName(request, i));
	else if (request.command == "USER")
		return (_setUserName(request, i));
	else if (request.command == "OPER")
		return (_setOper(request, i));
	else if (request.command == "MODE")
		return (_setMode(request, i));
	else if (request.command == "PRIVMSG")
		return ("PRIVMSG command\n");
	else if (request.command == "HELP")
		return (_printHelpInfo(i));
	else if (request.command == "JOIN")
		return (_joinChannel(request, i));
	else if (request.command == "TOPIC")
		return (_topic(request, i));
	else if (request.command == "KICK")
		return (_kick(request, i));
	else if (request.command == "PART")
		return (_part(request, i));
	else if (request.command == "QUIT")
		return (_quit(request, i));
	else if (request.command == "INFO")
		return (_printUserInfo(i));
	else if (request.command == "DEEZNUTS")
		return (_DeezNuts( request, i));
	else
		return ("Invalid command\n");
};

std::string	Server::_kick(Request request, int i)
{
	if (!this->_clients[i]->getRegistered())
		return (_printError(451, "ERR_NOTREGISTERED", ":You have not registered"));
	if (request.args.size() < 2)
		return (_printError(461, " ERR_NEEDMOREPARAMS", " :Not enough parameters"));
	if (0 /* No such channel */)
		return (_printError(403, " ERR_NOSUCHCHANNEL", " <channel name> :No such channel"));
	if (!this->_clients[i]->getisOperator())
		return (_printError(482, " ERR_CHANOPRIVSNEEDED", request.args[0] + " :You're not channel operator"));
	if (0 /* User is not in channel */)
		return (_printError(441, " ERR_USERNOTINCHANNEL", "<nick> <channel> :They aren't on that channel"));
	if (0 /* Not in channel */)
		return (_printError(442, " ERR_NOTONCHANNEL", request.args[0] + " :You're not on that channel"));
	return ("");
}


std::string	Server::_topic(Request request, int i)
{
	if (!this->_clients[i]->getRegistered())
		return (_printError(451, "ERR_NOTREGISTERED", ":You have not registered"));
	if (request.args.size() == 0)
		return (_printError(461, " ERR_NEEDMOREPARAMS", " :Not enough parameters"));
	if (0 /* Not in channel */)
		return (_printError(442, " ERR_NOTONCHANNEL", request.args[0] + " :You're not on that channel"));
	if (request.args.size() == 1)
	{
		if (this->_allChannels.find(request.args[0])->second->getTopic() == "")
			return (_printReply(331, "RPL_NOTOPIC", request.args[0] + " :No topic is set"));
		else
			return (_printReply(332, "RPL_TOPIC", request.args[0] + " :" + this->_allChannels.find(request.args[0])->second->getTopic()));
	}
	if (!this->_clients[i]->getisOperator())
		return (_printError(482, " ERR_CHANOPRIVSNEEDED", request.args[0] + " :You're not channel operator"));
	this->_allChannels.find(request.args[0])->second->setTopic(request.args[1]);
	return ("");
}

int	Server::_partChannel( std::string ChannelName, int i )
{
	std::map<std::string, Channel *>::iterator itCh= this->_allChannels.find(ChannelName);
	if (itCh == this->_allChannels.end() /* No such channel */)
		return (NOSUCHCHANNEL);
	else
	{
		std::pair<Client *, int> user(itCh->second->findUserRole(i));
		if (user.second == -1 /* Not in channel */)
			return (NOTINCHANNEL);
		else
		{
			if (user.second == 0)
				itCh->second->removeMember(i);
			else if (user.second == 1)
				itCh->second->removeOperator(i);
			else
				itCh->second->removeVoice(i);
			user.first->leaveChannel(itCh->second->getName());
		}
	}
	return (0);
}

std::string	Server::_part( Request request, int i )
{
	if (!this->_clients[i]->getRegistered())
		return (_printError(451, "ERR_NOTREGISTERED", ":You have not registered"));
	if (request.args.size() == 0)
		return (_printError(461, " ERR_NEEDMOREPARAMS", " :Not enough parameters"));
	std::vector<std::string>	parsChannels(_commaSeparator(request.args[0]));
	std::vector<std::string>::iterator it = parsChannels.begin();
	while (it != parsChannels.end())
	{
		int j = _partChannel(*it, i);
		if (j == NOSUCHCHANNEL /* No such channel */)
			return (_printError(403, " ERR_NOSUCHCHANNEL", *it + " :No such channel"));
		if (j == NOTINCHANNEL /* Not in channel */)
			return (_printError(442, " ERR_NOTONCHANNEL", *it + " :You're not on that channel"));
		it++;
	}
	// if (0 /* No such channel */)
	// if (0 /* Not in channel */)
	return ("");
};

std::vector<std::string> Server::_commaSeparator(std::string arg)
{
	std::vector<std::string> ret;
	int pos = 0;
	std::cout << "start parsing" << std::endl;
	while ((pos = arg.find(",")) != std::string::npos)
	{
		ret.push_back(arg.substr(0, pos));
		std::cout << ret.back() << std::endl;
		arg.erase(0, pos + 1);
	}
	ret.push_back(arg.substr(0, pos));
	std::cout << ret.back() << std::endl;
	return (ret);
};

int	Server::_createPrvChannel( std::string ChannelName, std::string ChannelKey, int CreatorFd)
{
	std::map<std::string, Channel *>::iterator it = this->_allChannels.find(ChannelName);
	if (it == this->_allChannels.end())
	{
		Channel *channel = new Channel(ChannelName, ChannelKey, this->_clients[CreatorFd]);
		this->_allChannels.insert(std::pair<std::string, Channel *>(ChannelName, channel));
		this->_clients[CreatorFd]->joinChannel(ChannelName, channel);
	}
	else
	{
		if (it->second->getKey() == ChannelKey)
		{
			int i = it->second->addMember(this->_clients[CreatorFd]);
			if (i == USERISJOINED)
				this->_clients[CreatorFd]->joinChannel( it->first, it->second );
			else if (i == USERALREADYJOINED)
				return (USERALREADYJOINED);
			else if (i == BANNEDFROMCHAN)
				return (BANNEDFROMCHAN);
			return (USERISJOINED);
		}
		else
			return (BADCHANNELKEY);
	}
	return (USERISJOINED);
}

int	Server::_createChannel( std::string ChannelName, int CreatorFd )
{
	std::map<std::string, Channel *>::iterator it = this->_allChannels.find(ChannelName);
	if (it == this->_allChannels.end())
	{
		Channel *channel = new Channel(ChannelName, this->_clients[CreatorFd]);
		this->_allChannels.insert(std::pair<std::string, Channel *>(ChannelName, channel));
		this->_clients[CreatorFd]->joinChannel( ChannelName, channel );
	}
	else
	{
		if (it->second->getKey().empty())
		{
			int i = it->second->addMember(this->_clients[CreatorFd]);
			if (i == USERISJOINED)
				this->_clients[CreatorFd]->joinChannel( it->first, it->second );
			else if (i == USERALREADYJOINED)
				return (USERALREADYJOINED);
			else if (i == BANNEDFROMCHAN)
				return (BANNEDFROMCHAN);
			return (USERISJOINED);
		}
	}
	return (USERISJOINED);
};

std::string	Server::_joinChannel( Request request, int i )
{
	int j = 1;
	if (!this->_clients[i]->getRegistered())
		return (_printError(451, "ERR_NOTREGISTERED", ":You have not registered"));
	if (request.args.size() == 0)
		return (_printError(461, " ERR_NEEDMOREPARAMS", " :Not enough parameters"));
	if (request.args[0] == "0")
		return(this->_clients[i]->leaveAllChannels());
	std::vector<std::string> parsChannels(_commaSeparator(request.args[0]));
		std::vector<std::string> parsKeys;
	if (request.args.size() == 2)
		parsKeys = _commaSeparator(request.args[1]);
	std::vector<std::string>::iterator itChannels = parsChannels.begin();
	std::vector<std::string>::iterator itKeys = parsKeys.begin();
	while (itChannels != parsChannels.end() && j == 1)
	{
		if ( itKeys != parsKeys.end())
			j = _createPrvChannel(*itChannels, *itKeys, i);
		else
			j = _createChannel(*itChannels, i);
		if (j == BANNEDFROMCHAN)
			return (_printError(474, " ERR_BANNEDFROMCHAN", *itChannels + " :Cannot join channel (+b)"));
		if (j == TOOMANYCHANNELS )
			return (_printError(405, " ERR_TOOMANYCHANNELS", *itChannels + " :You have joined too many channels"));
		if (j == BADCHANNELKEY )
			return (_printError(475, " ERR_BADCHANNELKEY", *itChannels + " :Cannot join channel (+k)"));
		if (j == CHANNELISFULL )
			return (_printError(471, " ERR_CHANNELISFULL", *itChannels + " :Cannot join channel (+l)"));
		if (j == NOSUCHCHANNEL)
			return (_printError(403, " ERR_NOSUCHCHANNEL", *itChannels + " :No such channel"));
		if (itKeys != parsKeys.end())
			itKeys++;
		itChannels++;
	};
	--itChannels;
	return (_printReply(332, "RPL_TOPIC", *itChannels + " :" + this->_allChannels.find(*itChannels)->second->getTopic()));
};

bool		Server::_validMode(Request request) {
	char	c = request.args[1][1];
	if (request.args[1].length() != 2 || (request.args[1][0] != '-' && request.args[1][0] != '+'))
		return false;
	if (c != 'a' && c != 'i' && c != 'w' && c != 'r' && c != 'o' && c != 'O' && c != 's')
		return false;
	return true;
}

std::string	Server::_printUserModes(std::string ret, int i)
{
	ret.append("a: " + std::to_string(this->_clients[i]->getMode('a')));
	ret.append("\ni: " + std::to_string(this->_clients[i]->getMode('i')));
	ret.append("\nw: " + std::to_string(this->_clients[i]->getMode('w')));
	ret.append("\nr: " + std::to_string(this->_clients[i]->getMode('r')));
	ret.append("\no: " + std::to_string(this->_clients[i]->getMode('o')));
	ret.append("\nO: " + std::to_string(this->_clients[i]->getMode('O')));
	ret.append("\ns: " + std::to_string(this->_clients[i]->getMode('s')) + "\n");
	return ret;
}

std::string	Server::_setMode(Request request, int i)
{
	if (!this->_clients[i]->getRegistered())
		return (_printError(451, "ERR_NOTREGISTERED", ":You have not registered"));
	if (request.args.size() < 2) {
		std::string	ret;
		if (request.args.size() == 1 && request.args[0] == this->_clients[i]->getNickName())
			ret = _printUserModes(ret, i);
		ret.append(std::to_string(461) + " ERR_NEEDMOREPARAMS\n\tPASS :Not enough parameters\n");
		return (ret);
	}
	if (request.args[0] != this->_clients[i]->getNickName())
		return (_printError(502, "ERR_USERSDONTMATCH", ":Cannot change mode for other users"));
	if (!_validMode(request))
		return (_printError(501, "ERR_UMODEUNKNOWNFLAG", ":Unknown MODE flag"));
	if (request.args[1][0] == '+')
		this->_clients[i]->setMode(true, request.args[1][1]);
	else
		this->_clients[i]->setMode(false, request.args[1][1]);
	return (_printReply(221, "RPL_UMODEIS", request.args[1]));
}

std::string	Server::_setOper(Request request, int i)
{
	if (!this->_clients[i]->getRegistered())
		return (_printError(451, "ERR_NOTREGISTERED", ":You have not registered"));
	if (request.args.size() < 2)
		return (_printError(461, "ERR_NEEDMOREPARAMS", "PASS :Not enough parameters"));
	if (request.args[1] != this->_clients[i]->getPassWord())
		return (_printError(464, "ERR_PASSWDMISMATCH", ":Password incorrect"));
	this->_clients[i]->setIsOperator(true);
	return (_printReply(381, "RPL_YOUREOPER", ":You are now an IRC operator"));
}

std::string	Server::_setPassWord(Request request, int i)
{
	if (request.args.size() < 1)
		return (_printError(461, "ERR_NEEDMOREPARAMS", "PASS :Not enough parameters"));
	if (this->_clients[i]->getRegistered())
		return (_printError(462, "ERR_ALREADYREGISTRED", ":Unauthorized command (already registered)"));
	this->_clients[i]->setPassWord(request.args[0]);
	return ("");
};

std::string	Server::_setNickName(Request request, int i)
{
	if (request.args.size() < 1)
		return (_printError(431, "ERR_NONICKNAMEGIVEN", ":No nickname given"));
	int	j = 0;
	while (request.args[0][j])
	{
		if (!isalnum(request.args[0][j]) && request.args[0][j] != '-')
			return (_printError(432, "ERR_ERRONEUSNICKNAME", request.args[0] + " :Erroneous nickname"));
		j++;
	}
	if (std::find(this->_clientNicknames.begin(), this->_clientNicknames.end(), request.args[0]) != this->_clientNicknames.end())
		return (_printError(433, "ERR_NICKNAMEINUSE", request.args[0] + " :Nickname is already in use"));

	this->_clients[i]->setNickName(request.args[0]);
	this->_clientNicknames.push_back(this->_clients[i]->getNickName());
	if (this->_clients[i]->getUserName() != "") {
		this->_clients[i]->setID(this->_clients[i]->getNickName() + "!" + this->_clients[i]->getUserName() + "@" + this->_clients[i]->getHost());
		this->_clients[i]->setRegistered(true);
		return (_printReply(1, "RPL_WELCOME", "Welcome to the Internet Relay Network " + this->_clients[i]->getID()));
	}
	return ("");
};

std::string	Server::_setUserName(Request request, int i)
{
	if (this->_clients[i]->getRegistered())
		return (_printError(462, "ERR_ALREADYREGISTRED", ":Unauthorized command (already registered)"));
	if (request.args.size() < 4)
		return (_printError(461, "ERR_NEEDMOREPARAMS", "USER :Not enough parameters"));

	this->_clients[i]->setUserName(request.args[0]);
	this->_clients[i]->setFullName(request.args[3]);
	if (this->_clients[i]->getNickName() != "") {
		this->_clients[i]->setID(this->_clients[i]->getNickName() + "!" + this->_clients[i]->getUserName() + "@" + this->_clients[i]->getHost());
		this->_clients[i]->setRegistered(true);
		return (_printReply(1, "RPL_WELCOME", "Welcome to the Internet Relay Network " + this->_clients[i]->getID()));
	}
	return ("");
};

std::string	Server::_quit(Request request, int i)
{
	std::string ret = ":" + this->_clients[i]->getID() + " QUIT ";
	if (request.args.size())
		ret.append(":" + request.args[0] + "\n");
	else
		ret.append("\n");
	_broadcastmsg(this->_clients[i]->getClientfd(), ret, ret.length());
	// close(this->_clients[i]->getClientfd());
	_removeFromPoll(i);
	return ("QUIT");
};

std::string	Server::_printHelpInfo(int i)
{
	std::string	helpInfo;

	helpInfo.append(GREEN);
	helpInfo.append("STEP 1: PASS (Optional)\n");
	helpInfo.append(RESET);
	helpInfo.append("\tUse PASS command to set a password. e.g: PASS MyAwesomePassword123\n\n");
	helpInfo.append(GREEN);
	helpInfo.append("STEP 2: NICK\n");
	helpInfo.append(RESET);
	helpInfo.append("\tUse NICK command to set a nickname. e.g: NICK deezNuts69\n\n");
	helpInfo.append(GREEN);
	helpInfo.append("STEP 3: USER\n");
	helpInfo.append(RESET);
	helpInfo.append("\tUse USER command to register your username and fullname.e.g: USER deez * * :Deez Nuts\n\n");
	return (helpInfo);
};


std::string	Server::_printUserInfo(int i)
{
	std::string info;
	info.append("NickName: " + this->_clients[i]->getNickName() + "\n");
	info.append("UserName: " + this->_clients[i]->getUserName() + "\n");
	info.append("FullName: " + this->_clients[i]->getFullName() + "\n");
	info.append("Host: " + this->_clients[i]->getHost() + "\n");
	info.append("ID: " + this->_clients[i]->getID() + "\n");
	info.append("PassWord: " + this->_clients[i]->getPassWord() + "\n");
	info.append("Operator: " + std::to_string(this->_clients[i]->getisOperator()) + "\n");

	info.append("/************************ List All Your Channels *****************************/\n");
	// std::map<std::string, Channel>::iterator it = this->_allChannels.begin();
	// while (it != this->_allChannels.end())
	// {
	// 	info.append(BLUE + it->first + RESET + ":\n");
	// 	info.append(YELLOW "\tChannel Name: " RESET + it->first + "\n");
	// 	info.append(YELLOW "\tChannel Name inside class: " RESET + it->second.getName() + "\n");
	// 	info.append(YELLOW  "\tChannel Creator: " RESET + it->second.getOperators().begin()->second.getFullName() + "\n");
	// 	it++;
	// }
	info.append(this->_clients[i]->JoinedChannels());
	return (info);
};



// USER mbari * * :Bari mehdi
// JOIN #test,&help
