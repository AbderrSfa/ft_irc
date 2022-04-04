#include <iostream>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

void	sendRequest(int sockfd) {
	char	buffer[256];
	int		n;

	/* Ask for a message from the user */
	std::cout << "Enter a message" << std::endl;
	bzero(buffer, 256);
	fgets(buffer, 255, stdin);

	/* Send message to the server */
	n = send(sockfd, buffer, strlen(buffer), 0);
	if (n < 0) {
		std::cerr << "Error sending message" << std::endl;
		exit(1);
	}

	/* Read server's response */
	bzero(buffer, 256);
	n = recv(sockfd, buffer, 255, 0);
	if (n < 0) {
		std::cerr << "Error sending message" << std::endl;
		exit(1);
	}
	std::cout << buffer;
}

int main(int argc, char const *argv[])
{
	struct addrinfo		hints;
	struct addrinfo		*clientInfo;
	int					sockfd, n;
	char				buffer[256];

	
	if (argc != 3) {
		std::cerr << "Specify host address and port number" << std::endl;
		exit(1);
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(argv[1], argv[2], &hints, &clientInfo)) {
		std::cerr << "Invalid address" << std::endl;
		exit(1);
	}

	/* Getting socket fd */
	sockfd = socket(clientInfo->ai_family, clientInfo->ai_socktype, clientInfo->ai_protocol);
	if (sockfd < 0) {
		std::cerr << "Error opening socket" << std::endl;
		exit(1);
	}

	/* Connecting to server */
	if (connect(sockfd, clientInfo->ai_addr, clientInfo->ai_addrlen) < 0) {
		std::cerr << "Error connecting" << std::endl;
		exit(1);
	}

	sendRequest(sockfd);
	close(sockfd);
	return 0;
}
