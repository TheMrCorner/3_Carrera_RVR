#include "Serializable.h"

#include <iostream>
#include <string>
#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, 80);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
       alloc_data(80*sizeof(char) + 2 * sizeof(int16_t));
       
       char* tmp = _data ;
       
       memcpy(tmp, name, sizeof(char)*80);
       
       tmp += sizeof(char) *80;
       
       memcpy(tmp, &x, sizeof(int16_t));
       
       tmp += sizeof(int16_t);
       
       memcpy(tmp, &y, sizeof(int16_t));
       
       tmp += sizeof(int16_t);
		
    }

    int from_bin(char * data)
    {
       char* tmp = data;
       
       memcpy(name, tmp, sizeof(char)*80);
       
       tmp += sizeof(char) *80;
       
       memcpy(&x, tmp, sizeof(int16_t));
       
       tmp += sizeof(int16_t);
       
       memcpy(&y, tmp, sizeof(int16_t));
      
    }
    
    void show_info(){
		std::cout << "Name: " << name << " PosX: " << x << " PosY: " << y << std::endl;
	}


public:
    char name[80];

    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
	// Atributes
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 12300, 987);
    const char * pathR = "jugador1.txt";
   
    int writeFD = open(pathR, O_RDWR | O_CREAT);
    int readFD = open(pathR, O_RDONLY);
    
    // Función
    one_w.to_bin();
    
    write(writeFD, one_w.data(), one_w.size());
    
    close(writeFD);
    
    char * data2 = (char*)malloc(one_w.size());
     
    read(readFD, data2, one_w.size());
    
    close(readFD);
    
    one_r.from_bin(data2);

    one_r.show_info();
    
    /*std::ofstream _wr_file;
    std::ifstream _rd_file;*/

	// Program
    // Serializar y escribir one_w en un fichero
  
    /*_wr_file.open("player_one.txt");
    one_w.to_bin();
    std::cout << one_w.data() << std::endl;
    _wr_file << one_w.data();
    _wr_file.close();*/

    // Leer el fichero en un buffer y "deserializar" en one_r
    /*_rd_file >> one_r.data();
    one_r.from_bin(one_w.data());*/
    
    // Mostrar el contenido one_r
    std::cout << one_r.data() << std::endl;

    return 0;
}
