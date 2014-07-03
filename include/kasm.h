/*********************************************
kasm.h

************************************************/

#include "defs.h"

unsigned int    _read_msw();

void            _lidt (IDTR *idtr);

void		_mascaraPIC1 (byte mascara);  /* Escribe mascara de PIC1 */
void		_mascaraPIC2 (byte mascara);  /* Escribe mascara de PIC2 */

void		_Cli(void);        /* Deshabilita interrupciones  */
void		_Sti(void);	 /* Habilita interrupciones  */

void		_int_08_hand();      /* Timer tick Handler*/
void		_int_09_hand();      /* Keyboard Handler*/
void		_int_74_hand();      /* Mouse Handler*/

void		_rtc_hand_read();
void		_rtc_hand_write(int hour, int h_m_s);

void 		_rtc_hand_save_prev_time();
void 		_rtc_hand_save_current_time();

void		_setmouse();
byte		inportb(byte b); /* Mouse */
byte		outportb(byte b, byte a); /* Mouse */	

void		_debug (void);

