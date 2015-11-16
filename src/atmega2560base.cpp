 /*
 ****************************************************************************
 *
 * simulavr - A simulator for the Atmel AVR family of microcontrollers.
 * Copyright (C) 2001, 2002, 2003   Klaus Rudolph       
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 ****************************************************************************
 */
#include "atmega2560base.h"

#include "irqsystem.h"
#include "hwstack.h"
#include "hweeprom.h"
#include "hwwado.h"
#include "hwsreg.h"
#include "avrerror.h"
#include "avrfactory.h"

AVR_REGISTER(atmega640, AvrDevice_atmega640)
AVR_REGISTER(atmega1280, AvrDevice_atmega1280)
AVR_REGISTER(atmega2560, AvrDevice_atmega2560)

AvrDevice_atmega2560base::~AvrDevice_atmega2560base() {
    delete usart1;
    delete usart0;
    delete wado;
    delete spi;
    delete acomp;
    delete ad;
    delete aref;
    delete admux;
    delete gpior2_reg;
    delete gpior1_reg;
    delete gpior0_reg;
    delete timer5;
    delete inputCapture5;
    delete timerIrq5;
    delete timer4;
    delete inputCapture4;
    delete timerIrq4;
    delete timer3;
    delete inputCapture3;
    delete timerIrq3;
    delete timer2;
    delete timerIrq2;
    delete timer1;
    delete inputCapture1;
    delete timerIrq1;
    delete timer0;
    delete timerIrq0;
    delete extirqpc;
    delete pcmsk2_reg;
    delete pcmsk1_reg;
    delete pcmsk0_reg;
    delete pcifr_reg;
    delete pcicr_reg;
    delete extirq;
    delete eifr_reg;
    delete eimsk_reg;
    delete eicra_reg;
    delete eicrb_reg;
    delete osccal_reg;
    delete clkpr_reg;
    delete stack;
    delete eeprom;
    delete irqSystem;
}

