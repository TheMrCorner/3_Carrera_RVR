/*	En la consola:
	> vim udp_mt_cc
	Despues de crear el c++
	> g++ -o udp_mt udp_mlt.cc -lpthread -std=c++0x
	> ./udp_mt 0.0.0.0 3000
	En otra consola
	>nc -u 127.0.0.1 3000
	>ps -alL

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

class MessageThread
{
	public:
	
		MessageThread(int _sd):sd(_sd){};
		
		void do_message()
		{
		    bool exit = true;
			while(exit)
			{

				int i = 0;
				
				ssize_t bytes;
				char buffer[80];
				struct sockaddr src_addr;
				socklen_t addrlen = sizeof(struct sockaddr);

				char host[NI_MAXHOST];
				char serv[NI_MAXSERV];

				bytes = recvfrom(sd, (void*) buffer, sizeof(char)*80, 0, &src_addr, &addrlen);

				buffer[bytes] = '\0';

				getnameinfo( &src_addr, addrlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV );
				std::cout<<bytes<< " bytes de " <<host<< ":" <<serv<< std::endl;

			   time_t t = time(nullptr);
			   struct tm * timeinfo = localtime(&t);


			   if(buffer[0]=='t'){
				 strftime(buffer, 80, "%I:%M%p.", timeinfo);
				 puts(buffer);

				 bytes = sizeof(timeinfo);
				 buffer[bytes] = '\0';
				 sendto(sd, buffer, bytes, 0, &src_addr, addrlen);

			   }else if(buffer[0]=='d'){
				   
				 strftime(buffer, 80, "%F %a", timeinfo);
				 puts(buffer);

				 bytes = 14;
				 buffer[bytes] = '\0';
				 sendto(sd, buffer, bytes, 0, &src_addr, addrlen);

			   }else if(buffer[0]=='q'){
				 buffer[0] = '^';
				 buffer[1] = 'C';
				 
				 sendto(sd, buffer, 2, 0, &src_addr, addrlen);
				 std::cout<<"Saliendo..."<< std::endl;
				 exit=false;
			   }else{
			   
				std::cout<<"Comando no soportado: "<<buffer<< std::endl;

			   }
				
				pthread_mutex_lock(&mutex);
				
				fin = ++i > 5;
				
				if(fin)
				{
					pthread_cond_signal(&cond);
				}
				
				pthread_mutex_unlock(&mutex);
			} 	
		}
	
	private:
		//Socket
		int sd;
		
		//Mensajes
		ssize_t bytes;
		char buffer[80];	
};

extern "C" void *_do_message(void * _mt)
{
        MessageThread * mt = static_cast <MessageThread *>(_mt);
        mt->do_message();
        delete mt;
        return 0;
}


int main(int argc, char **argv)
{
    struct addrinfo hints;

    struct addrinfo *res;

    memset((void*)&hints,0,sizeof(struct addrinfo));

    hints.ai_family = AF_UNSPEC;

    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo( argv[1], argv[2], &hints, &res);

    if( rc !=0){
        std::cout<< "Error getaddrinfo:" <<gai_strerror(rc)<<std::endl;
        return -1;
    }

    int sd = socket(res->ai_family, res->ai_socktype, 0);

    bind(sd, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);
	
	
	//man pthread_create
	
	//CREAR THREADS
	for(int i=0; i<5; i++)
	{
		MessageThread * mt = new MessageThread(sd);
		
		pthread_t thread_id;
		pthread_attr_t attr;
		
		pthread_attr_init(&attr);
		
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHABLE);
		
		pthread_create(&thread_id, &attr, _do_message, mt);
	}
	
	//ESPERAR FIN
	pthread_mutex_lock(&mutex);
	while(!fin)
	{
		pthread_cond_wait(&cond, &mutex);
	}

         


    return 0;

}