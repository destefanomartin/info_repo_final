// Final 22feb2022

typedef unsigned char uint8_t; 
typedef unsigned int uint32_t; 
#define CINCO_SEG 5000
#define ESPERA_RESP 10000
static uint16_t model; 
static uint16_t timer_5seg; 
static uint16_t Timer_10S; 
static uint8_t x;
static uint32_t Comando; 
static uint16_t Respuesta;  
static char* MODELO1[]= {"Temp", "Hum", "Tor", "Sort"};
static char* MODELO2[]= {"Rem", "Rest", "Vari", "Crest", "Sort", "Yli"};
static char* MODELO3[]= {"Temp", "Hum","Rem", "Crest", "Sort", "Yli"};
static char* MODELO4[]= {"Temp", "Hum2", "Tor", "Rem","Rest","Vari"};
static char* MODELO5[]= {"Temp", "Hum", "Crest", "Yli"};
uint8_t flag_modelo = 1; 


enum ESTADO {REPOSO, VERIFICAR_MODELO, CONSULTAR_PARAMETROS,RECIBIR_TRAMA, INFORME}

void Maq_Estado(void)
{
    static uint8_t ESTADO; 

    switch(ESTADO)
    {
    case REPOSO:
        if(get_TeclaText() == 1)
        {
            timer_5seg = CINCO_SEG; 
            ESTADO = VERIFICAR_MODELO; 
        }
        break;
    case VERIFICAR_MODELO: 
        get_trama_modelo(); 
        if(timer_5seg != 0 && flag_modelo == 0)
        {
            ESTADO = CONSULTAR_PARAMETROS; 
        }
    case CONSULTAR_PARAMETROS: 
        if(MaqEstado_Parametros() == 1);
        ESTADO = RECIBIR_TRAMA;
    case RECIBIR_TRAMA: 
        get_trama_parametros(); 
        ESTADO = INFORME; 
    case INFORME: 
        Informe(); 
        break; 
    default:
        break;
    }
}


void get_trama_parametros(uint8_t)
{
    static uint8_t ESTADO_PAR; 

    switch (ESTADO_PAR)
    {
    case /* constant-expression */:
        /* code */
        break;
    case MODELO_1: 
        {
            
        }
    default:
        break;
    }

    
}


enum ESTADO_TRMODELO {INICIO, MODELO, CHECKSUM, FINAL_TRAMAMODELO}

void get_trama_modelo()
{
    uint8_t dato, modelo[2], index, checksum = 0, acum=0; 
    dato = PopRx(); 
    static uint8_t ESTADO_TRMODELO; 
    switch (ESTADO_TRMODELO)
    {
    case INICIO:
        if(dato == '#')
        {
            index = 2; 
            ESTADO_TRMODELO = MODELO; 
        }
        break;
    case MODELO: 
        modelo[index-1] = dato - 48; 
        acum += modelo[index-1];  
        index--; 
        if(index == 0){
        if(modelo == 00)
        {
            model = modelo; 
            ESTADO_TRMODELO = CHECKSUM; 
        }
        else if(modelo == 02)
        {
            model = modelo; 
            ESTADO_TRMODELO = CHECKSUM; 
        }
        else if(modelo == 04)
        {
            model = modelo; 
            ESTADO_TRMODELO = CHECKSUM; 
        }
        else if(modelo == 18)
        {
            model = modelo; 
            ESTADO_TRMODELO = CHECKSUM; 

        }
        else if(modelo >= 20 && modelo <= 31)
        {
            model = modelo; 
            ESTADO_TRMODELO = CHECKSUM; 
        }
        }
    case CHECKSUM: 
        checksum %= acum; 
        if(dato == (checksum+48)) ESTADO_TRMODELO = FINAL_TRAMAMODELO; 
    
    case FINAL_TRAMAMODELO: 
        if(dato == '*')
        {
            flag_modelo = 0; 
            ESTADO_TRMODELO = REPOSO; 
        }


    default:
        break;
    }
}


enum ESTADOS {REPOSO, MODELO_1, MODELO_2, MODELO_3, MODELO_4, MODELO_5}; 

uint8_t MaqEstado_Parametros() 
{
    static uint8_t ESTADO; 

    switch (ESTADO)
    {
    case REPOSO: 
        if(model == 00)
        {
            ESTADO = MODELO_1; 
        }
        else if(model == 02)
        {
            ESTADO = MODELO_2; 
        }
        else if(model == 04)
        {
            ESTADO = MODELO_3; 
        }
        else if(model == 18)
        {
            ESTADO = MODELO_4; 
        }
        else
        {
            ESTADO = MODELO_5; 
        }
    case MODELO_1: 
        for(int i=0; i<(strlen(MODELO1)-1); i++)
        {
            EnviarConsulta(MODELO1[i]);
            
        }
        ESTADO = REPOSO;  
        break; 
    case MODELO_2: 
        for(int i=0; i<(strlen(MODELO2)-1); i++)
        {
            EnviarConsulta(MODELO2[i]);
        }
        ESTADO = REPOSO;  
        break; 
    case MODELO_3: 
        for(int i=0; i<(strlen(MODELO3)-1); i++)
        {
            EnviarConsulta(MODELO3[i]);
        }
        ESTADO = REPOSO;  
        break; 
    case MODELO_4: 
        for(int i=0; i<(strlen(MODELO4)-1); i++)
        {
            EnviarConsulta(MODELO4[i]);
        }
        ESTADO = REPOSO;  
        break; 
    case MODELO_5: 
        for(int i=0; i<(strlen(MODELO5)-1); i++)
        {
            EnviarConsulta(MODELO5[i]);
        }
        ESTADO = REPOSO;  
        break; 
        
    default:
        break;
    }
    return 1; 
}

enum ESTADO_CONS {INICIO, ENVIAR_PARAMETRO, FINAL_TRAMA}; 

void EnviarConsulta(char* parametro)
{
    static uint8_t ESTADO_CONS; 
    uint8_t dato; 
    switch (ESTADO_CONS)
    {
    case INICIO:
        dato = '#'
        PushTX(dato); 
        ESTADO_CONS = ENVIAR_PARAMETRO; 
        break;
    case ENVIAR_PARAMETRO: 
        for(i = 0 , i < strlen(parametro), i++)
        {   
            dato = parametro[i];
            PushtTX(dato); 
        }
        ESTADO_CONS = FINAL_TRAMA; 
    case FINAL_TRAMA: 
        dato = '*'; 
        PushTX(dato); 
        Timer_10S = ESPERA_RESP; 
    default:
        break;
    }
}