#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char const *argv[]){
	
	struct sockaddr src_addr;
	socklen_t addrlen = sizeof(struct sockaddr);
		
	struct addrinfo hints;

    struct addrinfo *res;
    
    bool end_connection = false;

    memset((void*)&hints,0,sizeof(struct addrinfo));

    hints.ai_family = AF_INET;

    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo( argv[1], argv[2], &hints, &res);

    if( rc !=0){
        std::cout<< "Error getaddrinfo:" <<gai_strerror(rc)<<std::endl;
        return -1;
    }

    int sd = socket(res->ai_family, res->ai_socktype, 0);

    freeaddrinfo(res);
    
    connect(sd, (struct sockaddr *)res->ai_addr, res->ai_addrlen);
	
	//BUCLE PARA LA CONEXIÓN
	while(!end_connection)
	{
		
		ssize_t bytes = 0;
		char buffer[80];
		std::string msg;

        char _buffer[80];
        ssize_t _bytes;

		char host[NI_MAXHOST];
		char serv[NI_MAXSERV];
		
		std::getline(std::cin, msg);
		    
        strcpy(buffer, msg.c_str());

        bytes = sizeof(char) * sizeof(buffer);

		send(sd, buffer, bytes, 0);
			
		_bytes = recv(sd, (void*) _buffer, sizeof(char)*80, 0);
		
		_buffer[_bytes] = '\0';

        if(buffer[0] == 'Q'){
            end_connection = true;
		}
		else{
            std::cout << _buffer << std::endl;
        }
		        	
	}

    close(sd);

	return 0;
}