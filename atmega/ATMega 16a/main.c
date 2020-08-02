#include<avr/io.h>
#define	F_CPU	16000000UL
#include<util/delay.h>      //used for delay function
int main()
{
	
	uint16_t x;
	uint16_t y;
	
	//USART setup
	
	UCSRB = 0b00001000;    //enable transmitter 
	UCSRC = 0b10000110;    //8-bit data,no parity, 1 stop bit
	UBRRL = 0b00001000;    //11570 baud rate
	
	//ADC setup
	
	ADCSRA = 0x87; //make ADC enable and select ck/128 & ADC Enable
	ADMUX= 0x40;   //Reference Selection Bits AVCC with external capacitor at AREF pin and ADC 1
	
	while(1)                  //infinite loop
	{
		ADCSRA |= (1<<ADSC); //ADC Start Conversion ADSC bit
		while((ADCSRA && (1<<ADIF))==0);  //wait for ADC Interrupt Flag
		//ADCSRA |= (0<<ADSC);
		x = ADCL;
		y = ADCH;
		if(x == 0x48 || x == 0x4c){x--;}
		if(y == 0x48 || y == 0x4c){y--;}
		
		
		while((UCSRA & (1<<UDRE))==0); //is UDR empty by check UDRE to move again
		UDR = 'L' ;
		//while ( ! (UCSRA & (1<<UDRE)));
		while((UCSRA & (1<<UDRE))==0); //is UDR empty by check UDRE to move again 
		UDR = x ;  //move low byte to be sent
		
		while((UCSRA & (1<<UDRE))==0); //is UDR empty by check UDRE to move again
		UDR = 'H' ;
		
		while((UCSRA & (1<<UDRE))==0); //is UDR empty by check UDRE to move again
		//while ( ! (UCSRA & (1<<UDRE)));
		UDR = y ;  //move low byte to be sent
		//while((UCSRA && (1<<TXC))==1);//
		_delay_ms(5000);
	}
	return 0;
}