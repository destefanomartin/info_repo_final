// Lector pasaportes y Driver luz 

// Ejercicio 1 Driver luz - 120 Hz - Se cambio enunciado por imposibilidad de resolver 


void InicializacionPWM(uint16_t div, uint8_t pin)
{
    // Para este ejemplo div = 120000 -> 20 ms -> 120Hz 
    // Para mi en la inicializacion comprende setear pin y frecuencia o periodo 
    PWM_Reg &= ~(0x01); 
    PWM_Reg |= (pin << 10); 
    PWM_Reg |= div << 15;  

}

void SetIntensity(uint8_t time_on)
{
    PWM_Reg |= (time_on << 3) 
}

void ADC_Handler(void)
{
    // Se lee el registro del ADC -> Se debe conocer la cantidad de cuentas y su respectivo valor maximo y hacer el calculo de cuanto debe ser el valor de Time_ON 
}

// Ejercicio 2 - Pasaportes 

#define REPOSO 0 
#define LECTURA 1 
#define TIEMPO_LECTURA 15000
#define TIEMPO_COOLDOWN 20000
#define PORT_SENSOR 0
#define PIN_SENSOR 3 

uint16_t timer_lectura; 
uint16_t timer_cooldown; 
uint8_t trama_recibida; 




void Maq_Estados_Principal(void)
{
    static uint8_t ESTADO; 

    switch (ESTADO)
    {
    case REPOSO:
        if(checkSensor == 1)
        {
            lightON(); 
            timer_lectura = TIEMPO_LECTURA; 
            ESTADO = LECTURA; 
        }
        break;
    case LECTURA: 
        if(trama_recibida == 0 && timer_lectura == 0)
        {
            timer_cooldown = TIEMPO_COOLDOWN;
            ESTADO = REPOSO;  
            break; 
        }
        if(trama_recibida == 1)
        {
            enviar_trama(); 
            ESTADO = REPOSO; 
        }
        break;
    default:
        break;
    }
}


void Systick_Handler(void)
{
    if(timer_lectura > 0)
    {
        timer_lectura--; 
    }
    if(timer_cooldown > 0)
    {
        timer_cooldown--; 
    }
}

uint8_t checkSensor()
{
    return(GetPin(PORT_SENSOR,PIN_SENSOR)); 
}

void Recepcion_Trama()
{
    uint8_t dato = PopRx(); 
    static uint8_t ESTADO_TRAMA; 
    switch (ESTADO_TRAMA)
    {
    case REPOSO:
        if(dato == '#') ESTADO_TRAMA=NOMBRE; 
        break;
    
    default:
        break;
    }
}