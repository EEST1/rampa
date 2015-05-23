#include "lcd.h"

/* esta librería solo puede utilizar las cuatro líneas menos significativas de 
 * un puerto en caso de querer utilizar otras líneas se debe de reprogramar
 * es necesarío este habilitada la interrupción de timer en la cual se debe de 
 * llamar a la función LCD_tic() la misma establece las demoras que de otra
 * manera habría que implementarlas como una función aparte 
 */
#define ENTRADA	0x0F	// Esto es para invertir el bus de datos y poder
#define SALIDA	0xF0    //leer cuando necesito ver si está busy..

#define TRUE    0x1
#define FALSE   0x0

unsigned short LCD_tout;								// Contador para decrementar cada 1mS

void write_DATA(unsigned char dato);	// Pone señales para escribir dato
void write_CMD(unsigned char dato);	// Pone señales para escribir comando
void write_LCD (unsigned char dato);	// Pone byte en el bus 
void read_BUSY(void);                   // Verifica si terminó la operación en proceso

// Esta rutina recibe un puntero a char e imprime en el LCD ese string

void msg2LCD(char *datos){

	while (*datos != 0){
		write_DATA(*datos);
		read_BUSY();
		datos++;
	}
}

// Esta rutina recibe un byte e imprime en el LCD
void char2LCD(unsigned char dato){

	write_DATA(dato);
	read_BUSY();
}

// Envía el comando de borrar display (cmd 0x01)
void clear_LCD(void){
	write_CMD(0x01);
	read_BUSY();
}

// Lleva el cursor a la primera posición del primer renglón
void ret_HOME(void){
	write_CMD(0x02);
	read_BUSY();
}

// Pone el cursor donde quiero (0x00 a 0x0F el 1er renglón, 0x40 a 0x4F el 2do)
void set_CURSOR(unsigned char posi){
	posi |= 0x80;
	write_CMD(posi);
	read_BUSY();
}

// Inicialización de display
void LCD_init(void){
        BUS_DIR = 0b00010000;   // especificamente para el shield 1.3

	LCD_tout = 30;          // Espera 30 interrup de timer
	while (LCD_tout);

	write_CMD(0x02);        // trabajar en 4bits 0010****
	LCD_tout = 1;		// Al menos 50uS 
	while (LCD_tout);	// Espera acá hasta que LCD_tout se hace 0

	write_CMD(0x28);	// 4 bits 2 lineas caracter 5*8
	LCD_tout = 1;
	while (LCD_tout);

	write_CMD(0x0C);        //disp ON Cursor OFF Blink OFF
	LCD_tout = 1;
	while (LCD_tout);

	write_CMD(0x01);        // borra el display
	LCD_tout = 1;
	while (LCD_tout);

	write_CMD(0x06);        //Incrementar la pos auto SCROLL OFF
	LCD_tout = 1;
	while (LCD_tout);

	read_BUSY();												// Esta rutina lee el bit de busy a ver si el LCD se liberó..

}
// Pone las señales correspondientes para enviar un dato a mostrar
void write_DATA(unsigned char dato){

	RS = TRUE;
	RW = FALSE;
	write_LCD(dato);	// Envía efectivamente el dato

}

// Pone las señales correspondientes para enviar un comando
void write_CMD(unsigned char dato){

	RS = FALSE;
	RW = FALSE;
	write_LCD(dato);	// Envía efectivamente el comando

}

// Pone un byte en el bus de a un nibble por vez
void write_LCD (unsigned char dato){
	unsigned char aux;

	DISPLAY &= 0xF0;	// Limpia bus
	E = TRUE;		// Pone en 1 la señal E

	aux = dato>>4;          // desplaza 4 bits a la derecha el dato
	DISPLAY |= aux;		// Pone el nibble alto en el bus

	E = FALSE;		// Pone en 0 la señal E

	DISPLAY &= 0xF0;        // Limpia bus
	E = TRUE;		// Pone en 1 la señal E

	aux = dato & 0x0F;
	DISPLAY |= aux;		// Pone el nibble bajo en el bus

	E = FALSE;		// Pone en 0 la señal E
}

// Lee el bit de busy (LCD terminó de procesar byte recibido..)
void read_BUSY(void){
    
        unsigned char aux;

	BUS_DIR |= ENTRADA;	// Pone el bus como entrada (hay que leer)
	RS = FALSE;		// Pone las señales para indicar que va a leer
	RW = TRUE;
	do{		 	// Hace
		E = TRUE;	// E=1
		aux = BUSY;	// Lee bit de busy
		E = FALSE;	// E=0
		E = TRUE;	// E=1
		(void)BUSY;	// Lectura dummy para completar el byte
		E = FALSE;	// E=0, completo byte
	}while (aux);		// Mientras busy = 1, cuando busy = 0 (LCD terminó), sale de este do-while
	RW = FALSE;		// Normaliza las señales
	BUS_DIR &= SALIDA;	// Hace el bus salida para seguir escribiendo al LCD
}

// Sólo decrementa una variable si esta está cargada con algún valor
void LCD_tic(void){
	 if (LCD_tout) LCD_tout--;
}