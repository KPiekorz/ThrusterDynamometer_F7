/*
 * controlpanel.c
 *
 *  Created on: 23.09.2019
 *      Author: Konto_U¿ytkowe
 */

#include "controlpanel.h"


extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim9;
extern TIM_HandleTypeDef htim11;
extern TIM_HandleTypeDef htim2;
extern ADC_HandleTypeDef hadc1;

static char received_command[100];

const float V25 = 0.76; // [Volts]
const float Avg_slope = 0.025; //[Volts/degree]
const float SupplyVoltage = 3.0; // [Volts]
const float ADCResolution = 4095.0;

static uint32_t count_adc = 0;
static uint16_t vibro_value_table[20];
static uint16_t hal_value_table[20];



/*
 * task do wysylania danych z czujnikow
 */
void vTaskSendData(void *p){

	char sens_value_frame[1500];
	uint16_t frame_length = 0;
	uint32_t ulNotifiedValue = 0x0;
	uint32_t start_ms = 0;

	if(xSemaphoreTake(xMutexSensValue, portMAX_DELAY) == 1){

//		sens_value.vibro_value = 0;
//		sens_value.hal_value = 0;
		sens_value.shunt_value = 0;
		sens_value.tenso_value = 0;
		sens_value.temp_F7 = 0;
		strncpy(sens_value.temp, "", sizeof(sens_value.temp));

		xSemaphoreGive(xMutexSensValue);
	}

	reset_time = 1;

	while(1){

		if( xTaskNotifyWait( 0xFFFFFFFF, 0, &ulNotifiedValue, portMAX_DELAY) == pdTRUE ){

			if(xSemaphoreTake(xMutexSensValue, portMAX_DELAY) == 1){

				memset(sens_value_frame, '\0', sizeof(sens_value_frame));

				if (reset_time == 1){
					start_ms = HAL_GetTick();
					reset_time = 0;
				}

				frame_length = sprintf(sens_value_frame, "<%s|%s|%d|%d|%d|%d|%d>\r\n",
						get_time(start_ms), sens_value.temp, sens_value.vibro_value,
						sens_value.shunt_value, sens_value.hal_value,
						sens_value.tenso_value, sens_value.temp_F7);

				if (accept_err == ERR_OK) {

					netconn_write(newconn, (const unsigned char* )sens_value_frame, frame_length, NETCONN_COPY);

				}

				xSemaphoreGive(xMutexSensValue);
			}

		}else{



		}

	}

}

/*
 * Przetworzenie odbebranych danych i wlaczenie odpowiedniej funkcji
 */
