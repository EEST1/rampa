#ifndef _LCD_
#define _LCD_
#include <xc.h> 
// Si en otro proyecto tengo pines distintos cambio aqu� solamente...
// La sguiente configuraci�n es unicamente para usar con el shield 1.3
#define RS			LATBbits.LB7
#define RW			LATBbits.LB6
#define	E			LATBbits.LB5

#define DISPLAY                 LATB		// Es para denominar al puerto donde est� colgado el bus
#define BUSY			PORTBbits.RB3	// Con estos defines me abstraigo del hardware
#define BUS_DIR                 TRISB

void LCD_init(void);				// Inicializa el LCD, requiere timer (interrupci�n)
void msg2LCD(unsigned char*);                   // Env�a un mensaje para mostrar en LCD
void char2LCD(unsigned char);                   // Env�a un caracter ascii solamente
void clear_LCD(void);				// Borra la pantalla del LCD
void ret_HOME(void);				// Ubica el cursor en la primera posici�n
void set_CURSOR(unsigned char);                 // Ubica el cursor en una posici�n determinada
void LCD_tic(void);				// Esta rutina se llama desde la interrupci�n de timer de 1mS


#endif