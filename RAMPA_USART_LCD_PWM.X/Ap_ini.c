#include <xc.h>
#include "Ap_ini.h"


/*este código está pensado para inicializar correctamente la placa o shield
1.3 y debe de ser utilizado con cuidado para otras placas o aplicaciones*/


void pic_ini13(void){

    //Borrado de todos los puertos
    LATA = 0x00;
    LATB = 0x00;
    LATC = 0x00;
    LATD = 0x00;
    LATE = 0x00;

    //LEDS y pulsadores

    TRISD = 0xF0;       //RD0 a RD3 salidas RD4 a RD7 entradas
    TRISCbits.RC6 = 0;  //RC6 y RC7 salidas
    TRISCbits.RC7 = 0;

    //Display

    CMCON = 0x07;       //comparadores de entrada deshabilitados
    ADCON1 = 0x0F;      //Conversores AD deshabilitados
    TRISA = 0xC0;       //RA1 a RA5 salidas
    TRISE = 0x00;       //RE0 a RE2 salidas el resto no está implementado

    //Puerto B

    TRISB = 0xFF;       //todas entradas, de ser necesario será configuradas por
                        //el alumno según el mismo necesite
}

void timer_ini13(){
    T0CONbits.TMR0ON = 0;   //apago el timer
    T0CONbits.T08BIT = 1;   //selecciono el timer en 8 bits
    T0CONbits.T0CS = 0;     //el timer cuenta fosc/4
    T0CONbits.PSA = 0;      //habilito el preescaler
    T0CONbits.T0PS0 = 1;    //cargo el preescaler con 256
    T0CONbits.T0PS1 = 1;
    T0CONbits.T0PS2 = 1;
    TMR0L = 209;            //el timer contará 47 fosc/4 * 256 = 12032 * 0,083us
    TMR0H = 0xFF;           //en total aprox 1.0022ms  casi 1ms
    INTCONbits.TMR0IE = 1;  //Habilita la interrupción de timer 0
    T0CONbits.TMR0ON = 1;   //enciendo el time0

}
void timer1_ini13(){

    T1CONbits.T1CKPS0=0;    //configuro el preescaler
    T1CONbits.T1CKPS1=0;
    TMR1L=145;               //el timer1 contará 157 * 1 * 0.0833us = 13,07us
    TMR1H=0xFF;             //ese tiempo * 2 = 26.156us para una frec = 38.231
    PIR1bits.TMR1IF=0;
    PIE1bits.TMR1IE=1;      //habilita la interrupción del timer 1
    T1CONbits.TMR1ON = 1;   //enciendo el timer1
}
void pwm_ini13(void){
    PR2=0x00;               //PWM_Perior=[PR2+1]*4*Tosc*(TMR2 Preescaler)
    CCPR1L=0;               //Duty=(CCPR1L:CCP1CON<5:4>)*TOSC*(TMR2 Preescaler)
    CCP1CONbits.DC1B1=0;
    CCP1CONbits.DC1B0=1;
    TRISCbits.RC2=0;        //Salida de PWM RC2 activada
    T2CONbits.TMR2ON=1;     //Timer 2 encendido
    CCP1CONbits.CCP1M3 =1;
    CCP1CONbits.CCP1M2 =1;

}

void pwm_set(unsigned char periodo, unsigned char duty ){
   if(duty>0 && duty!=CCPR1L)CCPR1L=duty;
   if(periodo>0 && periodo!=PR2)PR2=periodo;
}
void usart_ini13 (void){


    
    
    TXSTAbits.BRGH = 1;     // High Baud Rate Select bit
    BAUDCONbits.BRG16=1;
    TXSTAbits.SYNC = 0;     //modo asincrónico
    SPBRG = 0xE1;
    SPBRGH = 0x04;           //=(FOSC/9600)/4-1 donde FOSC=48MHz
    RCSTAbits.SPEN  = 1;    //Habilita el puerto serie
    
    //transmisión
    TXSTAbits.TX9 = 0;      //recepción y transmisión de 8 bits
    TXSTAbits.TXEN = 0;     //Inicialmente el transmisor está desactivado.

    //recepción
    (void) RCREG;
    (void) RCREG;
    (void) RCREG;
    RCSTAbits.RX9 = 0 ;     //Recepción de 8 bits
    PIE1bits.RCIE = 1;      //habilita la interrupción de recepción
    RCSTAbits.CREN = 1;     //Habilita la recepción
    INTCONbits.PEIE = 1;    //Habilita las interrupciones parcialmente

    TRISCbits.RC6 = 1;  //RC6 y RC7 salidas
    TRISCbits.RC7 = 1;

    
}


