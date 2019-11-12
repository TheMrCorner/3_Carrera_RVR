/*	En la consola:
	> vim tcp_mt.cc
	Despues de cargar el codigo
	>g++ -o tcp_mt tcp_mt.cc -pthread
	> ./tcp_mt 0.0.0.0 2222
	En otra terminal
	> ss -tanp
	> nc 127.0.0.1 2222
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static bool fin = false;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

class ConnectionThread
{
	public:
	
		ConnectionThread(int sd):sd_cliente(sd){};
		
		void do_connection()
		{
			//BUCLE PARA CADA MENSAJE DE CONEXIÓN
			while(!end_connection)
			{
	
				bytes = recvfrom(sd_cliente, (void*) buffer, sizeof(char)*80, 0, &src_addr, &addrlen);
				buffer[bytes] = '\0';
				
				if(bytes <= 0){
					end_connection = true;
				}
				else{//ENVIAR RESPUESTA
					std::cout<< "Mensaje: " <<buffer;
					send(sd_cliente, buffer, bytes, 0);
				}	
				

			}
		}
	
	private:
		//Socket
		int sd_cliente;
		
		char host[NI_MAXHOST];
		char serv[NI_MAXSERV];
		
		struct sockaddr src_addr;
		socklen_t addrlen = sizeof(struct sockaddr);
		
		//Mensajes
		ssize_t bytes;
		bool end_connection = false;
		char buffer[80];
};

extern "C" void * _do_connection(void * _ct)
{
	ConnectionThread * ct = static_cast <ConnectionThread *>(_ct);
	ct->do_connection();
	delete ct;
	return 0;
}

int main(int argc, char **argv)
{
    struct addrinfo hints;

    struct addrinfo *res;

    memset((void*)&hints,0,sizeof(struct addrinfo));

    hints.ai_family = AF_UNSPEC;

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

		char host[NI_MAXHOST];
		char serv[NI_MAXSERV];

		
		
		//Devuelve socket de la conexion establecida
		int sd_cliente = accept(sd, &src_addr, &addrlen);
		
		getnameinfo( &src_addr, addrlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV );
        std::cout<< "Conexión desde IP: " <<host<< "\t:Puerto: " <<serv<< std::endl;
		 
		   
		//CREAR THREADS
		ConnectionThread * ct = new ConnectionThread(sd_cliente);
		
		pthread_t thread_id;
		pthread_attr_t attr;
		
		pthread_attr_init(&attr);
		
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		
		pthread_create(&thread_id, &attr, _do_connection, ct);

		
		//ESPERAR FIN - No sé si esto también va, probar:
		/*pthread_mutex_lock(&mutex);
		while(!fin)
		{
			pthread_cond_wait(&cond, &mutex);
		}/*/
		
	}
	return 0;	
}
