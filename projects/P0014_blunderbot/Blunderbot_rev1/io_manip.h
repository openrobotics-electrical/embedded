#ifndef IO_MANIP_H
#define IO_MANIP_H

#include "new_and_delete.h"

namespace io_manip {
	
class GPIO {
protected:
	volatile uint8_t *const port_out, *const port_dir, *const port_in;
	const uint8_t pin;
	const uint8_t pin_mask;
	
	GPIO(volatile uint8_t *const io_port, const uint8_t io_pin) :
		port_out(io_port), port_dir(io_port-1U), port_in(io_port-2U),
		pin(io_pin), pin_mask(1<<io_pin) {}
	virtual ~GPIO() {}
	
public:	
	bool inline is_set() { return *port_in & pin_mask; }
};

class Output : public GPIO {
public:
	Output(volatile uint8_t *const io_port, const uint8_t io_pin) 
			: GPIO(io_port, io_pin) {
		// Set output low
		*port_out &= ~pin_mask;
		// Set pin in port to output
		*port_dir |= pin_mask;
	};
	~Output() {}
		
	void inline toggle() { *port_out ^= pin_mask; }
	void inline set() { *port_out |= pin_mask; }
	void inline clear() { *port_out &= ~pin_mask; }
};

class Input : public GPIO {
public:
	Input(volatile uint8_t *const io_port, const uint8_t io_pin)
			: GPIO(io_port, io_pin) {
		// Set pin in port to input;
		*port_dir &= ~pin_mask;
	};
	~Input() {}
};

} /* End of namespace io_manip */
#endif /* IO_MANIP_H */