
#include "main.h"

#include "enable1.h"
#include "led_breath.h"
#include "led_pwm.h"
#include "mosfet_pwm.h"

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi1_tx;

HCD_HandleTypeDef hhcd_USB_OTG_FS;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_USB_OTG_FS_HCD_Init(void);

int main(void) {
    /* MCU
     * Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the
     * Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_I2C1_Init();
    MX_SPI1_Init();
    MX_USB_OTG_FS_HCD_Init();

    enable_gpio_pin_always_high();  // Configure PA2 to output 3.3V
    LED2_PWM_Init();
    MOSFET_PWM_Init();
    LED2_Breath_Init();

    /* Infinite loop */
    while (1) {
        LED2_Breath_Update();
        HAL_Delay(BOOST_MAIN_LOOP_DELAY_MS); /* Pace the breathing animation. */
    }
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    if (HAL_PWREx_ControlVoltageScaling(BOOST_VOLTAGE_SCALING) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = BOOST_RCC_OSCILLATOR_TYPE;
    RCC_OscInitStruct.MSIState = BOOST_RCC_MSI_STATE;
    RCC_OscInitStruct.MSICalibrationValue = BOOST_RCC_MSI_CALIBRATION;
    RCC_OscInitStruct.MSIClockRange = BOOST_RCC_MSI_CLOCK_RANGE;
    RCC_OscInitStruct.PLL.PLLState = BOOST_RCC_PLL_STATE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = BOOST_RCC_CLOCK_TYPE;
    RCC_ClkInitStruct.SYSCLKSource = BOOST_RCC_SYSCLK_SOURCE;
    RCC_ClkInitStruct.AHBCLKDivider = BOOST_RCC_AHBCLK_DIVIDER;
    RCC_ClkInitStruct.APB1CLKDivider = BOOST_RCC_APB1CLK_DIVIDER;
    RCC_ClkInitStruct.APB2CLKDivider = BOOST_RCC_APB2CLK_DIVIDER;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, BOOST_FLASH_LATENCY) !=
        HAL_OK) {
        Error_Handler();
    }
}

static void MX_I2C1_Init(void) {
    hi2c1.Instance = BOOST_I2C1_INSTANCE;
    hi2c1.Init.Timing = BOOST_I2C1_TIMING;
    hi2c1.Init.OwnAddress1 = BOOST_I2C1_OWN_ADDRESS1;
    hi2c1.Init.AddressingMode = BOOST_I2C1_ADDRESSING_MODE;
    hi2c1.Init.DualAddressMode = BOOST_I2C1_DUAL_ADDRESS_MODE;
    hi2c1.Init.OwnAddress2 = BOOST_I2C1_OWN_ADDRESS2;
    hi2c1.Init.OwnAddress2Masks = BOOST_I2C1_OWN_ADDRESS2_MASKS;
    hi2c1.Init.GeneralCallMode = BOOST_I2C1_GENERAL_CALL_MODE;
    hi2c1.Init.NoStretchMode = BOOST_I2C1_NO_STRETCH_MODE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Analogue filter
     */
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, BOOST_I2C1_ANALOG_FILTER) !=
        HAL_OK) {
        Error_Handler();
    }

    /** Configure Digital filter
     */
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, BOOST_I2C1_DIGITAL_FILTER) !=
        HAL_OK) {
        Error_Handler();
    }
}

static void MX_SPI1_Init(void) {
    hspi1.Instance = BOOST_SPI1_INSTANCE;
    hspi1.Init.Mode = BOOST_SPI1_MODE;
    hspi1.Init.Direction = BOOST_SPI1_DIRECTION;
    hspi1.Init.DataSize = BOOST_SPI1_DATA_SIZE;
    hspi1.Init.CLKPolarity = BOOST_SPI1_CLK_POLARITY;
    hspi1.Init.CLKPhase = BOOST_SPI1_CLK_PHASE;
    hspi1.Init.NSS = BOOST_SPI1_NSS;
    hspi1.Init.BaudRatePrescaler = BOOST_SPI1_BAUDRATE_PRESCALER;
    hspi1.Init.FirstBit = BOOST_SPI1_FIRST_BIT;
    hspi1.Init.TIMode = BOOST_SPI1_TI_MODE;
    hspi1.Init.CRCCalculation = BOOST_SPI1_CRC_CALCULATION;
    hspi1.Init.CRCPolynomial = BOOST_SPI1_CRC_POLYNOMIAL;
    hspi1.Init.CRCLength = BOOST_SPI1_CRC_LENGTH;
    hspi1.Init.NSSPMode = BOOST_SPI1_NSSP_MODE;
    if (HAL_SPI_Init(&hspi1) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_USB_OTG_FS_HCD_Init(void) {
    hhcd_USB_OTG_FS.Instance = BOOST_USB_OTG_FS_INSTANCE;
    hhcd_USB_OTG_FS.Init.Host_channels = BOOST_USB_OTG_FS_HOST_CHANNELS;
    hhcd_USB_OTG_FS.Init.speed = BOOST_USB_OTG_FS_SPEED;
    hhcd_USB_OTG_FS.Init.dma_enable = BOOST_USB_OTG_FS_DMA_ENABLE;
    hhcd_USB_OTG_FS.Init.phy_itface = BOOST_USB_OTG_FS_PHY_INTERFACE;
    hhcd_USB_OTG_FS.Init.Sof_enable = BOOST_USB_OTG_FS_SOF_ENABLE;
    if (HAL_HCD_Init(&hhcd_USB_OTG_FS) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {
    /* DMA controller clock enable */
    BOOST_DMA1_ENABLE_CLOCK();

    HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, BOOST_DMA1_CH2_PRIORITY,
                         BOOST_DMA1_CH2_SUBPRIORITY);
    HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
    /* DMA1_Channel3_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, BOOST_DMA1_CH3_PRIORITY,
                         BOOST_DMA1_CH3_SUBPRIORITY);
    HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
}

static void MX_GPIO_Init(void) {
    /* GPIO Ports Clock Enable */
    BOOST_GPIO_ENABLE_PORTS();
}

void Error_Handler(void) {
    /* User can add his own implementation to report the HAL error return state
     */
    __disable_irq();
    while (1) {
    }
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line
       number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
       file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
