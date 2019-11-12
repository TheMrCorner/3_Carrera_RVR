#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char const *argv[]){
			
	struct addrinfo hints;
    struct addrinfo *res;
    bool end_connection = false;

    memset((void*)&hints,0,sizeof(struct addrinfo));

    hints.ai_family = AF_INET;

    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo( argv[1], argv[2], &hints, &res);

    if( rc !=0){
        std::cout<< "Error getaddrinfo:" <<gai_strerror(rc)<<std::endl;
        return -1;
    }

    int sd = socket(res->ai_family, res->ai_socktype, 0);
    if(sd==-1){
            std::cout << "Error, Socket mal inicializado" << std::endl;
    }

    freeaddrinfo(res);
	
		//CONEXIÓN CON SERVIDOR
	
		ssize_t bytes = 0;
		char buffer[80];
		std::string msg;
		struct sockaddr src_addr;
		socklen_t addrlen = sizeof(struct sockaddr);

        char _buffer[80];
        ssize_t _bytes;

		char host[NI_MAXHOST];
		char serv[NI_MAXSERV];
	
        msg = argv[3];
        strcpy(buffer, msg.c_str());

        bytes = sizeof(char) * sizeof(buffer);

		if(sendto(sd, buffer, bytes, 0, res->ai_addr, res->ai_addrlen) == -1){
			std::cout << "msg no enviado" << std::endl;
		}

		_bytes = recvfrom(sd, (void*) _buffer, sizeof(char)*80, 0, &src_addr, &addrlen);
		

		_buffer[_bytes] = '\0';

        if(_buffer[0] == 'q' || _bytes<=0){
            end_connection = true;
            std::cout << "Conexión fallida"<< std::endl;
		}
		else{
            std::cout << _buffer << std::endl;
        }	

    close(sd);

	return 0;
}