void vTaskReceivedData(void *p){

	CMD_MODE_t *command_mode;
	uint32_t ulNotifiedValue = 0x0;

	struct netbuf* buffer;
	void* data;
	u16_t len;
	err_t recv_err;

	vTaskSuspend(NULL);

	while(1){


		//Obsluga polaczenia
		if (accept_err == ERR_OK) {

			while ((recv_err = netconn_recv(newconn, &buffer)) == ERR_OK) {

				do {

					netbuf_data(buffer, &data, &len);

				} while (netbuf_next(buffer) >= 0);

				netbuf_delete(buffer);

				command_mode = (CMD_MODE_t*) pvPortMalloc(sizeof(CMD_MODE_t));
				extract_arg(command_mode, data);

				memset(usr_msg, '\0', sizeof(usr_msg));
				sprintf(usr_msg, "Oderana wiadomosc : %d  %d  %d  %d  %d\r\n",
						command_mode->num, command_mode->arg,
						command_mode->rise_time, command_mode->stay_time,
						command_mode->fall_time);


				netconn_write(newconn, usr_msg, strlen(usr_msg), NETCONN_COPY);


			switch (command_mode->num) {

			case START_COMMAND: {

				HAL_TIM_Base_Start(&htim2); // adc
				HAL_ADC_Start_DMA(&hadc1, adcValue, 4);


				HAL_TIM_Base_Start_IT(&htim11); // send
				HAL_TIM_Base_Start_IT(&htim9); // tenso

				break;
			}
			case STOP_COMMAND: {

				HAL_ADC_Stop_DMA(&hadc1);
				HAL_TIM_Base_Stop(&htim2);

				HAL_TIM_Base_Stop_IT(&htim9);
				reset_time = 1; HAL_TIM_Base_Stop_IT(&htim11);

				if(xSemaphoreTake(xMutexSensValue, portMAX_DELAY) == 1){

//					sens_value.vibro_value = 0;
//					sens_value.hal_value = 0;
					sens_value.shunt_value = 0;
					sens_value.tenso_value = 0;
					sens_value.temp_F7 = 0;

					xSemaphoreGive(xMutexSensValue);
				}

				break;
			}
			case CHANGE_DUTY_COMMAND: {
				duty = command_mode->arg;
				break;
			}
			case TENSO_OFFSET_COMMAND: {
				HX711_Tare(10);
				break;
			}
			case TENSO_CALIBRATION_COMMAND: {
				HX711_Calibration(command_mode->arg, HX711_Average_Value(10));
				break;
			}
			case SHORT_TEST_COMMAND: {

				if(xSemaphoreTake(xMutexBLDC, 0) == 1){

					rise = command_mode->rise_time;
					stay = command_mode->stay_time;
					fall = command_mode->fall_time;

					xTaskNotify(xTaskBLDCHandle, 0x1, eSetValueWithoutOverwrite);

					xSemaphoreGive(xMutexBLDC);
				}

				break;
			}
			case LONG_TEST_COMMAND: {

				if(xSemaphoreTake(xMutexBLDC, 0) == 1){

					rise = command_mode->rise_time;
					fall = command_mode->fall_time;

					xTaskNotify(xTaskBLDCHandle, 0x2, eSetValueWithoutOverwrite);

					xSemaphoreGive(xMutexBLDC);
				}

				break;
			}
			case THROTTLE_TEST_COMMAND: {

				if(xSemaphoreTake(xMutexBLDC, 0) == 1){

					rise = command_mode->rise_time;
					fall = command_mode->fall_time;

					xTaskNotify(xTaskBLDCHandle, 0x3, eSetValueWithoutOverwrite);

					xSemaphoreGive(xMutexBLDC);
				}

				break;
			}
			case START_PWM: {
				HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
				duty = 0;
				HAL_TIM_PWM_Start_DMA(&htim8, TIM_CHANNEL_1, &duty, 1);

				break;
			}
			case STOP_PWM: {
				HAL_TIM_PWM_Stop_DMA(&htim8, TIM_CHANNEL_1);
				break;
			}
			default: {

				break;
			}

			}

			vPortFree(command_mode);

			}

		}


	}

}

/*
 * Odczyt danych z czujnikow wykorzystujacych ADC
 */
void vTaskADC(void *p){

	uint32_t ulNotifiedValue = 0x0;


	while(1){

		if( xTaskNotifyWait( 0xFFFFFFFF, 0, &ulNotifiedValue, portMAX_DELAY) == pdTRUE ){

			/*
			 * Odczyt z wibracji co 1ms, z hal i shunt co 100 ms, w rytm timerów, ktore beda
			 * uzywac funkcji xtasknotify()
			 */

			if(xSemaphoreTake(xMutexSensValue, portMAX_DELAY) == 1){

				Vsense = (SupplyVoltage*adcValue[3])/ADCResolution;
				Temperature = ((Vsense-V25)/Avg_slope)+25;

				sens_value.temp_F7 = (int)Temperature;

				/*
				 * osobne tablice na dane z czujnikow w przerwniu i
				 * tutaj zamiana na gotowy string do wyslania
				 */

//				sens_value.vibro_value = adcValue[0]; // vibro - adc1_ch3
//				sens_value.shunt_value = adcValue[1]; // shunt - adc1_ch4
//				sens_value.hal_value =   adcValue[2]; // hal - adc1_ch6



				xSemaphoreGive(xMutexSensValue);
			}

		}else{

		}

	}

}

