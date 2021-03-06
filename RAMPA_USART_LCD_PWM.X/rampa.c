#include <xc.h>
#include "rampa.h"
#include "Ap_ini.h"

void rampa_ini13(void){
    rampa_status = OFF;             //global definida en rampa
    tiempo_rampa = 0;               //global lleva la cuenta en milisegundos
    TRISBbits.RB7 = 0;              //Salida IR
    TRISBbits.RB0 = 1;              //Entrada 
    INTCON2bits.INTEDG0=0;          //interrupt in falling edge
}

void assign_id(unsigned char id){
    slave_id=id;
}
void RAMPA_tic(void){
    if(tiempo_rampa<9999)tiempo_rampa++;
}
void ENVIO_tic(void){
    if(demora_envio)demora_envio--;     //mejorar!!!!!!!!
}
void MODULA_tic(void){
    if(modula)modula--;
    else{
        flag_modula=~flag_modula;
        modula=modula_set;
    }
}
void SOLENOIDE_tic(void){
    if(demora_solenoide)demora_solenoide--;
    else SOLENOIDE_OFF;
}

unsigned char decode(unsigned char dato_a_decod){
/*esta funci�n es una m�quina de estados que recorre los distintos bytes del
 * paquete los mismos estar�n compuestos por |address|-|comando|-|fin| */
    
    static unsigned char est_rec=address;   //estado de la m�quina
    
    static char com_rec;                    //el ultimo comando recivido
    unsigned char retorno;      //variable que ser� utilizada para guardar
                                //el dato a devolver
    switch(est_rec){                        //revisa el estado de la m�quina
        case address:{                      //estado de direccionamiento
            if(dato_a_decod==slave_id ){    //direccionamiento particular
                est_rec++;
                dato_a_decod = 0;
                retorno=per2per;            //se retorna el status
            }
            else if(dato_a_decod == broadcast_id){//direccionamiento global
                est_rec++;
                dato_a_decod = 0;
                retorno=broadcast;          //se retorna el status
            }
        }break;

        case comando:{                      //estado de interpretaci�n de com
            if(dato_a_decod){               //dato_a_decod distinto de cero
                com_rec=dato_a_decod;       //guarda el comando
                est_rec++;
                dato_a_decod = 0;
                retorno=ninguno;            //se retorna un estado de espera
          
            }
        }break;
/*Se deber�a implementar un time out para evitar posibles cuelgues*/
        case fin:{                          //estado de fin de paquete
            if(dato_a_decod==EOM){          //se espera el fin de paquete
                est_rec=address;            //se re inicializa la m�quina
                retorno = com_rec;          //se retorna el comando guardado
                com_rec=ninguno;            //se borra el comando

            }
        }break;
    }
    return retorno;                         //se retorna de la funcion
}
void informar(char *dato, char longitud){
    char i=0;
    RESPONDER_ON;                           //toma el canal cedido por el
    demora_envio=demora_envio_set;          //maestro                  
    while(demora_envio);                    //espera un tiempo para asegurar
                                            //la direcci�n correcta del BUS
    transmitir(slave_id);                   //transmite el address 
    
    for(i=0;i<longitud;i++) transmitir(*(dato+i)+48);   //transmite el dato

    transmitir(EOM);                        //finaliza el mensaje con EOM
    
    demora_envio=demora_envio_set-10;       //espera un tiempo para devolver              
    while(demora_envio);                    //el canal al maestro
    RESPONDER_OFF;                          //cede el canal
}
void transmitir(char Dato){        //transmite un char
    
    while(!TXSTAbits.TRMT);
    TXREG = Dato;                           //se transmite el dato
    //while(!TXSTAbits.TRMT);
    
    
}




