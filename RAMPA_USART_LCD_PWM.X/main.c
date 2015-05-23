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
#include"lcd.h"
#include"rampa.h"
//#include"Robello.h"     //funciones personalizadas prof Robello

#define start 's'
#define stop 'd'
#define lectura 'l'
#define reset 'r'

unsigned char caracter;

void main(void) {
    unsigned char backup,seg=0,dec=0,cent=0,mil=0;
    unsigned int backup_rampa;
          
    pic_ini13();                    //inicializa las ent/salidas del shield 1.3
    timer_ini13();                  //inicializa el timer para habilitar
                                    //el multiplexado de displays
    usart_ini13();
    ei();                           //habilitación global de interrupciones
    LCD_init();                     //requiere interrupciones de timer
    assign_id('a');                 //en caso de tratarse de un esclavo, asigna
                                    //el número enviado a slave_id
    msg2LCD("Welcome");
    rampa_status=OFF;               //global definida en rampa
    tiempo_rampa=0;                 //global lleva la cuenta en milisegundos

    while(1){
        if(caracter != backup){
            char2LCD(caracter);
            backup=caracter;
/*decodifico el comando la secuencia correcta es <address> <comando><EOM>
 * address: es el estado que verifica si es solicitado el dispositivo especico
 *          la misma corresponde a un único byte codificado en ASCII
 * comando: se pueden enviar todos los bytes ASCII que uno desee pero
 *          unicamente se utilizará el primero de todos
 * EOM:     Sin este dato al final del paquete no se ejecuta el comando enviado
 * Si la secuencia es correcta se devuelve el comando**************************
 */
//****Que hago con el comando************************************************//
            switch(decode(caracter)){
                case ninguno:;
                break;
                case start:{
                    LED1=1;
                    LED2=0;
                    rampa_status=ON;
                }break;
                case stop:{
                    LED1=0;
                    LED2=1;
                    rampa_status=OFF;
                }break;
                case lectura:{
                    LED1=1;
                    LED2=1;
                }break;
                case reset:{
                    LED1=0;
                    LED2=0;
                    tiempo_rampa=0;
                }break;
            }
        }  
        if(tiempo_rampa !=backup_rampa){
                seg=tiempo_rampa/1000;
                dec=(tiempo_rampa%1000)/100;
                cent=(tiempo_rampa%100)/10;
                mil=tiempo_rampa%10;
                backup_rampa=tiempo_rampa;
        }
    Send_4Disp(seg,dec,cent,mil);
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
        #endif
        }
        #ifdef ROBELLO_H
            if(MEMDIG == 1)
                mux_display();
                TMR0IF = 0; //borra el flag de la interrupción de timer
            return;                     //termina el código de interrupción
        #endif
#endif
#ifdef RAMPA_H
            if(RCIF){
                RCIF=0;  
                caracter=RCREG;
                (void) RCREG;
            }
#endif
}