/*
 * Obsluga automatycznych trybow pracy hamowni
 */
void vTaskBLDC(void *p){

	uint32_t ulNotifiedValue = 0x0;
	uint16_t inc;

	while(1){


		if( xTaskNotifyWait( 0xFFFFFFFF, 0, &ulNotifiedValue, portMAX_DELAY) == pdTRUE ){

			if(xSemaphoreTake(xMutexSensValue, portMAX_DELAY) == 1){

			duty = DUTY_MIN;

			switch(ulNotifiedValue) {
			case 1: { // short

				inc = (DUTY_MAX - DUTY_MIN)/(rise - 1);
				for(int i = 0; i <= rise; i++){
					vTaskDelay(1000);
					duty += inc;
				}
				vTaskDelay(stay*1000);
				inc = (DUTY_MAX - DUTY_MIN)/(fall - 1);
				for(int i = 0; i <= fall; i++){
					vTaskDelay(1000);
					duty -= inc;
				}

				break;
			}
			case 2: { // long

				inc = (DUTY_MAX - DUTY_MIN)/(rise - 1);
				for(int i = 0; i <= rise; i++){
					vTaskDelay(1000);
					duty += inc;
				}
				vTaskDelay(60000);
				inc = (DUTY_MAX - DUTY_MIN)/(fall - 1);
				for(int i = 0; i <= fall; i++){
					vTaskDelay(1000);
					duty -= inc;
				}

				break;
			}
			case 3: { // throttle

				inc = (DUTY_MAX - DUTY_MIN)/(rise - 1);
				for(int i = 0; i <= rise; i++){
					vTaskDelay(1000);
					duty += inc;
				}
				inc = (DUTY_MAX - DUTY_MIN)/(fall - 1);
				for(int i = 0; i <= fall; i++){
					vTaskDelay(1000);
					duty -= inc;
				}

				break;
			}
			}

			xSemaphoreGive(xMutexSensValue);
			}

		}else{

		}

	}

}



/*
 * Odbior danych z temperatura od F4
 */
void vTaskTempF4UART(void *p){

	uint32_t ulNotifiedValue = 0x0;

	while(1){

		if( xTaskNotifyWait( 0xFFFFFFFF, 0, &ulNotifiedValue, portMAX_DELAY) == pdTRUE ){

			if (xSemaphoreTake(xMutexSensValue, portMAX_DELAY) == 1) {

				strncpy(sens_value.temp, "", sizeof(sens_value.temp));
				strcpy(sens_value.temp, uartf4_received);
				HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);

				xSemaphoreGive(xMutexSensValue);
			}

		}else{

		}

	}

}


/*
 * Odbior i odczyt danych z tensometru
 */
void vTaskTenso(void *p){

	uint32_t ulNotifiedValue = 0x0;

	while(1){


		if( xTaskNotifyWait( 0xFFFFFFFF, 0, &ulNotifiedValue, portMAX_DELAY) == pdTRUE ){


			if (xSemaphoreTake(xMutexSensValue, portMAX_DELAY) == 1) {

				sens_value.tenso_value = HX711_Value_Gram();

				xSemaphoreGive(xMutexSensValue);
			}

		}else{

		}

	}

}


