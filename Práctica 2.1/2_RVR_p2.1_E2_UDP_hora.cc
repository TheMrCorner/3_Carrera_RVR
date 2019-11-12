#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <string.h>

int main(int argc, char **argv)
{
    struct addrinfo hints;
	bool exit=true;
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

    /*Gestionar Error
      if(sd==-1){}
    */

    bind(sd, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);

    while(exit){

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
         }


         return 0;

}
