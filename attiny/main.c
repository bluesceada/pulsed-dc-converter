/*
 * by Dennis Gnad <dennis@xorn.org>
 * Initial Version: 2021-11-29
 *
 * Yes, I know, the attiny45 is overpowered for this,
 * but it was in my parts drawer ¯\_(ツ)_/¯
 * You can just use the attiny25, software should fit.
 * You might also be able to adjust it for an assembler-only attiny13.
 *
 * For the timer, please use the calibrated internal
 * RC oscillator at 8 MHz (-> 4 MHz with software-set divider)
 * Please also set F_CPU correctly in the Makefile
 *
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h> /* for _delay_ms() */
#include <stdlib.h> /* also contains pseudo random numbers */
#include <stdint.h>

// Previously used simple loop used for measurements on breadboard
void UNUSED_software_only_loop() {
  // 20ms Period 120 Hz / 60 Hz
  const double HIGH_TIME_MS = 8.8;
  const double LOW_TIME_MS = 0.2;
  PORTB &= ~0x1; // reset PB0 to 0 (0V) (Transistor Gate)
  _delay_ms(LOW_TIME_MS);
  PORTB |= 0x1; // set PB0 to 1 (5V) (Transistor Gate)
  _delay_ms(HIGH_TIME_MS);
}

int main(void) {

  // scale all clocks by /2 --> 4 MHz now (with Int. RC)
  CLKPR=(1<<CLKPCE);
  CLKPR=(1<<CLKPS0);

  // PB0 pin as transistor gate output, PB3 as debug output
  PORTB &= ~(0x1 | 0x8); // pull-downs
  DDRB |= (0x1 | 0x8); // PB0 + PB3 output direction (low-Z)
  // PB2 is the input (for ADC)

  // ---- Timer Config ---- //
  // See Table 11-5 in datasheet attiny25/45/85 for various modes
  TCCR0A|=(1<<WGM01)|(1<<WGM00);
  TCCR0B|=(1<<WGM02);
  
  // enable overflow and compare B interrupts
  // (because compare A is the same as overflow in the used mode)
  TIMSK|=(1<<TOIE0)|(1<<OCIE0B);
  
  // 4 MHz / 256 => 64us time steps
  //  -> 1 / 120Hz / 64us = 130
  //     but because my internal RC wasn't calibrated correctly, I needed to use 134
  OCR0B=16; // runs ISR(TIMER0_COMPB_vect) at this time
  OCR0A=134; // resets Timer and runs ISR(TIMER0_OVF_vect) (because of the mode)
  
  //TCCR0B|=(1<<CS00); // no prescaling / use direct clk_I/O
  //TCCR0B|=(1<<CS01)|(1<<CS00); // prescale clk_I/O / 64
  TCCR0B|=(1<<CS02); // prescale clk_I/O / 256
  sei(); //enabling global interrupt
  // ---- END: Timer Config ---- //
  
  // ---- ADC Config ---- //
  // default: use Vcc as ref (5V)
  ADMUX|=(1<<MUX0); // PB2 input
  ADMUX|=(1<<ADLAR); // we will later only use the high 8 bits in ADCH and discard the 2 LSBs
  ADCSRA|=(1<<ADEN);//|(1<<ADATE);
  ADCSRA|=(1<<ADPS1); // ADC prescaler / 4 == 1 MHz
  //ADCSRA|=(1<<ADPS1)|(1<<ADPS0); // ADC prescaler / 8 == 500 kHz
  // ---- END: ADC Config ---- //

  while(1) {
    // interrupt-only operation
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_mode();
    //software_only_loop();
  }
  return 0;
}

ISR(TIMER0_COMPB_vect) {
  // fallback to enable transistor again now -- however, it might be
  // already enabled by ADC threshold, but anyway make sure ADC is disabled
  PORTB |= 0x1;
  // disable ADC and its interrupt
  ADCSRA &= ~((1<<ADIE)|(1<<ADEN));
}

ISR(TIMER0_OVF_vect) {
  // enable ADC, start conversion, interrupt enable:
  ADCSRA|=(1<<ADEN)|(1<<ADSC)|(1<<ADIE); 
  
  // disable the transistor every timer overflow
  // we will re-enable after a specific time, or
  // at a specific ADC threshold -- whatever comes first
  // here is a minor delay to align a bit more with ADC conversions
  _delay_ms(0.01);
  PORTB &= ~0x1;
}

ISR(ADC_vect) {
  // Check the ADC value and compare it against threshold:
  // the value is divided by about 10 with a voltage divider
  uint8_t Transistor_Drain_V = ADCH;
  // 255 == 5V <-> measured = 50V
  // 255/50 = 5.1 ~= 1V
  if (Transistor_Drain_V < 6) { // == if less than 1.2V
    ADCSRA &= ~((1<<ADIE)|(1<<ADEN)); // disable ADC and its interrupt for a while
    //TODO: test and measure it first with debug port, then try if it works with transistor
    //PORTB |= 0x1; // enable transistor again -> get more max. power from power banks
    PORTB |= 0x8; // debug
    // recommended ADC input resistance: 1..100k, or measurements would be slow
    
  } else {
     // continue checking ADC conversions ....
    PORTB ^= 0x8; // debug
  }
}

