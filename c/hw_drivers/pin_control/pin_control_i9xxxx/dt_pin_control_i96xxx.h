#ifndef DT__PIN_CONTROL_I96XXX_H_
#define _DT_PIN_CONTROL_I96XXX_H_

/**********  defines ************/
#define PORT_A  0
#define PORT_B  1
#define PORT_C  2
#define PORT_D  3

#define PIN_0   0
#define PIN_1   1
#define PIN_2   2
#define PIN_3   3
#define PIN_4   4
#define PIN_5   5
#define PIN_6   6
#define PIN_7   7
#define PIN_8   8
#define PIN_9   9
#define PIN_10  10
#define PIN_11  11
#define PIN_12  12
#define PIN_13  13
#define PIN_14  14
#define PIN_15  15

#define PORT_POS  8
#define PIN_POS   4

/*
 * format : PORT | PIN  | FUNCTION
 */
/*****************************************/
/************  PORT A ********************/
#define PIN_A00  ((PORT_A << PORT_POS) | (PIN_0 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_A00_GPIO      (PIN_A00 | 0)
#define PIN_CONTROL_DT_I96XXX_PIN_A00_I2S0_MCLK  (PIN_A00 | 1)

#define PIN_A01  ((PORT_A << PORT_POS) | (PIN_1 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_A01_GPIO       (PIN_A01 | 0)
#define PIN_CONTROL_DT_I96XXX_PIN_A01_I2S0_LRCLK  (PIN_A01 | 1)

#define PIN_A02  ((PORT_A << PORT_POS) | (PIN_2 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_A02_GPIO       (PIN_A02 | 0)
#define PIN_CONTROL_DT_I96XXX_PIN_A02_I2S0_BCLK   (PIN_A02 | 1)

#define PIN_A03  ((PORT_A << PORT_POS) | (PIN_3 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_A03_GPIO     (PIN_A03 | 0)
#define PIN_CONTROL_DT_I96XXX_PIN_A03_I2S0_DO   (PIN_A03 | 1)

#define PIN_A04  ((PORT_A << PORT_POS) | (PIN_4 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_A04_GPIO     (PIN_A04 | 0)
#define PIN_CONTROL_DT_I96XXX_PIN_A04_I2S0_DI   (PIN_A04 | 1)

#define PIN_A05  ((PORT_A << PORT_POS) | (PIN_5 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_A05_GPIO        (PIN_A05 | 0)

#define PIN_A06  ((PORT_A << PORT_POS) | (PIN_6 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_A06_GPIO       (PIN_A06 | 0)

#define PIN_A07  ((PORT_A << PORT_POS) | (PIN_7 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_A07_GPIO          (PIN_A07 | 0)
#define PIN_CONTROL_DT_I96XXX_PIN_A07_UART0_TX      (PIN_A07 | 1)

#define PIN_A08  ((PORT_A << PORT_POS) | (PIN_8 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_A08_GPIO          (PIN_A08 | 0)
#define PIN_CONTROL_DT_I96XXX_PIN_A08_UART0_RX      (PIN_A08 | 1)

#define PIN_A09  ((PORT_A << PORT_POS) | (PIN_9 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_A09_GPIO              (PIN_A09 | 0)

#define PIN_A10  ((PORT_A << PORT_POS) | (PIN_10 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_A10_GPIO               (PIN_A10 | 0)

#define PIN_A11  ((PORT_A << PORT_POS) | (PIN_11 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_A11_GPIO          (PIN_A11 | 0)

#define PIN_A12  ((PORT_A << PORT_POS) | (PIN_12 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_A12_GPIO               (PIN_A12 | 0)

#define PIN_A13  ((PORT_A << PORT_POS) | (PIN_13 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_A13_GPIO        (PIN_A13 | 0)

#define PIN_A14  ((PORT_A << PORT_POS) | (PIN_14 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_A14_GPIO        (PIN_A14 | 0)

#define PIN_A15  ((PORT_A << PORT_POS) | (PIN_15 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_A15_GPIO     (PIN_A15 | 0)



/*****************************************/
/************  PORT B ********************/
#define PIN_B00  ((PORT_B << PORT_POS) | (PIN_0 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_B00_GPIO      (PIN_B00 | 0)
#define PIN_CONTROL_DT_I94XXX_PIN_B00_USBD_VBUS (PIN_B00 | 1)

#define PIN_B01  ((PORT_B << PORT_POS) | (PIN_1 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_B01_GPIO     (PIN_B01 | 0)
#define PIN_CONTROL_DT_I94XXX_PIN_B01_USBD_DN  (PIN_B01 | 1)

#define PIN_B02  ((PORT_B << PORT_POS) | (PIN_2 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_B02_GPIO     (PIN_B02 | 0)
#define PIN_CONTROL_DT_I94XXX_PIN_B02_USBD_DP  (PIN_B02 | 1)

#define PIN_B03  ((PORT_B << PORT_POS) | (PIN_3 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_B03_GPIO     (PIN_B03 | 0)

#define PIN_B04  ((PORT_B << PORT_POS) | (PIN_4 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_B04_GPIO     (PIN_B04 | 0)

#define PIN_B05  ((PORT_B << PORT_POS) | (PIN_5 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_B05_GPIO     (PIN_B05 | 0)

#define PIN_B06  ((PORT_B << PORT_POS) | (PIN_6 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_B06_GPIO     (PIN_B06 | 0)

#define PIN_B07  ((PORT_B << PORT_POS) | (PIN_7 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_B07_GPIO     (PIN_B07 | 0)

