
volatile uint8_t dato; 
void main()
{
    Inicializacion(); // No es necesario implementar 

    while(1)
    {
        Maq_Estado_Principal(); 
    }
}


void Maq_Estado_Principal()
{
    uint8_t ESTADO = REPOSO; 

    switch (ESTADO)
    {
    case REPOSO:
        if(peso_and_tol > 0)
        {
            ESTADO = ESPERANDO_INICIO; 
        }
        break;
    case ESPERANDO_INICIO: 
        if(STOP == 1) ESTADO = REPOSO; 
        if(ARRANCAR == 1 && calcular_peso() == 0)
        {
            ESTADO = FUNCIONANDO; 
        }
    case FUNCIONANDO: 
        if(STOP == 1) ESTADO = REPOSO; 
        if(DETENER == 1)
        {
            ESTADO = SENSAR_PESO; 
        }
    case SENSAR_PESO: 
        if(STOP == 1) ESTADO = REPOSO; 
        if(calcular_peso() >= peso_mas_tol || calcular_peso() <= peso_men_tol)
        {
            Encender_LuzRoja(); 
        }
        else
        {
            Contador_cajas++; 
            Encender_LuzVerde(); 
            Display(Contador_Cajas);
        }
        
    
    default:
        break;
    }
}


uint8_t ADC_Handler(void)
{
    if(AD0DR5 >> 31 == 0x01) 
        dato = (AD0DR5 >> 4) & OxFF; 
    return dato;

}

uint8_t calcular_peso(void)
{
    uint8_t peso_final; 
    peso_final = dato * 5000/4095;  
    return peso_final; 
}

void RecibirTrama()
{
    uint8_t data, tolerance, peso_dec, peso_ent, ESTADO_COMSERIE; 
    data = PopRx(); 
    switch (ESTADO_COMSERIE) 
    {
    case INICIO_TRAMA:
        if(data == '$')
        {
            ESTADO = PESO_ENT; 
        }
        break;
    case PESO_ENT: 
        if(data != -1)
        {
            peso_pent = (int)(data - 48); 
            ESTADO = PESO_DEC; 
        }
        else ESTADO = INICIO_TRAMA; 
        break;
    case PESO_DEC: 
        if(data != -1)
        {
            peso_dec = (int)(data - 48); 
            ESTADO = TOLERANCIA; 
        }
        else ESTADO = INICIO_TRAMA; 
        break;
    case TOLERANCIA: 
        if(data != -1)
        {
            tolerance = ((int)(data - 48))/100;
            ESTADO = FINAL_TRAMA; 

        }
        else ESTADO = INICIO_TRAMA; 
        break;
    case FINAL_TRAMA: 
        if(data == '#')
        {
            peso_mas_tol = peso_pent + peso_dec/100 + tolerance; 
            peso_men_tol = peso_pent + peso_dec/100 - tolerance; 
            ESTADO = INICIO_TRAMA; 

        }
        else ESTADO = INICIO_TRAMA; 
        break;
    default:
        break;
    }
}