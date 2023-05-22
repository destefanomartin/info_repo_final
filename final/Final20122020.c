typedef unsigned char uint8_t; 

volatile uint32_t Temperatura; 

#define TIMER5MIN 300000
uint8_t room = -1, control = -1, accion = -1; 
uint32_t Timer_Temperatura = TIMER5MIN; 
uint16_t Temp_Set = 16; 
uint8_t Tecla_aux, tecla_confirm, ContadoTecla = 0; 
uint8_t intensity = 0; 



int main (void)
{
    InicializarHW(); 
    while(1)
    {
        MaqEstadoPrincipal(); 
        ControlTemperatura(); 
    }
}


enum ESTADO {REPOSO, CONTROL, LUZ, CLIMA}; 
void MaqEstadoPrincipal()
{
    static uint8_t ESTADO = REPOSO; 

    switch (ESTADO)
    {
    case REPOSO:
        Tecla = tecla_confirm;
        if(Tecla == T_COM)
        {
            room = 0; 
            ESTADO = CONTROL; 
        }
        if(Tecla == T_DOM1)
        {
            room = 1; 
            ESTADO = CONTROL; 
        }
        if(Tecla == T_DOM2)
        {
            room = 2; 
            ESTADO = CONTROL; 
        }
    case CONTROL: 
        Tecla = tecla_confirm; 
        if(Tecla == T_LUZ) 
        {
            control = LUZ; 
            Display(intensity); 
            ESTADO = LUZ; 
        }
        if(Tecla == T_CLIMA)
        {
            control = CLIMA; 
            Display(Temp_Set);
            ESTADO = CLIMA; 
        }
    case LUZ: 
        Tecla = tecla_confirm; 
        if(Tecla = T_MAS)
        {
            accion = SUBIR; 
            if(intensity <= 4)
            {   
                intensity++; 
                EnviarTrama(); 
            }
        }
        if(Tecla = T_MENOS)
        {
            accion = BAJAR;
            if(intensity > 0)
            {   
                intensity--; 
                
                EnviarTrama();
            } 
        }
        ModificarLuz(room,intensity); 
        ESTADO = REPOSO; 
        break; 
    case CLIMA: 
        Tecla =  tecla_confirm;
        if(Tecla = T_MAS)
        {
            accion = SUBIR;
            if(Temp_Set <= 30)
            {
                TempSet++; 
                EnviarTrama();
            }
        }
        if(Tecla = T_MENOS)
        {
            accion = BAJAR;
            if(Temp_Set > 16)
            {
                TempSet--; 
                EnviarTrama();
            }
        }
        ModificarTemperatura(room, Temp_Set); 
        ESTADO = REPOSO; 
        break; 
    default:
        break;
    }
}

enum ESTADO_TEMP {REPOSO_TEMP, CONTROL_TEMP};

void ControlTemperatura(void)
{
    static uint8_t ESTADO_TEM = REPOSO_TEMP; 

    switch (ESTADO_TEM)
    {
    case REPOSO_TEMP:
        if(Timer_Temperatura == 0)
        {
            ESTADO_TEMP = CONTROL_TEMP; 
        }
    case CONTROL_TEMP: 
        if(Temperatura > (Temp_Set+1))
        {
            EncenderRefrigeracion(); 
            ApagarCalefaccion(); 
        }
        if(Temperatura < (Temp_Set-1))
        {
            EncenderCalefaccion(); 
            ApagarRefrigeracion();
        }      
        Timer_Temperatura = TIMER5MIN; 
        ESTADO_TEMP = REPOSO; 
        break;
    default:
        ESTADO = REPOSO; 
        break;
    }
}


void ADC_Handler(void)
{
    uint32_t data = 0; 
    if((AD0DR5 >> 31) & 0x01)
    {   
        data = (AD0DR5 >> 4) & 0xFFF; 
        Temperatura = (50/CUENTAS_ADC) * data; 
    }
}




#define DIGITOS 2 
// Supongo previa tabla con valores predeterminados para manejo de display 

static uint8_t MsgDisplay[DIGITOS]; 
void Display(uint32_t valor)
{
    static uint8_t  aux[DIGITOS], index = 0; 

    for(int i=0; i<DIGITOS; i++, valor/=10)
        MsgDisplay[i] = Tabla_Valores_7s[valor%10]; 

}