#define PIN_B08  ((PORT_B << PORT_POS) | (PIN_8 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_B08_GPIO     (PIN_B08 | 0)

#define PIN_B09  ((PORT_B << PORT_POS) | (PIN_9 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_B09_GPIO     (PIN_B09 | 0)

#define PIN_B10  ((PORT_B << PORT_POS) | (PIN_10 << PIN_POS))

#define PIN_B11  ((PORT_B << PORT_POS) | (PIN_11 << PIN_POS))

#define PIN_B12  ((PORT_B << PORT_POS) | (PIN_12 << PIN_POS))

#define PIN_B13  ((PORT_B << PORT_POS) | (PIN_13 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_B13_GPIO     (PIN_B13 | 0)

#define PIN_B14  ((PORT_B << PORT_POS) | (PIN_14 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_B14_GPIO     (PIN_B14 | 0)

#define PIN_B15  ((PORT_B << PORT_POS) | (PIN_15 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_B15_GPIO      (PIN_B15 | 0)



/*****************************************/
/************  PORT C ********************/
#define PIN_C00  ((PORT_C << PORT_POS) | (PIN_0 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_C00_GPIO              (PIN_C00 | 0)

#define PIN_C01  ((PORT_C << PORT_POS) | (PIN_1 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_C01_GPIO              (PIN_C01 | 0)

#define PIN_C02  ((PORT_C << PORT_POS) | (PIN_2 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_C02_GPIO               (PIN_C02 | 0)

#define PIN_C03  ((PORT_C << PORT_POS) | (PIN_3 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_C03_GPIO              (PIN_C03 | 0)

#define PIN_C04  ((PORT_C << PORT_POS) | (PIN_4 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_C04_GPIO               (PIN_C04 | 0)

#define PIN_C05  ((PORT_C << PORT_POS) | (PIN_5 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_C05_GPIO     (PIN_C05 | 0)

#define PIN_C06  ((PORT_C << PORT_POS) | (PIN_6 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_C06_GPIO     (PIN_C06 | 0)

#define PIN_C07  ((PORT_C << PORT_POS) | (PIN_7 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_C07_GPIO     (PIN_C07 | 0)

#define PIN_C08  ((PORT_C << PORT_POS) | (PIN_8 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_C08_GPIO     (PIN_C08 | 0)

#define PIN_C09  ((PORT_C << PORT_POS) | (PIN_9 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_C09_GPIO     (PIN_C09 | 0)

#define PIN_C10  ((PORT_C << PORT_POS) | (PIN_10 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_C10_GPIO          (PIN_C10 | 0)

#define PIN_C11  ((PORT_C << PORT_POS) | (PIN_11 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_C11_GPIO          (PIN_C11 | 0)

#define PIN_C12  ((PORT_C << PORT_POS) | (PIN_12 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_C12_GPIO         (PIN_C12 | 0)

#define PIN_C13  ((PORT_C << PORT_POS) | (PIN_13 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_C13_GPIO         (PIN_C13 | 0)

#define PIN_C14  ((PORT_C << PORT_POS) | (PIN_14 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_C14_GPIO        (PIN_C14 | 0)

#define PIN_C15  ((PORT_C << PORT_POS) | (PIN_15 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_C15_GPIO     (PIN_C15 | 0)


/*****************************************/
/************  PORT D ********************/
#define PIN_D00  ((PORT_D << PORT_POS) | (PIN_0 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_D00_GPIO         (PIN_D00 | 0)

#define PIN_D01  ((PORT_D << PORT_POS) | (PIN_1 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_D01_GPIO         (PIN_D01 | 0)

#define PIN_D02  ((PORT_D << PORT_POS) | (PIN_2 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_D02_GPIO              (PIN_D02 | 0)

#define PIN_D03  ((PORT_D << PORT_POS) | (PIN_3 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_D03_GPIO              (PIN_D03 | 0)

#define PIN_D04  ((PORT_D << PORT_POS) | (PIN_4 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_D04_GPIO               (PIN_D04 | 0)

#define PIN_D05  ((PORT_D << PORT_POS) | (PIN_5 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_D05_GPIO              (PIN_D05 | 0)

#define PIN_D06  ((PORT_D << PORT_POS) | (PIN_6 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_D06_GPIO               (PIN_D06 | 0)

#define PIN_D07  ((PORT_D << PORT_POS) | (PIN_7 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_D07_GPIO     (PIN_D07 | 0)

#define PIN_D08  ((PORT_D << PORT_POS) | (PIN_8 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_D08_GPIO        (PIN_D08 | 0)

#define PIN_D09  ((PORT_D << PORT_POS) | (PIN_9 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_D09_GPIO        (PIN_D09 | 0)

#define PIN_D10  ((PORT_D << PORT_POS) | (PIN_10 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_D10_GPIO     (PIN_D10 | 0)

#define PIN_D11  ((PORT_D << PORT_POS) | (PIN_11 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_D11_GPIO     (PIN_D11 | 0)

#define PIN_D12  ((PORT_D << PORT_POS) | (PIN_12 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_D012_GPIO     (PIN_D12 | 0)

#define PIN_D13  ((PORT_D << PORT_POS) | (PIN_13 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_D13_GPIO     (PIN_D13 | 0)

#define PIN_D14  ((PORT_D << PORT_POS) | (PIN_14 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_D14_GPIO     (PIN_D14 | 0)

#define PIN_D15  ((PORT_D << PORT_POS) | (PIN_15 << PIN_POS))
#define PIN_CONTROL_DT_I96XXX_PIN_D15_GPIO     (PIN_D15 | 0)




/**********  define API  types ************/


/**********  define API  functions  ************/


#endif