AvrDevice_atmega2560base::AvrDevice_atmega2560base(unsigned ram_bytes,
                                                     unsigned flash_bytes,
                                                     unsigned ee_bytes ):
    AvrDevice(0x200 - 0x60, // I/O space size (above ALU registers) //TODO 64 in standard io memory following the registers
	      //  The first 32 locations address the Register file, the next 64 location the standard
	      //  I/O Memory, then 416 locations of Extended I/O memory and the next 8,192 locations address the internal data
	      //  SRAM.
              ram_bytes,    // RAM size
              0xDE00,       // External RAM size
              flash_bytes), // Flash Size
    porta(this, "A", true),
    portb(this, "B", true),
    portc(this, "C", true),
    portd(this, "D", true),
    porte(this, "E", true),
    portf(this, "F", true),
    portg(this, "G", true),
    porth(this, "H", true),
    portj(this, "J", true),
    portk(this, "K", true),
    portl(this, "L", true),
    gtccr_reg(&coreTraceGroup, "GTCCR"),
    assr_reg(&coreTraceGroup, "ASSR"),
    prescaler01(this, "01", &gtccr_reg, 0, 7),
    prescaler2(this, "2", PinAtPort(&portb, 6), &assr_reg, 5, &gtccr_reg, 1, 7)
{ 
    flagELPMInstructions = true;
    fuses->SetFuseConfiguration(19, 0xff9962);

    irqSystem = new HWIrqSystem(this, 4, 31);

    eeprom = new HWEeprom(this, irqSystem, ee_bytes, 25, HWEeprom::DEVMODE_EXTENDED); 
    stack = new HWStackSram(this, 16);
    clkpr_reg = new CLKPRRegister(this, &coreTraceGroup);
    osccal_reg = new OSCCALRegister(this, &coreTraceGroup, OSCCALRegister::OSCCAL_V5);

    rampz = new AddressExtensionRegister(this, "RAMPZ", 1);
    eind = new AddressExtensionRegister(this, "EIND", 1);

    eicra_reg = new IOSpecialReg(&coreTraceGroup, "EICRA");
    eicrb_reg = new IOSpecialReg(&coreTraceGroup, "EICRB");
    eimsk_reg = new IOSpecialReg(&coreTraceGroup, "EIMSK");
    eifr_reg = new IOSpecialReg(&coreTraceGroup, "EIFR");
    extirq = new ExternalIRQHandler(this, irqSystem, eimsk_reg, eifr_reg);
    extirq->registerIrq(1, 0, new ExternalIRQSingle(eicra_reg, 0, 2, GetPin("D2")));
    extirq->registerIrq(2, 1, new ExternalIRQSingle(eicra_reg, 2, 2, GetPin("D3")));
    extirq->registerIrq(3, 2, new ExternalIRQSingle(eicra_reg, 4, 2, GetPin("B2")));
    extirq->registerIrq(4, 3, new ExternalIRQSingle(eicra_reg, 6, 2, GetPin("XX")));
    extirq->registerIrq(5, 4, new ExternalIRQSingle(eicrb_reg, 0, 2, GetPin("XX")));
    extirq->registerIrq(6, 5, new ExternalIRQSingle(eicrb_reg, 2, 2, GetPin("XX")));
    extirq->registerIrq(7, 6, new ExternalIRQSingle(eicrb_reg, 4, 2, GetPin("XX")));
    extirq->registerIrq(8, 7, new ExternalIRQSingle(eicrb_reg, 6, 2, GetPin("XX")));

    pcicr_reg = new IOSpecialReg(&coreTraceGroup, "PCICR");
    pcifr_reg = new IOSpecialReg(&coreTraceGroup, "PCIFR");
    pcmsk0_reg = new IOSpecialReg(&coreTraceGroup, "PCMSK0");
    pcmsk1_reg = new IOSpecialReg(&coreTraceGroup, "PCMSK1");
    pcmsk2_reg = new IOSpecialReg(&coreTraceGroup, "PCMSK2");
    extirqpc = new ExternalIRQHandler(this, irqSystem, pcicr_reg, pcifr_reg);
    extirqpc->registerIrq(9, 0, new ExternalIRQPort(pcmsk0_reg, &porta)); //TODO ctae is this port based?
    extirqpc->registerIrq(10, 1, new ExternalIRQPort(pcmsk1_reg, &portb)); //TODO ctae is this port based?
    extirqpc->registerIrq(11, 2, new ExternalIRQPort(pcmsk2_reg, &portc)); //TODO ctae is this port based?

    timerIrq0 = new TimerIRQRegister(this, irqSystem, 0);
    timerIrq0->registerLine(0, new IRQLine("TOV0",  23));
    timerIrq0->registerLine(1, new IRQLine("OCF0A", 21));
    timerIrq0->registerLine(2, new IRQLine("OCF0B", 22));

    timer0 = new HWTimer8_2C(this,
                             new PrescalerMultiplexerExt(&prescaler01, PinAtPort(&portd, 4)), //todo ctae check pin
                             0,
                             timerIrq0->getLine("TOV0"),
                             timerIrq0->getLine("OCF0A"),
                             new PinAtPort(&portb, 3), //todo ctae check pin
                             timerIrq0->getLine("OCF0B"),
                             new PinAtPort(&portb, 4)); //todo ctae check pin

    timerIrq1 = new TimerIRQRegister(this, irqSystem, 1);
    timerIrq1->registerLine(0, new IRQLine("TOV1",  20));
    timerIrq1->registerLine(1, new IRQLine("OCF1A", 17));
    timerIrq1->registerLine(2, new IRQLine("OCF1B", 18));
    timerIrq1->registerLine(3, new IRQLine("OCF1C", 19));
    timerIrq1->registerLine(5, new IRQLine("ICF1",  16)); //todo ctae ICF1 is capture event??

    inputCapture1 = new ICaptureSource(PinAtPort(&portb, 0)); //todo ctae check pin
    timer1 = new HWTimer16_3C(this,
			      new PrescalerMultiplexerExt(&prescaler01, PinAtPort(&portd, 5)), //todo ctae check pin
			      1,
			      timerIrq1->getLine("TOV1"),
			      timerIrq1->getLine("OCF1A"),
			      new PinAtPort(&portd, 5), //todo ctae check pin
			      timerIrq1->getLine("OCF1B"),
			      new PinAtPort(&portd, 4), //todo ctae check pin
			      timerIrq1->getLine("OCF1C"),
			      new PinAtPort(&portd, 4), //todo ctae check pin
			      timerIrq1->getLine("ICF1"),
			      inputCapture1);

    timerIrq2 = new TimerIRQRegister(this, irqSystem, 2);
    timerIrq2->registerLine(0, new IRQLine("TOV2",  15));
    timerIrq2->registerLine(1, new IRQLine("OCF2A", 13));
    timerIrq2->registerLine(2, new IRQLine("OCF2B", 14));

    timer2 = new HWTimer8_2C(this,
                             new PrescalerMultiplexer(&prescaler2),
                             2,
                             timerIrq2->getLine("TOV2"),
                             timerIrq2->getLine("OCF2A"),
                             new PinAtPort(&portd, 7),
                             timerIrq2->getLine("OCF2B"),
                             new PinAtPort(&portd, 6));

    timerIrq3 = new TimerIRQRegister(this, irqSystem, 3);
    timerIrq3->registerLine(0, new IRQLine("TOV1",  35));
    timerIrq3->registerLine(1, new IRQLine("OCF1A", 32));
    timerIrq3->registerLine(2, new IRQLine("OCF1B", 33));
    timerIrq3->registerLine(3, new IRQLine("OCF1C", 34));
    timerIrq3->registerLine(5, new IRQLine("ICF1",  31)); //todo ctae ICF1 is capture event??

    inputCapture3 = new ICaptureSource(PinAtPort(&portb, 0)); //todo ctae check pin
    timer3 = new HWTimer16_3C(this,
			      new PrescalerMultiplexerExt(&prescaler01, PinAtPort(&portd, 5)), //todo ctae check pin
			      3,
			      timerIrq3->getLine("TOV1"),
			      timerIrq3->getLine("OCF1A"),
			      new PinAtPort(&portd, 5), //todo ctae check pin
			      timerIrq3->getLine("OCF1B"),
			      new PinAtPort(&portd, 4), //todo ctae check pin
			      timerIrq3->getLine("OCF1C"),
			      new PinAtPort(&portd, 4), //todo ctae check pin
			      timerIrq3->getLine("ICF1"),
			      inputCapture3);

    timerIrq4 = new TimerIRQRegister(this, irqSystem, 4);
    timerIrq4->registerLine(0, new IRQLine("TOV1",  45));
    timerIrq4->registerLine(1, new IRQLine("OCF1A", 42));
    timerIrq4->registerLine(2, new IRQLine("OCF1B", 43));
    timerIrq4->registerLine(3, new IRQLine("OCF1C", 44));
    timerIrq4->registerLine(5, new IRQLine("ICF1",  41)); //todo ctae ICF1 is capture event??

    inputCapture4 = new ICaptureSource(PinAtPort(&portb, 0)); //todo ctae check pin
    timer4 = new HWTimer16_3C(this,
			      new PrescalerMultiplexerExt(&prescaler01, PinAtPort(&portd, 5)), //todo ctae check pin
			      4,
			      timerIrq4->getLine("TOV1"),
			      timerIrq4->getLine("OCF1A"),
			      new PinAtPort(&portd, 5), //todo ctae check pin
			      timerIrq4->getLine("OCF1B"),
			      new PinAtPort(&portd, 4), //todo ctae check pin
			      timerIrq4->getLine("OCF1C"),
			      new PinAtPort(&portd, 4), //todo ctae check pin
			      timerIrq4->getLine("ICF1"),
			      inputCapture4);

    timerIrq5 = new TimerIRQRegister(this, irqSystem, 5);
    timerIrq5->registerLine(0, new IRQLine("TOV1",  50));
    timerIrq5->registerLine(1, new IRQLine("OCF1A", 47));
    timerIrq5->registerLine(2, new IRQLine("OCF1B", 48));
    timerIrq5->registerLine(3, new IRQLine("OCF1C", 49));
    timerIrq5->registerLine(5, new IRQLine("ICF1",  46)); //todo ctae ICF1 is capture event??

    inputCapture5 = new ICaptureSource(PinAtPort(&portb, 0)); //todo ctae check pin
    timer5 = new HWTimer16_3C(this,
			      new PrescalerMultiplexerExt(&prescaler01, PinAtPort(&portd, 5)), //todo ctae check pin
			      5,
			      timerIrq5->getLine("TOV1"),
			      timerIrq5->getLine("OCF1A"),
			      new PinAtPort(&portd, 5), //todo ctae check pin
			      timerIrq5->getLine("OCF1B"),
			      new PinAtPort(&portd, 4), //todo ctae check pin
			      timerIrq5->getLine("OCF1C"),
			      new PinAtPort(&portd, 4), //todo ctae check pin
			      timerIrq5->getLine("ICF1"),
			      inputCapture5);

    gpior0_reg = new GPIORegister(this, &coreTraceGroup, "GPIOR0");
    gpior1_reg = new GPIORegister(this, &coreTraceGroup, "GPIOR1");
    gpior2_reg = new GPIORegister(this, &coreTraceGroup, "GPIOR2");

    admux = new HWAdmuxM16(this, &porta.GetPin(0), &porta.GetPin(1), &porta.GetPin(2),
                                 &porta.GetPin(3), &porta.GetPin(4), &porta.GetPin(5),
                                 &porta.GetPin(6), &porta.GetPin(7));
    aref = new HWARef4(this, HWARef4::REFTYPE_BG3);
    ad = new HWAd(this, HWAd::AD_M164, irqSystem, 24, admux, aref);

    acomp = new HWAcomp(this, irqSystem, PinAtPort(&portb, 2), PinAtPort(&portb, 3), 23, ad, timer1);

    spi = new HWSpi(this,
                    irqSystem,
                    PinAtPort(&portb, 5),   // MOSI
                    PinAtPort(&portb, 6),   // MISO
                    PinAtPort(&portb, 7),   // SCK
                    PinAtPort(&portb, 4),   // /SS
                    19,                     // irqvec
                    true);
    
    wado = new HWWado(this);

    usart0 = new HWUsart(this,
                         irqSystem,
                         PinAtPort(&portd, 1),    // TXD0
                         PinAtPort(&portd, 0),    // RXD0
                         PinAtPort(&portb, 0),    // XCK0
                         20,   // (21) RX complete vector
                         21,   // (22) UDRE vector
                         22);  // (23) TX complete vector

    usart1 = new HWUsart(this,
                         irqSystem,
                         PinAtPort(&portd, 3),    // TXD1
                         PinAtPort(&portd, 2),    // RXD1
                         PinAtPort(&portd, 4),    // XCK1
                         28,   // (29) RX complete vector
                         29,   // (30) UDRE vector
                         30,   // (31) TX complete vector
                         1);   // instance_id for tracking in UI

    rw[0x12d]= & timer5->ocrc_h_reg;
    rw[0x12c]= & timer5->ocrc_l_reg;
    rw[0x12b]= & timer5->ocrb_h_reg;
    rw[0x12a]= & timer5->ocrb_l_reg;
    rw[0x129]= & timer5->ocra_h_reg;
    rw[0x128]= & timer5->ocra_l_reg;
    rw[0x127]= & timer5->icr_h_reg;
    rw[0x126]= & timer5->icr_l_reg;
    rw[0x125]= & timer5->tcnt_h_reg;
    rw[0x124]= & timer5->tcnt_l_reg;
    rw[0x122]= & timer5->tccrc_reg;
    rw[0x121]= & timer5->tccrb_reg;
    rw[0x120]= & timer5->tccra_reg;
    rw[0x10B]= & portl.port_reg;
    rw[0x10A]= & portl.ddr_reg;
    rw[0x109]= & portl.pin_reg;
    rw[0x108]= & portk.port_reg;
    rw[0x107]= & portk.ddr_reg;
    rw[0x106]= & portk.pin_reg;
    rw[0x105]= & portj.port_reg;
    rw[0x104]= & portj.ddr_reg;
    rw[0x103]= & portj.pin_reg;
    rw[0x102]= & porth.port_reg;
    rw[0x101]= & porth.ddr_reg;
    rw[0x100]= & porth.pin_reg;

    // 0xCF - 0xFF reserved

    rw[0xCE]= & usart1->udr_reg;
    rw[0xCD]= & usart1->ubrrhi_reg;
    rw[0xCC]= & usart1->ubrr_reg;
    // 0xCB reserved
    rw[0xCA]= & usart1->ucsrc_reg;
    rw[0xC9]= & usart1->ucsrb_reg;
    rw[0xC8]= & usart1->ucsra_reg;
    // 0xC7 reserved
    rw[0xC6]= & usart0->udr_reg;
    rw[0xC5]= & usart0->ubrrhi_reg;
    rw[0xC4]= & usart0->ubrr_reg;
    // 0xC3 reserved
    rw[0xC2]= & usart0->ucsrc_reg;
    rw[0xC1]= & usart0->ucsrb_reg;
    rw[0xC0]= & usart0->ucsra_reg;
    // 0xBF reserved
    // 0xBE reserved
    rw[0xBD]= new NotSimulatedRegister("TWI register TWAMR not simulated");
    rw[0xBC]= new NotSimulatedRegister("TWI register TWCR not simulated");
    rw[0xBB]= new NotSimulatedRegister("TWI register TWDR not simulated");
    rw[0xBA]= new NotSimulatedRegister("TWI register TWAR not simulated");
    rw[0xB9]= new NotSimulatedRegister("TWI register TWSR not simulated");
    rw[0xB8]= new NotSimulatedRegister("TWI register TWBR not simulated");
    // 0xB7 reserved
    rw[0xb6]= & assr_reg;
    // 0xb5 reserved
    rw[0xb4]= & timer2->ocrb_reg;
    rw[0xb3]= & timer2->ocra_reg;
    rw[0xb2]= & timer2->tcnt_reg;
    rw[0xb1]= & timer2->tccrb_reg;
    rw[0xb0]= & timer2->tccra_reg;
    // 0x8c - 0xaf reserved
    rw[0xad]= & timer4->ocrc_h_reg;
    rw[0xac]= & timer4->ocrc_l_reg;
    rw[0xab]= & timer4->ocrb_h_reg;
    rw[0xaa]= & timer4->ocrb_l_reg;
    rw[0xa9]= & timer4->ocra_h_reg;
    rw[0xa8]= & timer4->ocra_l_reg;
    rw[0xa7]= & timer4->icr_h_reg;
    rw[0xa6]= & timer4->icr_l_reg;
    rw[0xa5]= & timer4->tcnt_h_reg;
    rw[0xa4]= & timer4->tcnt_l_reg;
    rw[0xa2]= & timer4->tccrc_reg;
    rw[0xa1]= & timer4->tccrb_reg;
    rw[0xa0]= & timer4->tccra_reg;
    rw[0x9d]= & timer3->ocrc_h_reg;
    rw[0x9c]= & timer3->ocrc_l_reg;
    rw[0x9b]= & timer3->ocrb_h_reg;
    rw[0x9a]= & timer3->ocrb_l_reg;
    rw[0x99]= & timer3->ocra_h_reg;
    rw[0x98]= & timer3->ocra_l_reg;
    rw[0x97]= & timer3->icr_h_reg;
    rw[0x96]= & timer3->icr_l_reg;
    rw[0x95]= & timer3->tcnt_h_reg;
    rw[0x94]= & timer3->tcnt_l_reg;
    rw[0x92]= & timer3->tccrc_reg;
    rw[0x91]= & timer3->tccrb_reg;
    rw[0x90]= & timer3->tccra_reg;
    rw[0x8d]= & timer1->ocrc_h_reg;
    rw[0x8c]= & timer1->ocrc_l_reg;
    rw[0x8b]= & timer1->ocrb_h_reg;
    rw[0x8a]= & timer1->ocrb_l_reg;
    rw[0x89]= & timer1->ocra_h_reg;
    rw[0x88]= & timer1->ocra_l_reg;
    rw[0x87]= & timer1->icr_h_reg;
    rw[0x86]= & timer1->icr_l_reg;
    rw[0x85]= & timer1->tcnt_h_reg;
    rw[0x84]= & timer1->tcnt_l_reg;
    // 0x83 reserved
    rw[0x82]= & timer1->tccrc_reg;
    rw[0x81]= & timer1->tccrb_reg;
    rw[0x80]= & timer1->tccra_reg;
    rw[0x7F]= new NotSimulatedRegister("ADC register DIDR1 not simulated");
    rw[0x7E]= new NotSimulatedRegister("ADC register DIDR0 not simulated");
    // 0x7D reserved
    rw[0x7C]= & ad->admux_reg;
    rw[0x7B]= & ad->adcsrb_reg;
    rw[0x7A]= & ad->adcsra_reg;
    rw[0x79]= & ad->adch_reg;
    rw[0x78]= & ad->adcl_reg;
    // 0x74, 0x75, 0x76, 0x77 reserved
    rw[0x73]= & timerIrq5->timsk_reg;
    rw[0x72]= & timerIrq4->timsk_reg;
    rw[0x71]= & timerIrq3->timsk_reg;
    rw[0x70]= & timerIrq2->timsk_reg;
    rw[0x6F]= & timerIrq1->timsk_reg;
    rw[0x6E]= & timerIrq0->timsk_reg;
    rw[0x6d]= pcmsk2_reg;
    rw[0x6c]= pcmsk1_reg;
    rw[0x6b]= pcmsk0_reg;
    rw[0x69]= eicrb_reg;
    rw[0x69]= eicra_reg;
    rw[0x68]= pcicr_reg;
    // 0x67 reserved
    rw[0x66]= osccal_reg;
    rw[0x65]= new NotSimulatedRegister("MCU register PRR1 not simulated");
    rw[0x64]= new NotSimulatedRegister("MCU register PRR0 not simulated");
    // 0x63 reserved
    // 0x62 reserved
    rw[0x61]= clkpr_reg;
    rw[0x60]= new NotSimulatedRegister("MCU register WDTCSR not simulated");
    rw[0x5f]= statusRegister;
    rw[0x5e]= & ((HWStackSram *)stack)->sph_reg;
    rw[0x5d]= & ((HWStackSram *)stack)->spl_reg;
    rw[0x5c]= & eind->ext_reg;
    rw[0x5b]= & rampz->ext_reg;
    // 0x58 - 0x5a reserved
    rw[0x57]= new NotSimulatedRegister("Self-programming register SPMCSR not simulated");
    // 0x56 reserved
    rw[0x55]= new NotSimulatedRegister("MCU register MCUCR not simulated");
    rw[0x54]= new NotSimulatedRegister("MCU register MCUSR not simulated");
    rw[0x53]= new NotSimulatedRegister("MCU register SMCR not simulated");
    // 0x52 reserved
    rw[0x51]= new NotSimulatedRegister("On-chip debug register OCDR not simulated");
    rw[0x50]= & acomp->acsr_reg;
    // 0x4F reserved
    rw[0x4E]= & spi->spdr_reg;
    rw[0x4D]= & spi->spsr_reg;
    rw[0x4C]= & spi->spcr_reg;
    rw[0x4B]= gpior2_reg;
    rw[0x4A]= gpior1_reg;
    // 0x49 reserved
    rw[0x48]= & timer0->ocrb_reg;
    rw[0x47]= & timer0->ocra_reg;
    rw[0x46]= & timer0->tcnt_reg;
    rw[0x45]= & timer0->tccrb_reg;
    rw[0x44]= & timer0->tccra_reg;
    rw[0x43]= & gtccr_reg;
    rw[0x42]= & eeprom->eearh_reg;
    rw[0x41]= & eeprom->eearl_reg;
    rw[0x40]= & eeprom->eedr_reg;
    rw[0x3F]= & eeprom->eecr_reg;
    rw[0x3E]= gpior0_reg;
    rw[0x3D]= eimsk_reg;
    rw[0x3C]= eifr_reg;
    rw[0x3b]= pcifr_reg;
    rw[0x3A]= & timerIrq5->tifr_reg;
    rw[0x39]= & timerIrq4->tifr_reg;
    rw[0x38]= & timerIrq3->tifr_reg;
    rw[0x37]= & timerIrq2->tifr_reg;
    rw[0x36]= & timerIrq1->tifr_reg;
    rw[0x35]= & timerIrq0->tifr_reg;
    rw[0x34]= & portg.port_reg;
    rw[0x33]= & portg.ddr_reg;
    rw[0x32]= & portg.pin_reg;
    rw[0x31]= & portf.port_reg;
    rw[0x30]= & portf.ddr_reg;
    rw[0x2F]= & portf.pin_reg;
    rw[0x2E]= & porte.port_reg;
    rw[0x2D]= & porte.ddr_reg;
    rw[0x2C]= & porte.pin_reg;
    rw[0x2B]= & portd.port_reg;
    rw[0x2A]= & portd.ddr_reg;
    rw[0x29]= & portd.pin_reg;
    rw[0x28]= & portc.port_reg;
    rw[0x27]= & portc.ddr_reg;
    rw[0x26]= & portc.pin_reg;
    rw[0x25]= & portb.port_reg;
    rw[0x24]= & portb.ddr_reg;
    rw[0x23]= & portb.pin_reg;
    rw[0x22]= & porta.port_reg;
    rw[0x21]= & porta.ddr_reg;
    rw[0x20]= & porta.pin_reg;

    Reset();
}

