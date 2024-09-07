#include <stdint.h>

void Reset_Handler();
void Default_Handler();
int main(void);

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


    for(;;) {}
}


