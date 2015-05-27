#include <xc.h>
#include "rampa.h"
#include "Ap_ini.h"



void assign_id(unsigned char id){
    slave_id=id;
}
void RAMPA_tic(void){
    if(tiempo_rampa<9999)tiempo_rampa++;
}

unsigned char decode(unsigned char buffer){
/*esta función es una máquina de estados que recorre los distintos bytes del
 * paquete los mismos estarán compuestos por |address|-|comando|-|fin| */
    static unsigned char est_rec=address;
    static char com_rec;      //el ultimo comando recivido
    unsigned char retorno;

    switch(est_rec){
        case address:{
            if(buffer==slave_id || buffer==broadcast_id){
                est_rec++;
                buffer = 0;
                retorno=ninguno;
              
                }
        }break;

        case comando:{
            if(buffer){              //buffer distinto de cero
                com_rec=buffer;
                est_rec++;
                buffer = 0;
                retorno=ninguno;
          
            }
        }break;

        case fin:{
            if(buffer==EOM){
                est_rec=address;
                retorno = com_rec;
                com_rec=ninguno;
            }
        }break;
    }
    return retorno;
}
void Transmitir(char Dato){
    TXSTAbits.TXEN = ON;
    while(TRMT);
    TXREG = Dato;
    TXSTAbits.TXEN = OFF;
    
}




