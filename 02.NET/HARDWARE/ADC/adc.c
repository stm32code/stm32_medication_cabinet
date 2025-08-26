#include "adc.h"
#include "delay.h"

__IO uint16_t ADC_ConvertedValue[NOFCHANEL] = {0, 0};
extern Data_TypeDef Data_init; // �豸���ݽṹ��
/**
 * @brief  ADC GPIO ��ʼ��
 * @param  ��
 * @retval ��
 */
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// �� ADC IO�˿�ʱ��
	ADC_GPIO_APBxClock_FUN(ADC_GPIO_CLK, ENABLE);

	// ���� ADC IO ����ģʽ
	GPIO_InitStructure.GPIO_Pin = ADC_PIN1 |
								  ADC_PIN2;
	//   ADC_PIN3 |
	//   ADC_PIN4 |
	//   ADC_PIN5 |
	//   ADC_PIN6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;

	// ��ʼ�� ADC IO
	GPIO_Init(ADC_PORT, &GPIO_InitStructure);
}

/**
 * @brief  ����ADC����ģʽ
 * @param  ��
 * @retval ��
 */
static void ADCx_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	// ��DMAʱ��
	RCC_AHBPeriphClockCmd(ADC_DMA_CLK, ENABLE);
	// ��ADCʱ��
	ADC_APBxClock_FUN(ADC_CLK, ENABLE);

	// ��λDMA������
	DMA_DeInit(ADC_DMA_CHANNEL);

	// ���� DMA ��ʼ���ṹ��
	// �����ַΪ��ADC ���ݼĴ�����ַ
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(ADC_x->DR));

	// �洢����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADC_ConvertedValue;

	// ����Դ��������
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;

	// ��������С��Ӧ�õ�������Ŀ�ĵصĴ�С
	DMA_InitStructure.DMA_BufferSize = NOFCHANEL;

	// ����Ĵ���ֻ��һ������ַ���õ���
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

	// �洢����ַ����
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;

	// �������ݴ�СΪ���֣��������ֽ�
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;

	// �ڴ����ݴ�СҲΪ���֣����������ݴ�С��ͬ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;

	// ѭ������ģʽ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;

	// DMA ����ͨ�����ȼ�Ϊ�ߣ���ʹ��һ��DMAͨ��ʱ�����ȼ����ò�Ӱ��
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;

	// ��ֹ�洢�����洢��ģʽ����Ϊ�Ǵ����赽�洢��
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	// ��ʼ��DMA
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);

	// ʹ�� DMA ͨ��
	DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);

	// ADC ģʽ����
	// ֻʹ��һ��ADC�����ڵ�ģʽ
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;

	// ɨ��ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;

	// ����ת��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;

	// �����ⲿ����ת���������������
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;

	// ת������Ҷ���
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;

	// ת��ͨ������
	ADC_InitStructure.ADC_NbrOfChannel = NOFCHANEL;

	// ��ʼ��ADC
	ADC_Init(ADC_x, &ADC_InitStructure);

	// ����ADCʱ�ӣΪPCLK2��8��Ƶ����9MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);

	// ����ADC ͨ����ת��˳��Ͳ���ʱ��
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL1, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL2, 2, ADC_SampleTime_55Cycles5);
	// ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL3, 3, ADC_SampleTime_55Cycles5);
	// ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL4, 4, ADC_SampleTime_55Cycles5);
	// ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL5, 5, ADC_SampleTime_55Cycles5);
	// ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL6, 6, ADC_SampleTime_55Cycles5);

	// ʹ��ADC DMA ����
	ADC_DMACmd(ADC_x, ENABLE);

	// ����ADC ������ʼת��
	ADC_Cmd(ADC_x, ENABLE);

	// ��ʼ��ADC У׼�Ĵ���
	ADC_ResetCalibration(ADC_x);
	// �ȴ�У׼�Ĵ�����ʼ�����
	while (ADC_GetResetCalibrationStatus(ADC_x))
		;

	// ADC��ʼУ׼
	ADC_StartCalibration(ADC_x);
	// �ȴ�У׼���
	while (ADC_GetCalibrationStatus(ADC_x))
		;

	// ����û�в����ⲿ����������ʹ���������ADCת��
	ADC_SoftwareStartConvCmd(ADC_x, ENABLE);
}

/**
 * @brief  ADC��ʼ��
 * @param  ��
 * @retval ��
 */
void ADCx_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_Mode_Config();
}
F32 TDS_value = 0.0, voltage_value;
F32 compensationCoefficient = 1.0; // �¶�У׼ϵ��
F32 compensationVolatge;
F32 kValue = 1.67;
// ���ڱ���ת�������ĵ�ѹֵ
float ADC_ConvertedValueLocal[4];
/**************TDSֵ�ɼ�����***************/
F32 TDS_Value_Conversion(u16 Value)
{
	ADC_ConvertedValueLocal[0] = (float)Value / 4096 * 3.3; // ADת��
	compensationCoefficient = 1.0 + 0.02 * ((Data_init.muddy * 10) - 25.0);
	compensationVolatge = ADC_ConvertedValueLocal[0] / compensationCoefficient;

	TDS_value = (133.42 * compensationVolatge * compensationVolatge * compensationVolatge -
				 255.86 * compensationVolatge * compensationVolatge + 857.39 * compensationVolatge) *
				0.5 * kValue;

	if ((TDS_value <= 0))
	{
		TDS_value = 0;
	}
	if ((TDS_value > 1400))
	{
		TDS_value = 1400;
	}
	// TDS_value = TDS_value;
	// TDS_value = (int)(TDS_value) / 1000 + (int)(TDS_value) % 1000 / 100 +
	// 			(int)(TDS_value * 100) % 100 / 10 + (int)(TDS_value * 100) % 10; // ��ȡ����
	return TDS_value;
}
// ����ʪ��
F32 Offset = 0.00; // deviation compensate
F32 phValue = 0;
float Get_Adc_Average(u16 Value, u8 times) // ��ȡ����ʪ�����ݲ����ظ�������
{
	//	ADC =(float) ADC_ConvertedValue/4096*3.3;
	phValue = (float)Value / 4096 * 3.3; // ADת��
	phValue = 3.5 * phValue + Offset;	 // convert the millivolt into pH value
	return phValue;
}
/*********************************************END OF FILE**********************/
