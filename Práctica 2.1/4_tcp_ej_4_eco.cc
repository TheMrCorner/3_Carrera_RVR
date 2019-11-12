/*	En la consola:
	> vim tcp.cc
	Despues de cargar el codigo
	>g++ -o tcp tcp.cc 
	> ./tcp localhost 7777
	En otra terminal
	> ss -tanp
	> nc localhost 7777
	
	Faltaba añadir los includes
	En la Línea 68 la función era rcv, pero es necesario poner rcvfrom, funciona correctamente

*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
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
	
    bind(sd, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);
	
	//PUBLICAR EL SERVIDOR
	listen(sd,16);
	
	//BUCLE PARA LA CONEXIÓN
	while(true)
	{
		
		ssize_t bytes;
		char buffer[80];
		struct sockaddr src_addr;
		socklen_t addrlen = sizeof(struct sockaddr);
		std::string end_msg = "";

		char host[NI_MAXHOST];
		char serv[NI_MAXSERV];

		
		
		//Devuelve socket de la conexion establecida
		int sd_cliente = accept(sd, &src_addr, &addrlen);
		
		getnameinfo( &src_addr, addrlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV );
        
        std::cout<< "Conexión desde IP: " <<host<< "\t:Puerto: " <<serv<< std::endl;
		   
		//BUCLE PARA CADA CONEXIÓN
		while(!end_connection){

            bytes = recv(sd_cliente, (void*) buffer, sizeof(char)*80, 0);

		    buffer[bytes] = '\0';
		
		    if(bytes <= 0 || buffer[0] == 'Q' ){
                std::cout << "Conexión finalizada" << std::endl;
				strcpy(buffer, end_msg.c_str());
				send(sd_cliente, buffer, bytes, 0); 
			    end_connection = true;
		    }
		    else{//ENVIAR RESPUESTA
			    //std::cout<< "Mensaje: " << buffer << std::endl;
                send(sd_cliente, buffer, bytes, 0);   
		    }		
        }
	}
	return 0;	
}
