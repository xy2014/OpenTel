
#include "eeprom.h"

ConfigGlobalTS ConfigGlobal;  
 
/* Private define ------------------------------------------------------------*/
/* Define the STM32F10x FLASH Page Size depending on the used STM32 device */
#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
  #define FLASH_PAGE_SIZE    ((uint16_t)0x800)
#else
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)
#endif

#define BANK1_WRITE_START_ADDR  ((uint32_t)0x0801e000)		//120k ~ 124k
#define BANK1_WRITE_END_ADDR    ((uint32_t)0x0801f000)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/  
uint32_t EraseCounter = 0x00, NbrOfPage = 0x00;
volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;

/* Private function prototypes -----------------------------------------------*/   
/* Private functions ---------------------------------------------------------*/
 
/**
	*
	*/
void EEPROM_Write(void) {
	__IO uint32_t *p;
	uint32_t i;		
	
  /* Porgram FLASH Bank1 ********************************************************/       
  /* Unlock the Flash Bank1 Program Erase controller */
  FLASH_UnlockBank1();

  /* Define the number of page to be erased */
  NbrOfPage = (BANK1_WRITE_END_ADDR - BANK1_WRITE_START_ADDR) / FLASH_PAGE_SIZE;

  /* Clear All pending flags */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	

  /* Erase the FLASH pages */
  for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
  {
    FLASHStatus = FLASH_ErasePage(BANK1_WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter));
  }
  
  /* Program Flash Bank1 */
	p = (__IO uint32_t *)&ConfigGlobal;
	for(i=0; i<sizeof(ConfigGlobalTS); i+=4){		
	  FLASH_ProgramWord(BANK1_WRITE_START_ADDR + i, *p++);
	}	

  FLASH_LockBank1();	 
}

/*****************************************************************************************************

*****************************************************************************************************/
void EEPROM_Read(void)
{	
	__IO uint32_t *p;
	uint32_t i;		
		
	//?EPPROM??,???ConfigGlobe(????)???
	p = (__IO uint32_t *)&ConfigGlobal;
	for(i=0; i<sizeof(ConfigGlobalTS); i+=4){		
		*p++ = *(__IO uint32_t *)(BANK1_WRITE_START_ADDR + i);	
	}	
	
	//????
	if(ConfigGlobal.good != 0x55aa){
		ConfigGlobal.good = 0x55aa;
		
	}	
}