void BarridoDisplay()
{
    static uint8_t aux, digito = 0; 
    SetPin(DIGITO1, OFF); SetPin(DIGITO2, OFF); 


    aux = MsgDisplay[digito]; 
    SetPin(Segmento_a, aux & 0x01); 
    SetPin(Segmento_b, (aux >> 1) & 0x01);
    SetPin(Segmento_c, (aux >> 2) & 0x01);
    SetPin(Segmento_d, (aux >> 3) & 0x01);
    SetPin(Segmento_e, (aux >> 4) & 0x01);
    SetPin(Segmento_f, (aux >> 5) & 0x01);
    SetPin(Segmento_g, (aux >> 6) & 0x01);


    switch (digito)
    {
    case DIGITO1:
        SetPin(DIGITO1, ON); 
        break;
    case DIGITO2: 
        SetPin(DIGITO2, ON); 
    }
    digitos++; 
    digitos%=DIGITOS; 
}

uint8_t TecladoSW(void)
{
    SetPin(COL0,ON);
    SetPin(COL1,OFF);
    SetPin(COL2,OFF);
    SetPin(COL3,OFF); 
    if(GetPin(FILA0) == ON) return T_COM; 
    if(GetPin(FILA1) == ON) return T_LUZ; 

    SetPin(COL0,OFF);
    SetPin(COL1,ON);
    SetPin(COL2,OFF);
    SetPin(COL3,OFF); 
    if(GetPin(FILA0) == ON) return T_DOM1; 
    if(GetPin(FILA1) == ON) return T_CLIMA; 

    SetPin(COL0,OFF);
    SetPin(COL1,OFF);
    SetPin(COL2,ON);
    SetPin(COL3,OFF); 
    if(GetPin(FILA0) == ON) return T_DOM2; 
   
    
    SetPin(COL0,OFF);
    SetPin(COL1,OFF);
    SetPin(COL2,OFF);
    SetPin(COL3,ON); 
    if(GetPin(FILA0) == ON) return T_MAS;
    if(GetPin(FILA1) == ON) return T_MENOS;

    return NO_KEY; 
}

enum ESTADO {INICIO, LUGAR, CONTROL, ACCION, FINAL}

void EnviarTrama()
{
    static uint8_t ESTADO=INICIO; 
    uint8_t dato; 
    switch (ESTADO)
    {
    case INICIO:
        dato = '$'
        PushTX(dato); 
        ESTADO = LUGAR;
    case LUGAR: 
        dato = room; 
        PushTX(dato); 
        ESTADO = CONTROL;
    case CONTROL: 
        dato = control; 
        PushTX(dato); 
        ESTADO=ACCION; 
    case ACCION: 
        dato = accion; 
        PushTX(dato); 
        ESTADO = FINAL; 
    case FINAL: 
        dato = '#'
        PushTX(dato);
        ESTADO = INICIO;
        break; 
    
    default:
        ESTADO = INICIO; 
        break;
    }
}

void PushTx(uint8_t dato)
{
    uint8_t index = 0; 

    BUFFER_TX[indexTxIn] = dato; 
    index++; 
    index %= TOPE_BUFFER_TX; 
    if(TxStart == 0)
    {
        TxStart = 1; 
        U0THR = BUFFER_TX[indexTxOut]
    }
}

uint8_t PopTX()
{
    uint8_t aux = -1; 

    if(indexTxIn != indexTxOut)
    {
        aux = BUFFER_TX[indexTxOut];
        indexTxOut++; 
        indexTxOut%=TOPE_BUFFER_TX;  
    }

    return aux; 
}


void UART0_Handler()
{
    uint8_t iir, dato; 
    iir = U0IIR; 
    do{
        if(iir  & 0x02)
        {
            dato=PopTx(); 
            if(dato > 0)
                U0THR = dato; 
        }

    }while((iir & 0x01) == 1); 
}

void Systick_Handler(void)
{
    uint8_t aux; 
    aux = TecladoSW(); 
    if(Tecla_aux == aux && ContadorTecla == 3)
    {
        tecla_confirm = Tecla_aux; 
        ContadoTecla=0; 
    }
    ContadorTecla++;
    Timer_Temperatura--;  
}