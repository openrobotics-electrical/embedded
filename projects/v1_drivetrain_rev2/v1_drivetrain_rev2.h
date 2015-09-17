#ifndef V1_DRIVETRAIN_REV2
#define V1_DRIVETRAIN_REV2

#define CLOCKWISE true
#define COUNTERCLOCKWISE false

uint8_t time_steps = 128;
int8_t M1_PWM, M2_PWM; // range of -time_steps to +time_steps
bool M1_direction, M2_direction;

// setup Motor 1 definitions and variables
#define M1A_PORT PORTG
#define M1A_DDR DDRG
#define M1A_PIN PING
#define M1B_PORT PORTL
#define M1B_DDR DDRL
#define M1B_PIN PINL
const uint8_t EN1A = _BV(0), EN1B = _BV(6), IN1A = _BV(2), IN1B = _BV(4);

// setup Motor 2 definitions and variables
#define M2A_PORT PORTL
#define M2A_DDR DDRL
#define M2A_PIN PINL
#define M2B_PORT PORTB
#define M2B_DDR DDRB
#define M2B_PIN PINB
const uint8_t EN2A = _BV(0), EN2B = _BV(2), IN2A = _BV(2), IN2B = _BV(0);

// PWM pins
#define PWM_PORT PORTH
#define PWM_DDR DDRH
const uint8_t PWM1 = _BV(3), PWM2 = _BV(4);

#endif