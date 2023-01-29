/*
 * Motorsteuerung-test.c
 *
 * Created: 06/12/2022 10:37:38
 * Author : joelr
 * eventuell noch Reset über Taste realisieren (mit Watchdog-Timer???)
 * KeyPad: Weiß an PB0
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define DOWN (0x40)
#define UP (0x20)
#define BRAKE (0x60)
#define FREE (0x00)

#define Stock0 0	//Höhe des 0. Stockes in Drehimpulsticks
#define Stock1 31	//Höhe des 1. Stockes in Drehimpulsticks
#define Stock2 60	//Höhe des 2. Stockes in Drehimpulsticks

#define HallKanalUnten 4	//Eingangskanal für den unteren Hallsensor

volatile unsigned char direction=0;	//Bit für die Richtung, 2=Aufwärts/1=Abwärts/0=Stillstand
volatile unsigned int heightticks=1000;	//Zählvariable für die Höhe, bzw. die Umdrehungen des Motors
volatile unsigned char stockwerk=0;	//Variable für das aktuelle Stockwerk

unsigned char read_ADC8(unsigned char kanal)
{
	unsigned char wert = 0;
	DDRA &= ~(1<<kanal);    //übergebener Kanal auf Eingang
	ADCSRA &= ~(1<<ADATE); //Einzelwandlung
	ADCSRA |= (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2);        //Hardwareteiler auf 128 setzen = 93,75kHz
	ADCSRA |= (1<<ADEN);    //ADC-Wandler einschalten
	ADMUX |= (1<<ADLAR);    //Wandlungsergebniss linksbündig
	ADMUX &= (~(1<<MUX3) & (~(1<<MUX4)));    //MUX-Kanal einstellen: unipolar
	ADMUX &= ~(1<<MUX0) & ~(1<<MUX1) & ~(1<<MUX2); //ADMUX kanäle rücksetzen
	ADMUX |= kanal;        // ADMUX mit kanal verodern, somit die kanäle setzen
	ADMUX |= (1<<REFS0);     // Referenz auf 5V
	ADCSRA |= (1<<ADSC);    //Wandlung starten
	while (ADCSRA & (1<<ADSC)){;};        //Warten, solange Wandlung im Gange ist
	wert = ADCH;                     //Wert ausgeben
	ADCSRA &= ~(1<<ADEN);    //Wandler ausschalten
	return wert;            //ADC-Wert übergeben
}

unsigned char hallsensor(){		//Gibt "1" zurück wenn der Lift am unteren Hallsensor steht, und "2" beim oberen Hallsensor, und "0" falls weder noch
	//Unterer Hall Sensor
	if (read_ADC8(HallKanalUnten)<=30)	{return 1;}		//Wenn der untere Hallsensor ausgelöst hat
	
	//Oberer Hall Sensor
	//if (read_ADC8(HallKanalOben)<=50)	{return 2;}		//Wenn der obere Hallsensor ausgelöst hat
	
	else if ((read_ADC8(HallKanalUnten)>=100))	{return 0;}	//Wenn beide nicht ausgelöst haben
	return 0;	//Fehlerhafte Messung
}

ISR(INT1_vect)		//Wird bei fallender Flanke an PD3 ausgeführt
{
	switch(heightticks){		//Stockwerk nach den Heightticks setzen
		case Stock1: stockwerk=1; break;
		case Stock2: stockwerk=2; break;
		default: break;
	}
	if (hallsensor()==1)
	{
		stockwerk=0;
		heightticks=0;
	}
	
	if (direction==2)	//Falls nach oben gefahren wird
	{
		heightticks++;	//Zählvariable erhöhen
	}
	if (direction==1)	//Falls nach unten gefahren wird
	{
		heightticks--;	//Zahlvariable decreasen
	}
	//_delay_ms(10);		//Entprellen
}

ISR(TIMER0_OVF_vect){		//Wird beim Overflow des Timers aufgerufen
	if (direction==2){		//Wenn aufwärts gefahren wird
		PORTC&=~(DOWN);	//Bitweise invertieren um zwischen Brems und Aufwärtsfahren zu wechseln
	}
	if (direction==1){		//Wenn abwärts gefahren wird
		PORTC&=~(UP);	//Bitweise invertieren um zwischen Brems und Aufwärtsfahren zu wechseln
	}
	if (direction==0){		//Wenn der Lift steht
		PORTC|=BRAKE;			//Bremsen
	}
}

ISR(TIMER0_COMP_vect){		//Wird aufgerufen wenn der Timer OCR0 erreicht
	if (direction==2){		//Wenn aufwärts gefahren wird
		PORTC|=(DOWN);	//Bitweise invertieren um zwischen Brems und Aufwärtsfahren zu wechseln
	}
	if (direction==1){		//Wenn abwärts gefahren wird
		PORTC|=(UP);	//Bitweise invertieren um zwischen Brems und Aufwärtsfahren zu wechseln
	}
	if (direction==0){		//Wenn der Lift steht
		PORTC|=BRAKE;			//Bremsen
	}
}



void AufzugFahren(unsigned char zielstockwerk){
	
	switch(heightticks){		//Stockwerk nach den Heightticks setzen
		case Stock0: stockwerk=0; break;
		case Stock1: stockwerk=1; break;
		case Stock2: stockwerk=2; break;
		default: break;
	}
	
	//if (stockwerk>zielstockwerk){
		//heightticks = heightticks - 2;
	//}
	//if (zielstockwerk==2&&stockwerk==1)
	//{
		//heightticks = heightticks+2;
	//}
	
	while (stockwerk>zielstockwerk)	//Wenn das aktuelle Stockwerk über dem Zielstockwerk liegt e.g. Aktuell: 1 Ziel:0
	{
		direction=1;
		OCR0=80;
	}
	while (stockwerk<zielstockwerk)	//Wenn das aktuelle Stockwerk unter dem Zielstockwerk liegt
	{
		direction=2;
		OCR0=220;
	}
	
	direction=0;
}
//Gibt beim gedrücktem Taster die Nummer zurück. Falls keiner gedrückt ist "3"
unsigned char tasterabfrage(){ 
	
	if (!(PINA&0x01)) {			//Taster S0		Erdgeschoss anfahren
		return 0;
	}
	else if (!(PINA&0x02)) {	//Taster S1		1. Stock anfahren
		return 1;
	}
	else if (!(PINA&0x04)){		//Taster S2		2. Stock anfahren
		return 2;
	}
	else if (!(PINA&0x08)){		//Taster S3
	}
	return 3;
}
int main(void)
{
	//IOs konfigurieren
	#pragma region
		DDRC=0xff;		//C-Register auf Ausgang setzen (LEDs, Motortreiber)
		PORTC=0x00;		//Alle Ausgänge im C-Register LOW setzen
		DDRA=0x00;		//A-Register auf Eingang setzen (Taster)
		PORTA=0x07;		//Pullups für S0...S4 einschalten
		DDRD=0x00;				//DDR-D auf Eingang
		PORTD=(1<<PD3);			//Pullup für INT1-Pin
	#pragma endregion
	
	//Interrupt 0 konfigurieren
	#pragma region Interrupt-Config
	GICR|=(1<<INT1);		//INT1 freigeben
	MCUCR|=(1<<ISC11);		//INT1 falling edge
	MCUCR&=~(1<<ISC10);		//INT1 falling edge
	#pragma endregion Interrupt-Config
	
	//Timer0 konfigurieren
	#pragma region Timer0-Config
	TCCR0|=(1<<CS02)|(1<<CS00);		//Timer0 Systemtakt/1024
	TIMSK|=(1<<TOIE0)|(1<<OCIE0);		//Overflow und Match Interrupt freigeben
	TCCR0|=(1<<WGM01)|(1<<WGM00);		//FastPWM
	OCR0=230;
	#pragma endregion Timer0-Config
	
	sei();					//Interrupt global freigeben
	
	//Am Start ins Erdgeschoss fahren, da der Anfangszustand sonst unbekannt ist
	while (hallsensor()!=1)
	{
		direction=2;
	}
	direction=0;
	heightticks=0;		//Heightticks nullen, da der Aufzug jetzt sicher im Erdgeschoss steht
	
    while (1) //Arbeitsschleife
    {
		switch(tasterabfrage()){		//Stockwerk nach den Heightticks setzen
			case 0: AufzugFahren(0); break;
			case 1: AufzugFahren(1); break;
			case 2: AufzugFahren(2); break;
			case 3: break;
			default: break;
		}
    }
}



