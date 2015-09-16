#ifndef V1_DRIVETRAIN_REV2
#define V1_DRIVETRAIN_REV2

// setup Motor 1 definitions and variables
#define M1A_PORT PORTG
#define M1A_DDR DDRG
#define M1A_PIN PING
#define M1B_PORT PORTL
#define M1B_DDR DDRL
#define M1B_PIN PINL
const uint8_t EN1A = _BV(0), EN1B = _BV(6), IN1A = _BV(2), IN1B = _BV(4);
float M1_output; // range -1 to +1

// setup Motor 2 definitions and variables
#define M2A_PORT PORTL
#define M2A_DDR DDRL
#define M2A_PIN PINL
#define M2B_PORT PORTB
#define M2B_DDR DDRB
#define M2B_PIN PINB
const uint8_t EN2A = _BV(0), EN2B = _BV(2), IN2A = _BV(2), IN2B = _BV(0);
float M2_output; // range -1 to +1

#endif