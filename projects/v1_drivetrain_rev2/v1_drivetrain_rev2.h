#ifndef V1_DRIVETRAIN_REV2
#define V1_DRIVETRAIN_REV2

uint8_t time_steps = 16;
int8_t M1_output, M2_output; // range of -time_steps to +time_steps

// setup Motor 1 definitions and variables
#define M1A_PORT PORTG
#define M1A_DDR DDRG
#define M1A_PIN PING
#define M1B_PORT PORTL
#define M1B_DDR DDRL
#define M1B_PIN PINL
#define M1PWM_PORT PORTH
const uint8_t EN1A = _BV(0), EN1B = _BV(6), IN1A = _BV(2), IN1B = _BV(4);

// setup Motor 2 definitions and variables
#define M2A_PORT PORTL
#define M2A_DDR DDRL
#define M2A_PIN PINL
#define M2B_PORT PORTB
#define M2B_DDR DDRB
#define M2B_PIN PINB

// PWM pins
#define PWM_PORT PORTH
#define PWM_DDR DDRH
const uint8_t PWM1 = _BV(3), PWM2 = _BV(4);

const uint8_t EN2A = _BV(0), EN2B = _BV(2), IN2A = _BV(2), IN2B = _BV(0);

#endif