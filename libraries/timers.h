#ifndef TIMERS_H 
#define TIMERS_H

namespace timers {
	
enum Prescaler {
	PRESCALER_1 = 1U,
	PRESCALER_8 = 2U,
	PRESCALER_64 = 3U,
	PRESCALER_256 = 4U,
	PRESCALER_1024 = 5U,
	PRESCALER_MASK = 7U
};

const uint16_t value_map[] = {0, 1, 8, 64, 256, 0, 1024, 0};

class GenericTimer {
protected:
	Prescaler prescale;
	GenericTimer(Prescaler pre) : prescale(pre) {}
public:
	virtual void inline start() = 0;
	virtual void inline stop() = 0;
	virtual bool inline is_running() = 0;	
	virtual void inline clear() = 0;
	uint16_t prescaler() { return value_map[static_cast<uint8_t>(prescale)]; }
};

class Timer8bit : public GenericTimer {
protected:
	Timer8bit(Prescaler pre) : GenericTimer(pre) {}
		
public:
	virtual uint8_t inline count() = 0;
	
	void inline start() { TCCR0B |= prescale; }
	void inline stop() { TCCR0B &= ~PRESCALER_MASK; }
	bool inline is_running() { return TCCR0B &= ~PRESCALER_MASK; }
	void inline set_output_compare_A(uint8_t oc_A) { OCR0A = oc_A; };
	void inline set_output_compare_B(uint8_t oc_B) { OCR0B = oc_B; };
};

class Timer16bit : public GenericTimer {
protected:
	Timer16bit(Prescaler pre, volatile uint8_t *const base) 
			: GenericTimer(pre), base_address(base) {}
	
	volatile uint8_t *const base_address; // Child classes must initialize this

public:
	virtual uint16_t count() = 0;

	void inline start() { *(base_address-1) |= prescale; }
	void inline stop() { *(base_address-1) &= ~PRESCALER_MASK; }
	bool inline is_running() { return *(base_address-1) & PRESCALER_MASK; }
	// TODO implement
	void inline set_output_compare_A(uint16_t oc_A) { *(base_address-6) = oc_A; };
	// TODO implement
	void inline set_output_compare_B(uint16_t oc_B) { *(base_address-8) = oc_B; };
};

class Timer0 : public Timer8bit {
public:
	Timer0(Prescaler pre) : Timer8bit(pre) {
		TCCR0A = (1<<COM0A1) | (1<<COM0B1) | (1<<WGM00);
		TCCR0B = 0;//(1<<WGM02);
	}
	uint8_t count() { return TCNT0; }
	void clear() { TCNT0 = 0; }
};

class Timer1 : public Timer16bit {
public:
	Timer1(Prescaler pre) : Timer16bit(pre, &TCCR1A) {
		TCCR1A = (1<<COM1B1) | (1<<COM1A1) | (1<<WGM11);
		TCCR1B = (1<<WGM13) | (1<<WGM12);
	}
	uint16_t count() { return TCNT1; }
	void clear() { TCNT1 = 0; }
};

class Timer2 : public Timer16bit {
public:
	Timer2(Prescaler pre) : Timer16bit(pre, &TCCR2A) {
		TCCR2A = (1<<COM1B1) | (1<<COM1A1) | (1<<WGM11);
		TCCR2B = (1<<WGM13) | (1<<WGM12);		
	}
	uint16_t count() { return TCNT2; }
	void clear() { TCNT2 = 0; }
};

} /* End of namespace timers */
#endif /* TIMERS_H */