/*******************Callback functions - USB and USART2**********************/
void USB_ReceivedCallBack(uint8_t *buf, uint32_t len){

	BaseType_t checkIfYieldRequired  = pdFALSE, xResult;;

	memset(received_command, '\0', sizeof(received_command));
	sprintf(received_command, "%s", buf);

	xResult = xTaskNotifyFromISR(xTaskReceivedDataHandle, 0, eNoAction, &checkIfYieldRequired);

	configASSERT( xResult == pdPASS );
	portYIELD_FROM_ISR(checkIfYieldRequired);

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){  // function (callback) - receiving from stm32 f4

	BaseType_t checkIfYieldRequired  = pdFALSE, xResult;

	if (huart->Instance == USART2) {

		xResult = xTaskNotifyFromISR(xTaskTempF4UARTHandle, 0, eNoAction, &checkIfYieldRequired);

		HAL_UART_Receive_DMA(&huart2, (uint8_t*) uartf4_received, UARTF4_TEMP_FRAME_SIZE);

		configASSERT( xResult == pdPASS );
		portYIELD_FROM_ISR(checkIfYieldRequired);

	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) // tim2 trigger adc1 with 10000 Hz
{
	BaseType_t checkIfYieldRequired = pdFALSE, xResult;

	if( count_adc == 20){

		xResult = xTaskNotifyFromISR(xTaskADCHandle, 0, eNoAction, &checkIfYieldRequired);

		count_adc = 0;

		configASSERT(xResult == pdPASS);
		portYIELD_FROM_ISR(checkIfYieldRequired);

	}else{

		vibro_value_table[count_adc] = adcValue[0];
		hal_value_table[count_adc] = adcValue[2];

	}

	count_adc++;

}

/****************************Additionals functions*****************************/
void extract_arg(CMD_MODE_t *cmd, char * received_command){

	char argument[4];

	memset(argument, '\0', sizeof(argument));
	uint8_t i = 1; uint8_t j = 0;
	while( received_command[i] != '|') {

		argument[j] = received_command[i];
		j++;
		i++;

	}
	argument[j] = '\0';
	sscanf(argument, "%d", &(cmd->num)); // numer komendy

	memset(argument, '\0', sizeof(argument));
	j = 0; i++;
	while (received_command[i] != '|') {

		argument[j] = received_command[i];
		j++;
		i++;

	}
	argument[j] = '\0';
	sscanf(argument, "%d", &(cmd->arg)); // duty lub tenso kalibracja

	memset(argument, '\0', sizeof(argument));
	j = 0; i++;
	while (received_command[i] != '|') {

		argument[j] = received_command[i];
		j++;
		i++;

	}
	argument[j] = '\0';
	sscanf(argument, "%d", &(cmd->rise_time)); // rise time

	memset(argument, '\0', sizeof(argument));
	j = 0; i++;
	while (received_command[i] != '|') {

		argument[j] = received_command[i];
		j++;
		i++;

	}
	argument[j] = '\0';
	sscanf(argument, "%d", &(cmd->stay_time)); // stay time

	memset(argument, '\0', sizeof(argument));
	j = 0; i++;
	while (received_command[i] != ']') {

		argument[j] = received_command[i];
		j++;
		i++;

	}
	argument[j] = '\0';
	sscanf(argument, "%d", &(cmd->fall_time)); // fall time

}

void usb_print(char * msg){
	char msg_to_send[100];
	memset(msg_to_send, '\0', sizeof(msg_to_send));
	sprintf(msg_to_send, "%s\r\n", msg);
	CDC_Transmit_FS(msg_to_send, strlen(msg_to_send));
}

void uart_print(char * msg){
	char msg_to_send[100];
	sprintf(msg_to_send, "%s\r\n", msg);
	HAL_UART_Transmit(&huart3, (uint8_t *) msg_to_send, strlen(msg_to_send), HAL_MAX_DELAY);
}

char * get_time(uint32_t start_ms){
	uint16_t min, sec, milisec;
	uint32_t sys_tick_ms = HAL_GetTick() - start_ms;


	milisec = sys_tick_ms % 1000;
	sec = (sys_tick_ms / 1000);
	min = sec / 60;
	sec = (sys_tick_ms / 1000) % 60;

	strncpy(current_time, "", sizeof(current_time));
	sprintf(current_time, "%d:%d,%d", min, sec, milisec);
	return current_time;
}







