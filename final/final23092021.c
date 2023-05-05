// Declaraciones display 

#define segmento_a P3.0 
#define segmento_b P3.1 
#define segmento_c P3.2 
#define segmento_d P3.3 
#define segmento_e P3.4 
#define segmento_f P3.5 
#define segmento_g P3.6
#define segmento_dp P3.7 

#define DIGITO1 P4.6 
#define DIGITO2 P2.1 
#define DIGITO3 P0.24 

#define Dig1 0 
#define Dig2 1 
#define Dig3 2 

uint8_t MsgDisplay[3]
// Declaraciones control de temperatura
#define RELE_TEMP P0.23
#define SENSOR_TEMP AD0.0

// Declaraciones de elementos interactivos 
#define BUTTON_1 P0.0
#define BUTTON_2 P0.1
#define BUZZER P0.2 

// uchar = unsigned char 
uchar code Disp7[ ] = { 0x3f, 0x06, 0x5B,0x4f, 0x66, 0x6D, 0x7C, 0x07, 0x7f, 0x67};

// Realizar la funciona main 

void main(args, argv[])
{
    Inicializacion(); // No tengo que realizar estas funciones pero detallo donde deberia ser invocada la funcion

    while(1)
    {
        Maq_Estado_Principal(); 
    }
}

// Funciones timertick 

void TimerHandler()
{

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
        SetPin(DIGITO2, ON)
        break;
    default:
        break;
    }
    digito++; 
    digito %= DIGITOS; 
}

void Display(uint8_t Valor)
{
    uint8_t i;

    for(i=0; i<DIGITOS; i++, val/10)
    {
        MsgDisplay[i] = Disp7[val%=10]; 
    } 
}

// Funciones de control (faltan las de pulsadores)
void Maq_Estado_Principal()
{
    
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

void EnvioTrama()
{
    uint8_t ESTADO, dato; 

    switch (ESTADO)
    {
    case INICIO:
        dato = caracter_inicio; 
        PushTX(dato);
        break;
        ESTADO = ENVIO_TEMPERATURA; 
    case ENVIO_TEMPERATURA: 
        dato = temp_medida; 
        PushTX(dato);
        ESTADO = ENVIO_TIEMPO_LOW; 
    case ENVIO_TIEMPO_LOW: 
        dato = low_time_data; 
        PushTX(dato); 
        ESTADO = ENVIO_TIEMPO_UP; 
    case ENVIO_TIEMPO_UP: 
        dato = up_time_data; 
        PushTX(dato); 
        ESTADO = CHECKSUM;
    case CHECKSUM: 
        dato = caracter_inicio + temp_medida + low_time_data 
        + up_time_data; 
        PushTX(dato); 
        ESTADO = FINAL_TRAMA; 
    case FINAL_TRAMA: 
        dato = "<"
        PushTX(dato); 
        ESTADO = REPOSO;        
        break;  
    default:
        break;
    }
}
// Funciones lectura conversor AD 
