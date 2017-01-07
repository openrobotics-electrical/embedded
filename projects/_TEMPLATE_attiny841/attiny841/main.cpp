/*
 * attiny841.cpp
 *
 * Created: 12/27/2016 2:35:39 AM
 * Author : Maxim
 */ 

#include <opbots.h>
using namespace opbots;

Output txden_485	(&PORTA, PORTA3);
Output tx_485		(&PORTA, PORTA1); // Used by USART0
Input  rx_485		(&PORTA, PORTA2); // Used by USART0
Output txden_ttl	(&PORTA, PORTA7);
Output tx_ttl		(&PORTA, PORTA5); // Used by USART1
Input  rx_ttl		(&PORTA, PORTA4); // Used by USART1
Input  s1			(&PORTA, PORTA0);
Input  s2			(&PORTB, PORTB2);

int main(void) {
	Serial::init(9600,0);
	Serial::set_txden_pin(txden_485,0);
    Serial::init(9600,1);
	Serial::set_txden_pin(txden_ttl,1);
	txden_485.clear();
	txden_ttl.clear();
	char msg_485[] = "Hello from uart0 on rs485 bus\r\n";
	char msg_ttl[] = "Hello from uart1 on ttl bus\r\n";
	SimpleBuffer<char> test(16);
	sei();
	bool rx_ready;
	char rx;
	while (1) {
		rx = Serial::get_char(&rx_ready);
		if (rx_ready) 
			test.push(rx);
		test.pop_into(&(*msg_ttl));
		Serial::transmit(msg_485,sizeof(msg_485),0);
		_delay_ms(1000);
		Serial::transmit(msg_ttl,sizeof(msg_ttl),1);
		_delay_ms(1000);
    }
}

