#include <iostream>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define MAXLINE 4096

void	checkError(int ret, std::string msg) {
	if (ret < 0) {
		std::cout << msg << std::endl;
		exit(1);
	}
}

int		sendAll(int clientfd, char *buffer, int *len) {
	int	total = 0;
	int bytesleft = *len;
	int n;

	while (total < *len) {
		n = send(clientfd, buffer + total, bytesleft, 0);
		if (n == -1)
			break;
		total += n;
		bytesleft -= n;
	}
	*len = total;
	return n == -1 ? -1: 0;
}

void	processRequest(int clientfd) {
	char	buffer[MAXLINE];
	int		n;

	n = send(clientfd, "Welcome to ft_irc server\n", 26, 0);
	if (n < 0) {
		std::cerr << "Error sending message" << std::endl;
		exit(1);
	}

	/* Reading client message */
	while (1) {
		bzero(buffer, MAXLINE);
		n = recv(clientfd, buffer, MAXLINE - 1, 0);
		if (n < 0) {
			std::cerr << "Error reading message" << std::endl;
			exit(1);
		}
		std::cout << buffer;

		/* Respond to client */
		n = send(clientfd, "I agree\n", 9, 0);
		if (n < 0) {
			std::cerr << "Error sending message" << std::endl;
			exit(1);
		}
	}
}

struct addrinfo*	initAddrInfo(const char **argv) {
	struct addrinfo		*servInfo;
	struct addrinfo		hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(argv[1], argv[2], &hints, &servInfo)) {
		std::cerr << "Invalid address" << std::endl;
		exit(1);
	}
	return servInfo;
}

int		getSockAndBind(struct addrinfo* servInfo) {
	int	sockfd;

	/* Getting socket fd */
	checkError(sockfd = socket(servInfo->ai_family, servInfo->ai_socktype, servInfo->ai_protocol), "Error opening socket");

	/* Reuse port  */
	int	yes = 1;
	checkError(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)), "setsockopt");

	/* Binding to host address */
	checkError(bind(sockfd, servInfo->ai_addr, servInfo->ai_addrlen), "Error in binding");

	return sockfd;
}

int		main(int argc, char const *argv[]) {
	struct addrinfo		*servInfo;
	int					sockfd, clientfd, pid;
	struct sockaddr_in	clientAddress;

	if (argc != 3) {
		std::cerr << "Specify host address and port number" << std::endl;
		exit(1);
	}

	servInfo = initAddrInfo(argv);
	sockfd = getSockAndBind(servInfo);
	freeaddrinfo(servInfo);

	/* Listening on host address */
	checkError(listen(sockfd, 5), "Error in listening");
	unsigned int clientLen = sizeof(clientAddress);

	while (1) {
		/* Accepting connections */
		checkError(clientfd = accept(sockfd, (struct sockaddr *)&clientAddress, &clientLen), "Error accepting request");

		/* Create child process */
		pid = fork();
		if (pid < 0) {
			std::cerr << "Error on forking" << std::endl;
			exit(1);
		}

		if (pid == 0) {
			/* Client process */
			close(sockfd);
			processRequest(clientfd);
			exit(0);
		}
		else {
			close(clientfd);
		}
	}
	return 0;
}
