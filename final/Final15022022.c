#define TRUE 1 
#define FALSE 0 
#define ON 1 
#define OFF 0 
#define PORT_PS 0 // VALORES ARBITRARIOS 
#define PIN_PS 2 // VALORES ARBITRARIOS 
#define PORT_MOTOR 1 // VALORES ARBITRARIOS 
#define PIN_MOTOR 15 // VALORES ARBITRARIOS 
#define MODO_SHABAT 1 
#define MODO_NORMAL 0 


bool Sensor_VerificarSiEstoyEnElPisoSuperior(void)
{
    if(GetPin(PUERTO_PS,PIN_PS) == 1)
        return TRUE; 
    else 
        return FALSE; 
}

void Ascensor_ActivarMovimiento(void)
{
    SetPin(PORT_MOTOR,PIN_MOTOR,ON); 
}

void main()
{
    Inicializacion(); 

    while(1)
    {
        void ControlDeAscensor_Ejecutar(void)
    }
}


void ControlDeAscensor_Ejecutar()
{
    uint8_t LLAVE_MODO; 
    LLAVE_MODO = DeteccionModo(); 
    switch (LLAVE_MODO)
    {
    case MODO_SHABAT:
        Botonera_Deshabilitada(); 
        EjecutarModoShabat(); 
        break;
    case MODO_NORMAL: 
        EjecutarModoNormal(); 
    default:
        break;
    }
}

void EjecutarModoShabat()
{
    uint8_t ESTADO = REPOSO; 

    switch (ESTADO)
    {
    case REPOSO:
        if(Ascensor_PisoInferior() == 1)
        {
            BarreraPuerta_Deshabilitada();
            Timer_EntSal();  
            TimerDetencion_ON();
            ESTADO = ASCENSO; 
        } 
        break;
    
    case ASCENSO: 
        IndicadorPosicion_Habilitado(); 
        Ascensor_DireccionAscendente(); 
        if(FlagEntSal == 0) 
        {   
            BarreraPuerta_Habilitada(); 
            TimerBuzzer_ON(); 
            if(FlagStop == 0)
            {
                Ascensor_MovimientoHabilitado(); 
            }
        }
        if(Ascensor_PisoIntermedio() == 1) 
        {
            Ascensor_MovimientoDeshabilitado(); 
            BarreraPuerta_Deshabilitada(); 
            Timer_EntSal();
            TimerDetencion_ON(); 
        }

        if(Ascensor_PisoSuperior() == 1)
        {
            BarreraPuerta_Deshabilitada();
            Timer_EntSal();  
            TimerDetencion_ON();
            ESTADO = DESCENSO; 
        }
        break; 
    case DESCENSO: 
        
        IndicadorPosicion_Deshabilitado();
        Ascensor_DireccionDescendente(); 
        if(FlagEntSal == 0) 
        {   
            BarreraPuerta_Habilitada(); 
            TimerBuzzer_ON(); 
            if(FlagStop == 0)
            {
                Ascensor_MovimientoHabilitado(); 
            }
        }
        
        if(Ascensor_PisoIntermedio() == 1) 
        {
            Ascensor_MovimientoDeshabilitado(); 
            BarreraPuerta_Deshabilitada(); 
            Timer_EntSal();
            TimerDetencion_ON(); 
        }

        if(Ascensor_PisoInferior() == 1)
        {
            BarreraPuerta_Deshabilitada();
            Timer_EntSal();  
            TimerDetencion_ON();
            ESTADO = ASCENSO; 
        } 

        break; 
    default:
        ESTADO = REPOSO; 
        break;
    }
}