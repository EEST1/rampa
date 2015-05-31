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

#define ninguno 0
#define OFF 0
#define ON 1

#define SAL_IR LATBbits.LATB7






enum estados{                       //los estados posibles
    address,
    comando,
    fin,
    respuesta

};



unsigned char slave_id;     //fija el id
const unsigned char broadcast_id = 'm';
int tiempo_rampa=0;
unsigned char rampa_status=ON;




//funciones
unsigned char decode(unsigned char);    //decodifica el paquete
void assign_id(unsigned char);          //inicializa el id del esclavo
void RAMPA_tic(void);
void Transmitir(char);
void informar(unsigned char,unsigned char,unsigned char,unsigned char);