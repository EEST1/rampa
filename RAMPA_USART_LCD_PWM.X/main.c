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
    
    char paquete[4];                        //arreglo para el envio serie
    unsigned char broadcast_flag=0;         //flag auxiliar de broadcast
    backup_tiempo_rampa = ReadADC();
          
    pic_ini13();                    //inicializa las ent/salidas del shield 1.3
    timer_ini13();                  //inicializa el timer en 1ms para habilitar
                                    //el multiplexado de displays y la cuenta
    timer1_ini13();                 //inicializa el timer 1 para generar 38.2KHz
    usart_ini13();                  //inicializa el puerto serie a 9600
   
    /* Si no se utiliza el MAX232 se debe de ejecutar lo siguiente:  */
    BAUDCONbits.RXCKP = 1;
    BAUDCONbits.TXCKP = 1;
    /* En caso contrario comentar las lineas                         */
    
    ei();                           //habilitación global de interrupciones

    assign_id('a');                 //en caso de tratarse de un esclavo, asigna
                                    //el número enviado a slave_id
    rampa_ini13();                  //inicialización

    while(1){
            if(caracter_recibido != backup_ultimo_caracter){
            backup_ultimo_caracter=caracter_recibido;
            
/*decodifico el comando la secuencia correcta es:
 * 
 *                       <address> <comando><EOM>
 * 
 * 
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
                    rampa_status=ON;
                    INTCONbits.INT0IF=0;
                    INTCONbits.INT0IE=1;
                    SOLENOIDE_ON;
                    demora_solenoide = demora_solenoide_set;
                }break;
                case stop:{                   
                    rampa_status=OFF;
                    INTCONbits.INT0IE=0;
                    INTCONbits.INT0IF=0;
                }break;
                case lectura:{
                    if(!broadcast_flag) informar(paquete,4);        //envia la lectura
                   
                }break;
                case reset:{
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
   #ifdef AP_INI_H
        if(TMR0IF){                 //interrupción de timer0
            INTCONbits.TMR0IF = 0;  //borra el flag de la interrupción de timer
            TMR0L = 209;            //reinicia el timer0
            TMR0H = 0xFF;
        #ifdef  LEMOS_H
            tic_timer0();           //llama a la función tic_timer
        #endif
        #ifdef _LCD_
            LCD_tic();
        #endif
        #ifdef RAMPA_H
           if(rampa_status) RAMPA_tic();    //incrementa el tiempo de rampa
           if(demora_envio) ENVIO_tic();    //decrementa la demora_envio
           if(SOLENOIDE) SOLENOIDE_tic();
          //  MODULA_tic();
           

        #endif
        }
#endif
#ifdef RAMPA_H
            if(TMR1IF){                 //Interrupción del TMR1
                PIR1bits.TMR1IF = 0;
                TMR1L=144;
                TMR1H=0xFF;     
            //if(flag_modula)    SAL_IR = ~SAL_IR;
            //else SAL_IR=0;
                SAL_IR = ~SAL_IR;
                
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


