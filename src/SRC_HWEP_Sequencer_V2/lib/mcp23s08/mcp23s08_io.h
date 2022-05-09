// ------------------------------------------------------------
// Register Addresses
// ------------------------------------------------------------

typedef enum
{
	IODIR = 0x00,	// IO Direction (0:OUT) (1:IN)
	IPOL = 0x01,	
	GPINTEN = 0x02,	
	DEFVAL = 0x03,
	INTCON = 0x04,
	IOCON = 0x05,
	GPPU = 0x06,
	INTF = 0x07,
	INTCAP = 0x08,
	GPIO_R = 0x09,
	OLAT = 0x0A,
} mcp23s08_reg_adr;

// ------------------------------------------------------------
// Hardware Addresses
// ------------------------------------------------------------

typedef enum
{
	HW_ADR_0 = 0b00,
	HW_ADR_1 = 0b01,
	HW_ADR_2 = 0b10,
	HW_ADR_3 = 0b11,
} mcp23s08_hw_adr;

// ------------------------------------------------------------
// IODIR
// ------------------------------------------------------------
#define IO0 0
#define IO1 1
#define IO2 2
#define IO3 3
#define IO4 4
#define IO5 5
#define IO6 6
#define IO7 7
// ------------------------------------------------------------
// IPOL
// ------------------------------------------------------------
#define IP0 0
#define IP1 1
#define IP2 2
#define IP3 3
#define IP4 4
#define IP5 5
#define IP6 6
#define IP7 7
// ------------------------------------------------------------
// GPINTEN
// ------------------------------------------------------------
#define GPINT0 0
#define GPINT1 1
#define GPINT2 2
#define GPINT3 3
#define GPINT4 4
#define GPINT5 5
#define GPINT6 6
#define GPINT7 7
// ------------------------------------------------------------
// DEFVAL
// ------------------------------------------------------------
#define DEF0 0
#define DEF1 1
#define DEF2 2
#define DEF3 3
#define DEF4 4
#define DEF5 5
#define DEF6 6
#define DEF7 7
// ------------------------------------------------------------
// INTCON
// ------------------------------------------------------------
#define IOC0 0
#define IOC1 1
#define IOC2 2
#define IOC3 3
#define IOC4 4
#define IOC5 5
#define IOC6 6
#define IOC7 7
// ------------------------------------------------------------
// IOCON
// ------------------------------------------------------------
#define INTPOL 1
#define ODR 2
#define HAEN 3
#define DISSLW 4
#define SREAD 5
// ------------------------------------------------------------
// GPPU
// ------------------------------------------------------------
#define PU0 0
#define PU1 1
#define PU2 2
#define PU3 3
#define PU4 4
#define PU5 5
#define PU6 6
#define PU7 7
// ------------------------------------------------------------
// INTF
// ------------------------------------------------------------
#define INT0 0
#define INT1 1
#define INT2 2
#define INT3 3
#define INT4 4
#define INT5 5
#define INT6 6
#define INT7 7
// ------------------------------------------------------------
// INTCAP
// ------------------------------------------------------------
#define ICP0 0
#define ICP1 1
#define ICP2 2
#define ICP3 3
#define ICP4 4
#define ICP5 5
#define ICP6 6
#define ICP7 7
// ------------------------------------------------------------
// GPIO_R
// ------------------------------------------------------------
#define GP0 0
#define GP1 1
#define GP2 2
#define GP3 3
#define GP4 4
#define GP5 5
#define GP6 6
#define GP7 7
// ------------------------------------------------------------
// OLAT
// ------------------------------------------------------------

#define OL0 0
#define OL1 1
#define OL2 2
#define OL3 3
#define OL4 4
#define OL5 5
#define OL6 6
#define OL7 7