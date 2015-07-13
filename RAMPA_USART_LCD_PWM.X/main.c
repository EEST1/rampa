/*
 * File:   main.c
 * Author: Esteban Lemos
 * Created on 5 de febrero de 2014, 20:49
 * En caso de necesitar el proyecto completo ingresar en
 * https://github.com/EEST1/Ap_template
 */
/*
 *Los LEDS  del shield están designados de izquierda a derechacomo LED1, LED2,
 *LED3 y LED4.
 *Los pulsadores de izquierda a derecha como BOT1, BOT2, BOT3, BOT4.
 *Para habilitar los displays de izquierda a derecha DISP1, DISP2, DISP3 y DISP4.
 *Para escribir un dato en el diplay se debe de escirbir en el nible menos
 *significativo del puerto A o usar la función Send_Disp(Nrodedisp , Dato)
 *El punto se controla con la variable DOT.
 */

#include <xc.h>
#include "confbits.h"   //no se debe de modificar este archivo para
                        //garantizar el funcionamiento del BOOTLOADER
#include"Ap_ini.h"      //define nombres para la entradas salidas del
                        //shield
#include"Lemos.h"       //funciones personalizadas Prof Lemos
#include"rampa.h"

/*Definiciones relacionadas con las operaciones validas*/
#define start 's'       //inicia la cuenta
#define stop 'd'        //para la cuenta
#define lectura 'l'     //transmite la cuenta
#define reset 'r'       //reinicializa la cuenta pero no para la cuenta


unsigned char caracter_recibido;            //Almacenaje del la recepción serie

void main(void) {
    unsigned char backup_ultimo_caracter;   //Var auxiliar para la rec serie
    unsigned int backup_tiempo_rampa;       //Var auxiliar para la cuenta
    char seg=0,dec=0,cent=0,mil=0; //Var para guardar el BCD
    char paquete[4];
    unsigned char broadcast_flag=0;         //
          
    pic_ini13();                    //inicializa las ent/salidas del shield 1.3
    timer_ini13();                  //inicializa el timer en 1ms para habilitar
                                    //el multiplexado de displays y la cuenta
    timer1_ini13();                 //inicializa el timer 1 para generar 38KHz
    usart_ini13();                  //inicializa el puerto serie a 9600
    ei();                           //habilitación global de interrupciones

    assign_id('a');                 //en caso de tratarse de un esclavo, asigna
                                    //el número enviado a slave_id
    rampa_ini13();                  //inicialización

    while(1){
        if(caracter_recibido != backup_ultimo_caracter){
            backup_ultimo_caracter=caracter_recibido;
/*decodifico el comando la secuencia correcta es <address> <comando><EOM>
 * address: es el estado que verifica si es solicitado el dispositivo especico
 *          la misma corresponde a un único byte codificado en ASCII
 * comando: se pueden enviar todos los bytes ASCII que uno desee pero
 *          unicamente se utilizará el primero de todos
 * EOM:     Sin este dato al final del paquete no se ejecuta el comando enviado
 * Si la secuencia es correcta se devuelve el comando**************************
 */
//****Que hago con el comando************************************************//
            switch(decode(caracter_recibido)){
                case ninguno:;
                break;
                case per2per:{
                    broadcast_flag=OFF;
                }break;
                case broadcast:{
                    broadcast_flag=ON;
                }break;
                case start:{
                    LED1=1;
                    //LED2=0;
                    rampa_status=ON;
                    INTCONbits.INT0IF=0;
                    INTCONbits.INT0IE=1;
                }break;
                case stop:{
                    LED1=0;
                    //LED2=1;
                    rampa_status=OFF;
                }break;
                case lectura:{
                    if(!broadcast_flag){        //me aseguro que no se escriba
                    LED1=1;
                    //LED2=1;
                    demora_envio=demora_envio_set;
                    envio_status=ON;
                    while(demora_envio);
                    informar(paquete,4);        //envia la lectura
                    envio_status=OFF;
                    }
                }break;
                case reset:{
                    LED1=0;
                    //LED2=0;
                    tiempo_rampa=0;
                }break;
            }
        }  
/*para no hacer la siguiente conversión a menos que sea necesario se realiza una
 *comprobación */
        if(tiempo_rampa !=backup_tiempo_rampa){ //separa la cuenta en BCD

                paquete[0]=tiempo_rampa/1000;          //obtiene los segundos
                paquete[1]=(tiempo_rampa%1000)/100;    //obtiene las decimas
                paquete[2]=(tiempo_rampa%100)/10;     //obtiene la centesimas
                paquete[3]=tiempo_rampa%10;            //obtiene las milesimas

                backup_tiempo_rampa=tiempo_rampa;
        }
    Send_4Disp(paquete[0],paquete[1],paquete[2],paquete[3]);               //se actualiza la info del disp
    }
}

void __interrupt myISR(){
    /*Aquí se ejecuta el código de las interrupciones que hayan sucedido,
     *por defecto el XC8 entra siempre en esta interrupción denominada de
     *Alta prioridad
     *NO OLVIDES DESCOMENTAR ei(), PARA HABILITAR LAS INTERRUPCIONES
     *
     */
    //el siguiente código verifica que el flag de TIMER0 esté habilitado
#ifdef AP_INI_H
        if(TMR0IF){
            INTCONbits.TMR0IF = 0;  //borra el flag de la interrupción de timer
            TMR0L = 209;           //reinicia el time
            TMR0H = 0xFF;
        #ifdef  LEMOS_H
            tic_timer0();           //llama a la función tic_timer
        #endif
        #ifdef _LCD_
            LCD_tic();
        #endif
        #ifdef RAMPA_H
           if(rampa_status) RAMPA_tic();
           if(demora_envio) ENVIO_tic(); 

        #endif
        }
#endif
#ifdef RAMPA_H
            if(TMR1IF){                 //Interrupción del TMR1
                PIR1bits.TMR1IF = 0;
                TMR1L=144;
                TMR1H=0xFF;     
                SAL_IR =~ SAL_IR;
            }
            if(RCIF){                   //Interrupción de la recepción
                RCIF=0;  
                caracter_recibido=RCREG;
                (void) RCREG;
            }
            if(INT0IF){                 //Interrupción del INT0
                rampa_status=OFF;
                INTCONbits.INT0IE=0;
                INTCONbits.INT0IF=0;
            }
#endif
}


