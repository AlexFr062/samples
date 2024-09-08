#include <stdint.h>

void Reset_Handler();
void Default_Handler();
int main(void);
void SystemInit(void);
void __libc_init_array();

// From the linker script:
// RAM    (xrw)    : ORIGIN = 0x20000000,   LENGTH = 192K
// FLASH    (rx)    : ORIGIN = 0x8000000,   LENGTH = 2048K

#define SRAM_START (0x20000000U)
#define SRAM_SIZE (192U * 1024U)                // 0x30000
#define SRAM_END (SRAM_START + SRAM_SIZE)
#define STACK_POINTER_INIT_ADDRESS (SRAM_END)

// Cortex-M system exceptions
void Nmi_Handler(void) __attribute__((weak, alias("Default_Handler")));
void Hard_Fault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void Bus_Fault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void Usage_Fault_Handler(void) __attribute__((weak, alias("Default_Handler")));

uint32_t g_pfnVectors[] __attribute__((section(".isr_vector"))) =
{
  STACK_POINTER_INIT_ADDRESS,

  // Cortex-M system exceptions
  (uint32_t)&Reset_Handler,
  (uint32_t)&Nmi_Handler,
  (uint32_t)&Hard_Fault_Handler,
  (uint32_t)&Bus_Fault_Handler,
  (uint32_t)&Usage_Fault_Handler
};

// **********************************************************************************************
// Variables, defined in the linker script STM32F429ZIX_FLASH.ld
// SRAM:
extern uint8_t _sdata;		// start address for the .data section   (initialized data section)
extern uint8_t _edata;		// end address for the .data section
extern uint8_t _sbss;		// start address for the .bss section     (uninitialized data section)
extern uint8_t _ebss;		// end address for the .bss section

// Flash:
extern uint8_t _sidata;		// start address for initialized values
// **********************************************************************************************
uint32_t g_data1 = 0xA1020304;	// Initialized data
uint32_t g_data2;               // Uninitialized data

void Default_Handler()
{
    for(;;){}
}

void Reset_Handler()
{
    asm ("ldr r0, = g_data1");  // r0 = &g_data1    0x20000000
    asm ("ldr r1, [r0]");       // r1 = *r0         0xbe00                 junk, expected 0xA1020304
    asm ("ldr r2, = g_data2");  // r2 = &data2      0x20000020
    asm ("ldr r3, [r2]");       // r3 = *r2         0x60015155             junk, expected 0

    asm ("ldr r0, =_sdata");    // 0x20000000    r0 = &sdata    SRAM
    asm ("ldr r1, =_edata");    // 0x20000004    r1 = &_edata   SRAM
    asm ("ldr r2, =_sbss");     // 0x20000004    r2 = &_sbss    SRAM
    asm ("ldr r3, =_ebss");     // 0x20000024    r3 = &_sbss    SRAM
    asm ("ldr r4, =_sidata");   // 0x80012dc     r4 = &_sidata  flash

    SystemInit();

    // ********************************************************************************
    // Copy (&edata - &_sdata)/4  uint32_t values  from _sidata (flash) to _sdata (SRAM)

    const uint32_t data_size = ((uint32_t)&_edata - (uint32_t)&_sdata) / 4;

    uint32_t *source_ptr = (uint32_t*) &_sidata; // Flash
    uint32_t *dest_ptr = (uint32_t*) &_sdata;    // SRAM

    for (uint32_t i = 0; i < data_size; i++)
    {
        dest_ptr[i] = source_ptr[i];
    }

    asm ("ldr r0, = g_data1");  // 0x20000000
    asm ("ldr r1, [r0]");       // 0xa1020304             OK
    asm ("ldr r2, = g_data2");  // 0x20000020
    asm ("ldr r3, [r2]");       // 0x60015155             still junk

    // ********************************************************************************
    // zero (&_ebss - &_sbss)/4  uint32_t values, starting from _sbss

    const uint32_t bss_size = ((uint32_t)&_ebss - (uint32_t)&_sbss) / 4;
    dest_ptr = (uint32_t*) &_sbss;

    for (uint32_t i = 0; i < bss_size; i++)
    {
        dest_ptr[i] = 0;
    }

    asm ("ldr r0, = g_data1");  // 0x20000000
    asm ("ldr r1, [r0]");       // 0xa1020304             OK
    asm ("ldr r2, = g_data2");  // 0x20000020
    asm ("ldr r3, [r2]");       // 0x0                    OK

    // ********************************************************************************


    __libc_init_array();
    main();

    //for(;;) {}
}


