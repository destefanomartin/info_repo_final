/* FINAL YOGURTERA - 23 de Septiembre de 2021 - Resolucion: Destefano Martin  */



// Typedef de los tipos 
typedef unsigned char uint8_t; 
typedef unsigned short int uint16_t; 
typedef unsigned char uchar; 
typedef unsigned int uint32_t; 

// Declaraciones display
#define segmento_a P3,0 
#define segmento_b P3,1 
#define segmento_c P3,2 
#define segmento_d P3,3 
#define segmento_e P3,4 
#define segmento_f P3,5 
#define segmento_g P3,6
#define segmento_dp P3,7 

#define DIGITO1 P4,6 
#define DIGITO2 P2,1 
#define DIGITO3 P0,24 

#define Dig1 0 
#define Dig2 1 
#define Dig3 2 

#define DIGITOS 3 

uint8_t MsgDisplay[3]; 
// Declaraciones control de temperatura
#define RELE_TEMP P0.23
#define SENSOR_TEMP AD0.0
// Declaraciones para timers 
#define VALOR_TIEMPO 1000 
#define INCREMENTO_30m 1800000
#define MAX_TIME 54000000
#define MIN_TIME 21600000
// Declaraciones de elementos interactivos 
#define PORT1 0
#define PIN1 0
#define PORT2 0
#define PIN2 1
#define BUZZER P0.2 
#define ON 1 
#define OFF 0

#define MAX_TEMP 43 
#define MIN_TEMP 38

// uchar = unsigned char 
uint8_t code Disp7[] = { 0x3f, 0x06, 0x5B,0x4f, 0x66, 0x6D, 0x7C, 0x07, 0x7f, 0x67};
extern volatile MsgDisplay[DIGITOS]; 
uint8_t Timer_1S; 
uint16_t Temperatura; 
uint32_t Tiempo_sensado = MIN_TIME;  
uint8_t Flag_Puls1 = 0; 
uint8_t Flag_Puls2 = 0; 
// Realizar la funciona main 

void main(args, argv[])
{
    Inicializacion(); // No tengo que realizar estas funciones pero detallo donde deberia ser invocada la funcion

    while(1)
    {
        Maq_Estado_Principal(); 
    }
}

// Funciones display 

void BarridoDisplay()
{
    uint8_t auxiliar; 
    static uint8_t digito = 0; 
    SetPin(DIGITO1, OFF); SetPin(DIGITO2, OFF); SetPin(DIGITO3, OFF); 

    auxiliar = MsgDisplay[digito]; 
    SetPin(segmento_a, (auxiliar & (uint8_t) 0x01));
    SetPin(segmento_b, ((auxiliar >> 1) & (uint8_t) 0x01));
    SetPin(segmento_c, ((auxiliar >> 2) & (uint8_t) 0x01));
    SetPin(segmento_d, ((auxiliar >> 3) & (uint8_t) 0x01));
    SetPin(segmento_e, ((auxiliar >> 4) & (uint8_t) 0x01));
    SetPin(segmento_f, ((auxiliar >> 5) & (uint8_t) 0x01));
    SetPin(segmento_g, ((auxiliar >> 6) & (uint8_t) 0x01));
    SetPin(segmento_dp, ((auxiliar >> 7) & (uint8_t) 0x01));

    switch (digito)
    {
    case Dig1:
        SetPin(DIGITO1, ON)
        break;
    case Dig2:
        SetPin(DIGITO2, ON)
        break;
    case Dig3:
        SetPin(DIGITO3, ON)
        break;
    default:
        break;
    }
    digito++; 
    digito %= DIGITOS; 
}

void Display(uint32_t Valor)
{
    uint8_t i;

    for(i=0; i<DIGITOS; i++, Valor/=10)
    {
        MsgDisplay[i] = Disp7[Valor%10]; 
    } 
}


enum ESTADO {REPOSO, SENSANDO}; 
// Funciones de control (faltan las de pulsadores)
void Maq_Estado_Principal()
{
    static uint8_t ESTADO; 

    switch (ESTADO)
    {
    case REPOSO:
        if(Flag_Puls2 == 1)
        {
            Flag_Puls2 = 0; 
            if(Tiempo_sensado == MAX_TIME)
            {
                Tiempo_sensado = MIN_TIME; 
            }
            else Tiempo_sensado += INCREMENTO_30m; 
        }
        if(Flag_Puls1 == 1)
        {
            ESTADO = SENSANDO; 
        }
    case SENSANDO: 
        if(Tiempo_sensado != 0)
        {
            if(Temperatura > MAX_TEMP)
            {
                ApagarResistencia(); 
            }
            if(Temperatura < MIN_TEMP)
            {
                EncenderResistencia(); 
            }
        }
        else 
        {
            ApagarResistencia(); 
            EncenderBuzzer(); 
            Tiempo_sensado = MIN_TIME; 
            ESTADO = REPOSO; 
        }
    default:
        ESTADO = REPOSO; 
        break;
    }
}

void Verificar_Pulsadores()
{
    if((GetPin(PORT1,PIN1)) == 1)
    {
        Flag_Puls1 = 1; 
    }
    if((GetPin(PORT2,PIN2)) == 1)
    {
        Flag_Puls2 = 1; 
    } 
}
void EncenderResistencia()
{
    SetPin(RELE_TEMP, ON); 
}

void ApagarResistencia()
{
    SetPin(RELE_TEMP, OFF); 
}

void EncenderBuzzer()
{
    SetPin(BUZZER, ON); 
}

void ApagarBuzzer()
{
    SetPin(BUZZER, OFF); 
}

// Funciones envio trama 

enum ESTADO {INICIO, ENVIO_TEMPERATURA, ENVIO_TIEMPO_LOW, ENVIO_TIEMPO_UP, CHECKSUM, FINAL_TRAMA}; 

void EnvioTrama()
{
    static uint8_t ESTADO, dato; 
    uint32_t checksum; 
    switch (ESTADO)
    {
    case INICIO:
        dato = '>'; 
        checksum += dato; 
        PushTX(dato);
        break;
        ESTADO = ENVIO_TEMPERATURA; 
    case ENVIO_TEMPERATURA: 
        dato = Temperatura;
        checksum += dato;  
        PushTX(dato);
        ESTADO = ENVIO_TIEMPO_LOW; 
    case ENVIO_TIEMPO_LOW: 
        dato = low_time_data; 
        checksum += dato; 
        PushTX(dato); 
        ESTADO = ENVIO_TIEMPO_UP; 
    case ENVIO_TIEMPO_UP: 
        dato = up_time_data; 
        checksum += dato; 
        PushTX(dato); 
        ESTADO = CHECKSUM;
    case CHECKSUM: 
        dato = checksum; 
        PushTX(dato); 
        ESTADO = FINAL_TRAMA; 
    case FINAL_TRAMA: 
        dato = "<"; 
        PushTX(dato); 
        ESTADO = REPOSO;        
        break;  
    default:
        break;
    }
}
// Funciones lectura conversor AD 

void ADC_Handler(void)
{
    uint16_t value = 0; 
    value |= (AD0DR0 << 0x04) & 0xFFFF; 
    Temperatura = (100/4096)*value; 
}


void SysTick_Handler(void)
{
    BarridoDisplay(); 
    Verificar_Pulsadores();
    if(Flag_Puls1 == 1)
    {
        Timer_1S--; 
    }
    if(Timer_1S == 0)
    {
        Timer_1S = VALOR_TIEMPO; 
        EnvioTrama(); 
    }
}