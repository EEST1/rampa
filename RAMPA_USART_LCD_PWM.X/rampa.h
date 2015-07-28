/* 
 * File:   rampa.h
 * Author: esteban
 *
 * Created on 16 de mayo de 2015, 11:15
 */

#ifndef RAMPA_H
#define	RAMPA_H

#ifdef	__cplusplus
extern "C" {
#endif



#ifdef	__cplusplus
}
#endif

#endif	/* RAMPA_H */



//variables
#define EOM 'f'                     //fin de mensaje
#define broadcast 'b'               //indicador de broadcast
#define per2per 'p'                 //indicador de direccionamiento unico
#define demora_envio_set   15
#define modula_set 2

#define ninguno 0
#define OFF 0
#define ON 1

#define SAL_IR LATBbits.LATB7       //salida de 38.2KHz para el LED INFRAROJO
#define RESPONDER_ON    LATDbits.LD3=1
#define RESPONDER_OFF   LATDbits.LD3=0
#define SOLENOIDE_ON    LATDbits.LD2=1
#define SOLENOIDE_OFF   LATDbits.LD2=0






enum estados{                       //los estados posibles
    address,
    comando,
    fin,
    respuesta
};


//variables globales

unsigned char slave_id;                     //fija el id
const unsigned char broadcast_id = 'm';     //fija el broadcast_id
unsigned int tiempo_rampa=0;                //inicializa el tiempod e cuenta
unsigned char rampa_status=OFF;             //estado de la rampa
unsigned char demora_envio=0;               //variable auxiliar para el ajuste
                                            //de los tiempos en el protocolo
unsigned char modula=modula_set;
unsigned char flag_modula =0;



//funciones
unsigned char decode(unsigned char);    //decodifica el paquete
void assign_id(unsigned char nro_esclavo);          //inicializa el id del esclavo
void RAMPA_tic(void);                   //cuenta cada interrupción de timer
void ENVIO_tic(void);                   //cuenta cada interrupción de timer
void MODULA_tic(void);
void transmitir(char dato);                  //transmite un char
void informar(char *cadena,char longitud);
void rampa_ini13(void);                 //inicializa la rampa