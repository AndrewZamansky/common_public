
#ifndef _ArduinoSdSpi_API_H_
#define _ArduinoSdSpi_API_H_


#include "dev_management_api.h"


/*****************  defines  **************/


//

/**
  * @brief      Set SPIn_SS pin to high state.
  * @param[in]  spi The pointer of the specified SPI module.
  * @return     None.
  * @details    Disable automatic slave selection function and set SPIn_SS pin to high state.
  */
//#define SPI_SET_SS_HIGH(spi)   ((spi)->SSCTL = ((spi)->SSCTL & (~(SPI_SSCTL_AUTOSS_Msk | SPI_SSCTL_SSACTPOL_Msk | SPI_SSCTL_SS_Msk))))
#define SPI_SET_SS_HIGH_SPI0()     ((SPI0)->SSCTL = ((SPI0)->SSCTL & (~(SPI_SSCTL_AUTOSS_Msk | SPI_SSCTL_SSACTPOL_Msk | SPI_SSCTL_SS0_Msk))))
#define SPI_SET_SS_HIGH_SPI1()     ((SPI1)->SSCTL = ((SPI1)->SSCTL & (~(SPI_SSCTL_AUTOSS_Msk | SPI_SSCTL_SSACTPOL_Msk | SPI_SSCTL_SS1_Msk))))

/**
  * @brief      Set SPIn_SS pin to low state.
  * @param[in]  spi The pointer of the specified SPI module.
  * @return     None.
  * @details    Disable automatic slave selection function and set SPIn_SS pin to low state.
  */
//#define SPI_SET_SS_LOW(spi)   ((spi)->SSCTL = ((spi)->SSCTL & (~(SPI_SSCTL_AUTOSS_Msk | SPI_SSCTL_SSACTPOL_Msk))) | SPI_SSCTL_SS_Msk)
#define SPI_SET_SS_LOW_SPI0()     ((SPI0)->SSCTL = ((SPI0)->SSCTL & (~(SPI_SSCTL_AUTOSS_Msk | SPI_SSCTL_SSACTPOL_Msk))) | SPI_SSCTL_SS0_Msk)
#define SPI_SET_SS_LOW_SPI1()     ((SPI1)->SSCTL = ((SPI1)->SSCTL & (~(SPI_SSCTL_AUTOSS_Msk | SPI_SSCTL_SSACTPOL_Msk))) | SPI_SSCTL_SS1_Msk)






/**********  define API  types ************/




/**********  define API  functions  ************/


#endif
