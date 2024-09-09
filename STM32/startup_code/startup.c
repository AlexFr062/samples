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

void SVC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void) __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias("Default_Handler")));

void WWDG_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void PVD_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TAMP_STAMP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void RTC_WKUP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void FLASH_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void RCC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI0_IRQHandler                  (void) __attribute__((weak, alias("Default_Handler")));
void EXTI1_IRQHandler                  (void) __attribute__((weak, alias("Default_Handler")));
void EXTI2_IRQHandler                  (void) __attribute__((weak, alias("Default_Handler")));
void EXTI3_IRQHandler                  (void) __attribute__((weak, alias("Default_Handler")));
void EXTI4_IRQHandler                  (void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream0_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream1_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream2_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream3_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream4_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream5_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream6_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void ADC_IRQHandler                    (void) __attribute__((weak, alias("Default_Handler")));
void CAN1_TX_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void CAN1_RX0_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void CAN1_RX1_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void CAN1_SCE_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void EXTI9_5_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void TIM1_BRK_TIM9_IRQHandler          (void) __attribute__((weak, alias("Default_Handler")));
void TIM1_UP_TIM10_IRQHandler          (void) __attribute__((weak, alias("Default_Handler")));
void TIM1_TRG_COM_TIM11_IRQHandler     (void) __attribute__((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void TIM2_IRQHandler                   (void) __attribute__((weak, alias("Default_Handler")));
void TIM3_IRQHandler                   (void) __attribute__((weak, alias("Default_Handler")));
void TIM4_IRQHandler                   (void) __attribute__((weak, alias("Default_Handler")));
void I2C1_EV_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void I2C1_ER_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void I2C2_EV_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void I2C2_ER_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void SPI1_IRQHandler                   (void) __attribute__((weak, alias("Default_Handler")));
void SPI2_IRQHandler                   (void) __attribute__((weak, alias("Default_Handler")));
void USART1_IRQHandler                 (void) __attribute__((weak, alias("Default_Handler")));
void USART2_IRQHandler                 (void) __attribute__((weak, alias("Default_Handler")));
void USART3_IRQHandler                 (void) __attribute__((weak, alias("Default_Handler")));
void EXTI15_10_IRQHandler              (void) __attribute__((weak, alias("Default_Handler")));
void RTC_Alarm_IRQHandler              (void) __attribute__((weak, alias("Default_Handler")));
void OTG_FS_WKUP_IRQHandler            (void) __attribute__((weak, alias("Default_Handler")));
void TIM8_BRK_TIM12_IRQHandler         (void) __attribute__((weak, alias("Default_Handler")));
void TIM8_UP_TIM13_IRQHandler          (void) __attribute__((weak, alias("Default_Handler")));
void TIM8_TRG_COM_TIM14_IRQHandler     (void) __attribute__((weak, alias("Default_Handler")));
void TIM8_CC_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream7_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void FMC_IRQHandler                    (void) __attribute__((weak, alias("Default_Handler")));
void SDIO_IRQHandler                   (void) __attribute__((weak, alias("Default_Handler")));
void TIM5_IRQHandler                   (void) __attribute__((weak, alias("Default_Handler")));
void SPI3_IRQHandler                   (void) __attribute__((weak, alias("Default_Handler")));
void UART4_IRQHandler                  (void) __attribute__((weak, alias("Default_Handler")));
void UART5_IRQHandler                  (void) __attribute__((weak, alias("Default_Handler")));
void TIM6_DAC_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void TIM7_IRQHandler                   (void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream0_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream1_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream2_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream3_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream4_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void ETH_IRQHandler                    (void) __attribute__((weak, alias("Default_Handler")));
void ETH_WKUP_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void CAN2_TX_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void CAN2_RX0_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void CAN2_RX1_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void CAN2_SCE_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void OTG_FS_IRQHandler                 (void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream5_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream6_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream7_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void USART6_IRQHandler                 (void) __attribute__((weak, alias("Default_Handler")));
void I2C3_EV_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void I2C3_ER_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void OTG_HS_EP1_OUT_IRQHandler         (void) __attribute__((weak, alias("Default_Handler")));
void OTG_HS_EP1_IN_IRQHandler          (void) __attribute__((weak, alias("Default_Handler")));
void OTG_HS_WKUP_IRQHandler            (void) __attribute__((weak, alias("Default_Handler")));
void OTG_HS_IRQHandler                 (void) __attribute__((weak, alias("Default_Handler")));
void DCMI_IRQHandler                   (void) __attribute__((weak, alias("Default_Handler")));
void HASH_RNG_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void FPU_IRQHandler                    (void) __attribute__((weak, alias("Default_Handler")));
void UART7_IRQHandler                  (void) __attribute__((weak, alias("Default_Handler")));
void UART8_IRQHandler                  (void) __attribute__((weak, alias("Default_Handler")));
void SPI4_IRQHandler                   (void) __attribute__((weak, alias("Default_Handler")));
void SPI5_IRQHandler                   (void) __attribute__((weak, alias("Default_Handler")));
void SPI6_IRQHandler                   (void) __attribute__((weak, alias("Default_Handler")));
void SAI1_IRQHandler                   (void) __attribute__((weak, alias("Default_Handler")));
void LTDC_IRQHandler                   (void) __attribute__((weak, alias("Default_Handler")));
void LTDC_ER_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void DMA2D_IRQHandler                  (void) __attribute__((weak, alias("Default_Handler")));

uint32_t g_pfnVectors[] __attribute__((section(".isr_vector"))) =
{
  STACK_POINTER_INIT_ADDRESS,

  // Cortex-M system exceptions
  (uint32_t)&Reset_Handler,
  (uint32_t)&Nmi_Handler,
  (uint32_t)&Hard_Fault_Handler,
  (uint32_t)&Bus_Fault_Handler,
  (uint32_t)&Usage_Fault_Handler,

  0,
  0,
  0,
  0,
  0,
  (uint32_t)&SVC_Handler,
  (uint32_t)&DebugMon_Handler,
  0,
  (uint32_t)&PendSV_Handler,
  (uint32_t)&SysTick_Handler,

  (uint32_t)&WWDG_IRQHandler                   ,
  (uint32_t)&PVD_IRQHandler                    ,
  (uint32_t)&TAMP_STAMP_IRQHandler             ,
  (uint32_t)&RTC_WKUP_IRQHandler               ,
  (uint32_t)&FLASH_IRQHandler                  ,
  (uint32_t)&RCC_IRQHandler                    ,
  (uint32_t)&EXTI0_IRQHandler                  ,
  (uint32_t)&EXTI1_IRQHandler                  ,
  (uint32_t)&EXTI2_IRQHandler                  ,
  (uint32_t)&EXTI3_IRQHandler                  ,
  (uint32_t)&EXTI4_IRQHandler                  ,
  (uint32_t)&DMA1_Stream0_IRQHandler           ,
  (uint32_t)&DMA1_Stream1_IRQHandler           ,
  (uint32_t)&DMA1_Stream2_IRQHandler           ,
  (uint32_t)&DMA1_Stream3_IRQHandler           ,
  (uint32_t)&DMA1_Stream4_IRQHandler           ,
  (uint32_t)&DMA1_Stream5_IRQHandler           ,
  (uint32_t)&DMA1_Stream6_IRQHandler           ,
  (uint32_t)&ADC_IRQHandler                    ,
  (uint32_t)&CAN1_TX_IRQHandler                ,
  (uint32_t)&CAN1_RX0_IRQHandler               ,
  (uint32_t)&CAN1_RX1_IRQHandler               ,
  (uint32_t)&CAN1_SCE_IRQHandler               ,
  (uint32_t)&EXTI9_5_IRQHandler                ,
  (uint32_t)&TIM1_BRK_TIM9_IRQHandler          ,
  (uint32_t)&TIM1_UP_TIM10_IRQHandler          ,
  (uint32_t)&TIM1_TRG_COM_TIM11_IRQHandler     ,
  (uint32_t)&TIM1_CC_IRQHandler                ,
  (uint32_t)&TIM2_IRQHandler                   ,
  (uint32_t)&TIM3_IRQHandler                   ,
  (uint32_t)&TIM4_IRQHandler                   ,
  (uint32_t)&I2C1_EV_IRQHandler                ,
  (uint32_t)&I2C1_ER_IRQHandler                ,
  (uint32_t)&I2C2_EV_IRQHandler                ,
  (uint32_t)&I2C2_ER_IRQHandler                ,
  (uint32_t)&SPI1_IRQHandler                   ,
  (uint32_t)&SPI2_IRQHandler                   ,
  (uint32_t)&USART1_IRQHandler                 ,
  (uint32_t)&USART2_IRQHandler                 ,
  (uint32_t)&USART3_IRQHandler                 ,
  (uint32_t)&EXTI15_10_IRQHandler              ,
  (uint32_t)&RTC_Alarm_IRQHandler              ,
  (uint32_t)&OTG_FS_WKUP_IRQHandler            ,
  (uint32_t)&TIM8_BRK_TIM12_IRQHandler         ,
  (uint32_t)&TIM8_UP_TIM13_IRQHandler          ,
  (uint32_t)&TIM8_TRG_COM_TIM14_IRQHandler     ,
  (uint32_t)&TIM8_CC_IRQHandler                ,
  (uint32_t)&DMA1_Stream7_IRQHandler           ,
  (uint32_t)&FMC_IRQHandler                    ,
  (uint32_t)&SDIO_IRQHandler                   ,
  (uint32_t)&TIM5_IRQHandler                   ,
  (uint32_t)&SPI3_IRQHandler                   ,
  (uint32_t)&UART4_IRQHandler                  ,
  (uint32_t)&UART5_IRQHandler                  ,
  (uint32_t)&TIM6_DAC_IRQHandler               ,
  (uint32_t)&TIM7_IRQHandler                   ,
  (uint32_t)&DMA2_Stream0_IRQHandler           ,
  (uint32_t)&DMA2_Stream1_IRQHandler           ,
  (uint32_t)&DMA2_Stream2_IRQHandler           ,
  (uint32_t)&DMA2_Stream3_IRQHandler           ,
  (uint32_t)&DMA2_Stream4_IRQHandler           ,
  (uint32_t)&ETH_IRQHandler                    ,
  (uint32_t)&ETH_WKUP_IRQHandler               ,
  (uint32_t)&CAN2_TX_IRQHandler                ,
  (uint32_t)&CAN2_RX0_IRQHandler               ,
  (uint32_t)&CAN2_RX1_IRQHandler               ,
  (uint32_t)&CAN2_SCE_IRQHandler               ,
  (uint32_t)&OTG_FS_IRQHandler                 ,
  (uint32_t)&DMA2_Stream5_IRQHandler           ,
  (uint32_t)&DMA2_Stream6_IRQHandler           ,
  (uint32_t)&DMA2_Stream7_IRQHandler           ,
  (uint32_t)&USART6_IRQHandler                 ,
  (uint32_t)&I2C3_EV_IRQHandler                ,
  (uint32_t)&I2C3_ER_IRQHandler                ,
  (uint32_t)&OTG_HS_EP1_OUT_IRQHandler         ,
  (uint32_t)&OTG_HS_EP1_IN_IRQHandler          ,
  (uint32_t)&OTG_HS_WKUP_IRQHandler            ,
  (uint32_t)&OTG_HS_IRQHandler                 ,
  (uint32_t)&DCMI_IRQHandler                   ,
  0,
  (uint32_t)&HASH_RNG_IRQHandler               ,
  (uint32_t)&FPU_IRQHandler                    ,
  (uint32_t)&UART7_IRQHandler                  ,
  (uint32_t)&UART8_IRQHandler                  ,
  (uint32_t)&SPI4_IRQHandler                   ,
  (uint32_t)&SPI5_IRQHandler                   ,
  (uint32_t)&SPI6_IRQHandler                   ,
  (uint32_t)&SAI1_IRQHandler                   ,
  (uint32_t)&LTDC_IRQHandler                   ,
  (uint32_t)&LTDC_ER_IRQHandler                ,
  (uint32_t)&DMA2D_IRQHandler
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


