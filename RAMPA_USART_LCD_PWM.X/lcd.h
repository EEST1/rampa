#ifndef _LCD_
#define _LCD_
#include <xc.h> 
// Si en otro proyecto tengo pines distintos cambio aquí solamente...
// La sguiente configuración es unicamente para usar con el shield 1.3
#define RS			LATBbits.LB7
#define RW			LATBbits.LB6
#define	E			LATBbits.LB5

#define DISPLAY                 LATB		// Es para denominar al puerto donde está colgado el bus
#define BUSY			PORTBbits.RB3	// Con estos defines me abstraigo del hardware
#define BUS_DIR                 TRISB

void LCD_init(void);				// Inicializa el LCD, requiere timer (interrupción)
void msg2LCD(unsigned char*);                   // Envía un mensaje para mostrar en LCD
void char2LCD(unsigned char);                   // Envía un caracter ascii solamente
void clear_LCD(void);				// Borra la pantalla del LCD
void ret_HOME(void);				// Ubica el cursor en la primera posición
void set_CURSOR(unsigned char);                 // Ubica el cursor en una posición determinada
void LCD_tic(void);				// Esta rutina se llama desde la interrupción de timer de 1mS


#endif