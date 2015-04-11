# Introduction #

The Medulla Firmware Library is written to help users develop firmware for the ATxmega128a1 on the medulla board. In particular the library focuses on providing firmware drivers to interface with robotics hardware. This library is currently under development. Right now the goal is to re-implement the drivers in the medulla.1.5.1 directory and a  cleaner more useful way.

# Implementation Guidelines #

In most cases program execution speed will be very important in programs using the Medulla Firmware Library. Unless absolutely necessarily library functions should not block program execution. Additionally, wherever possible the interrupts, hardware peripherals, DMA, and event system in the XMega should be used. This will ensure a minimal impact on firmware performance, and make the most out of the XMega hardware.

Wherever not absolutely necessary, all drivers should store it's data in structs managed by the user program. The users program should be able to use hardware drivers multiple times on different IO simply by passing around a struct containing the configuration. This should allow for the most flexibility.

  * Tabs should be used for all indents
  * Java style syntax should be used for curly brackets (open bracket should be on the line of the function name and the close bracket should be on the line following the last line of the function).

# Hardware Drivers #

The following is an initial list of drivers that will be implemented in the Medulla Firmware Library:
  * **XMega system clock**
> > This driver simplifies configuring the clock systems on the medulla.
  * **Digital IO driver**
> > The DIO driver is a set of macros to simplify the use of the XMega digital IO. It provides simple interfaces for configuring IO direction and pull-up resistors. It also makes it easy to configure pin change interrupts on any DIO pin.
  * **ADC Driver**
> > The ADC driver provides functions to control the internal ADCs on the XMega.
  * **Biss-C encoder driver**
> > This driver implements the BISS-C interface to read encoders on any of XMega's SPI ports.
  * **SSI encoder driver**
> > The SSI encoder driver implements the SSI protocol on an SPI port to read data from an encoder.
  * **EtherCAT driver**
> > This driver implements the SPI protocol between the XMega and the EtherCAT slave chip on the Medulla board. It also manages the mapping between variables in XMega memory and the EtherCAT PDOs.
  * **Serial driver**
> > The serial driver uses any of the available USARTS to implement RS232 communication.