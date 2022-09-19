#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
unsigned char secondFlag=0;
unsigned char second=0;
unsigned char minute=0;
unsigned char hour=0;
// Interrupt Service Routine for timer1 compare mode channel A
ISR(TIMER1_COMPA_vect){
	secondFlag=1;
}

void timer1_ctc_init(void){
	TCNT1=0;//SET timer1 intial count to 0
	OCR1A=1000;//as in CTC mode(mode 4) the top is OCR1A where 1000 is EQV to 1 sec
	TCCR1A=(1<<FOC1A)|(1<<FOC1B);//The FOC1A/FOC1B bits are only active when the WGM13:0 bits specifies a non-PWM mode.
	//Clock = F_CPU/1024 CS10=1 CS11=0 CS12=1 And WGM12 MODE 4 in datasheet (CTC)
	TCCR1B=(1<<WGM12)|(1<<CS10)|(1<<CS12);
	TIMSK|=(1<<OCIE1A);/*
	When this bit is written to one, and the I-flag in the Status Register
	is set (interrupts globally enabled), the Timer/Counter1 Output Compare A match interrupt is enable
	*/

}
// Interrupt Service Routine for INT0 to reset the stop watch
ISR(INT0_vect){
	//reset the seconds,minutes and Hours count
	second=0;
	minute=0;
	hour=0;
}
void int0_init(void){
	DDRD&=~(1<<PD2);//Make pin PD2 as input
	PORTD|=(1<<PD2);//enable internal pull up resistance
	GICR|=(1<<INT0);//enable external interrupt 0,we will enable I-bit in the main function
	MCUCR|=(1<<ISC01);//The falling edge of interrupt 0 generates an interrupt request
	MCUCR&=~(1<<ISC00);
}
// Interrupt Service Routine for INT1 to pause the stop watch
ISR(INT1_vect){
	//to pause stop watch we will just disable clock using REG TCCR1B and reset CS10 CS11 CS12
	TCCR1B&=~(1<<CS10)&~(1<<CS11)&~(1<<CS12);
}
void int1_init(void){
	DDRD&=~(1<<PD3);//Make pin PD3 as input
	MCUCR|=(1<<ISC10)|(1<<ISC11);//The rising edge of interrupt 1 generates an interrupt request
	//we will use an external pull down resistance
	GICR|=(1<<INT1);//enable external interrupt 1,we will enable I-bit in the main function

}
// Interrupt Service Routine for INT2 to resume the stop watch
ISR(INT2_vect){
	//to resume stop watch we will just enable clock using REG TCCR1B and set CS10  CS12
	TCCR1B|=(1<<CS10)|(1<<CS12);
}
void int2_init(void){
	DDRB&=~(1<<PB2);//Make pin PB2 as input
	PORTB|=(1<<PB2);//enable internal pull up resistance
	GICR|=(1<<INT2);//enable external interrupt 2,we will enable I-bit in the main function
	MCUCSR&=~(1<<ISC2);//The falling edge of interrupt 2 generates an interrupt request
}
int main(void){
	SREG|=(1<<7);//enable I-bit
	DDRA|=(0x3F);//make first 6 bits in PORTA O/P PINS
	PORTA|=(0x3F);
	DDRC|=(0x0F);
	PORTC|=(0xF0);//display zero at the start
	//actvate Timer1,INT0,INT1,INT2
	timer1_ctc_init();
	int0_init();
	int1_init();
	int2_init();
	//main flow of the program
	while(1){
		if(secondFlag==1){//interrupt Timer 1 occured one sec passed
			//increment seconds by 1
			second++;
			if(second==60){//1 minute passed
				second=0;
				minute++;
			}
			if(minute==60){//1 hour passed
				minute=0;
				hour++;
			}
			if(hour==24){//1 day passed
				hour=0;
			}
			//reset the second flag to be set again to 1 in the next second
			secondFlag=0;
		}else{
			//display on 7 segments
			//sec 1st 7segment
			//enable 7 segment then write the number
			PORTA =(PORTA&(0xC0))|(1<<PA0);
			PORTC=(PORTC&(0xF0))|((second%10));//8%10=8
			_delay_ms(6);
			//display on 7 segments
			//sec 2nd 7segment
			//enable 7 segment then write the number
			PORTA =(PORTA&(0xC0))|(1<<PA1);
			PORTC=(PORTC&(0xF0))|((second/10));//28/10=2.8=2
			_delay_ms(6);
			//display on 7 segments
			//min 1st 7segment
			//enable 7 segment then write the number
			PORTA =(PORTA&(0xC0))|(1<<PA2);
			PORTC=(PORTC&(0xF0))|((minute%10));
			_delay_ms(6);
			//display on 7 segments
			//min 2nd 7segment
			//enable 7 segment then write the number
			PORTA =(PORTA&(0xC0))|(1<<PA3);
			PORTC=(PORTC&(0xF0))|((minute/10));
			_delay_ms(6);
			//display on 7 segments
			//hour 1st 7segment
			//enable 7 segment then write the number
			PORTA =(PORTA&(0xC0))|(1<<PA4);
			PORTC=(PORTC&(0xF0))|((hour%10));
			_delay_ms(6);
			//display on 7 segments
			//hour 2nd 7segment
			//enable 7 segment then write the number
			PORTA =(PORTA&(0xC0))|(1<<PA5);
			PORTC=(PORTC&(0xF0))|((hour/10));
			_delay_ms(6);
		}
	}
}
