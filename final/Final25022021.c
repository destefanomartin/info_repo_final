// Final TATETI - DISPLAY 
#define __RW volatile 

typedef unsigned int uint32_t;
typedef unsigned char uint8_t; 
typedef unsigned short uint16_t; 

// PARTE 1
// Definicion de registros 
#define GPREG0 ((__RW uint32_t *) 0x40024044)
#define CCRCLKEN ((__RW uint32_t *) 0x40024008)
#define PINSEL1 ((__RW uint32_t *) 0x4002C004)
#define PCONP ((__RW uint32_t *) 0x400FC0C4)
#define PCLKSEL0 ((__RW uint32_t *) 0x300FC1A8)
#define DIR_UART1 ((__RW uint32_t *) 0x30010000UL)
#define ISER0 ((__RW uint32_t *) 0xE00E100UL)


#define CLOCK_FREC 25000000
uint16_t TaTeTi_Number; 
void Init_UART1(uint32_t vel)
{
    // Se debe configurar PCONP 
    *PCONP |= (0x01 << 4);
    // Configurar CLKEN 
    *PCLKSEL0 &= ~(0x03 << 8);

    // Configuracion especifica de la UART 
    U1DLM = 0x00000000; 
    U1DLL = CLOCK_FREC/(16*vel) // Frecuencia variable 
    U1LCR = 0x0F; // 8 Bits, paridad impar, 2 bits stop 

    SetPinsel(0,16,1); 

    U1IER = 0x01;  // Enable interrupcion por recepcion 

    *ISER0 |= (0x01 << 6); // Enable interrupciones del LPC 

}

// PARTE 2 
#define NO_VALID_TR 0
#define INICIO_PARTIDA 1
#define AZUL_GANO 2 
#define ROJO_GANO 3 
#define TR_OK 4
#define BUFFER_SIZE 6
volatile uint8_t BUFFER_RX[BUFFER_SIZE]; 
volatile uint8_t fila, col, color; 

void UART1_Handler(void)
{
    uint8_t dato; 
    do 
    {
        if(U1IRR & 0x04)
        {
            dato = U1RBR; 
            recepcion_trama(dato); 

        }

    } while (!(U1IRR & 0x01)); 
}


void recepcion_trama(uint8_t dato)
{
    static uint8_t BUFFER[BUFFER_SIZE], index = 0, flag_start = 0;
    if(dato == 0xFA && flag_start == 0)
    {
        BUFFER[index] = dato; 
        index++; 
        flag_start = 1; 
    }

    if(flag_start == 0) return; 

    BUFFER[index] = dato; 
    index++; 

    if(index >= BUFFER_SIZE)
    {
        for(int i=0; i<BUFFER_SIZE; i++, index++)
        {
            BUFFER_RX[i]=BUFFER[i];
            flag_start=0; 
            index = 0;  
        }
    }
}

uint8_t analizar_trama()
{
    static uint8_t checksum; 
    static uint8_t fila_aux, columna_aux, color_aux; 
    if(BUFFER_RX[0] != 0xFA)
    {
        return NO_VALID_TR; 
    }

    if(BUFFER_RX[1] != ((TaTeTi_Number >> 8) & 0xFF) ) return NO_VALID_TR; 
    if(BUFFER_RX[2] != ((TaTeTi_Number) & 0xFF)) return NO_VALID_TR; 


    if(BUFFER_RX[3] != 0x02 || BUFFER_RX[3] != 0x04){
        return NO_VALID_TR; 
    }

    for(int j=0; j<BUFFER_SIZE; j++)
        checksum ^= BUFFER_RX[j];

    if(BUFFER_RX[5] != checksum) return NO_VALID_TR; 

    if(BUFFER_RX[3] == 0x84) return INICIO_PARTIDA; 
    else if(BUFFER_RX[3] == 0x81) return AZUL_GANO; 
    else if(BUFFER_RX[3] == 0x82) return ROJO_GANO; 
    else
    {   
        fila_aux = (BUFFER_RX[3] & 0x03); 
        columna_aux = ((BUFFER_RX[3] >> 2) & 0x03); 
        color_aux = (BUFFER_RX[3] >> 4) & 0x03;

        if(fila_aux < 2)
        {
            fila = fila_aux; 
        }
        else return NO_VALID_TR; 
        if(columna_aux < 2)
        {
            col = columna_aux; 
        }
        else return NO_VALID_TR; 
        if(color_aux < 2)
        {
            color = color_aux;
        }
        else return NO_VALID_TR; 
        return TR_OK; 
    }
}

// PARTE 3 

#define TENMINUTES 120000
#define FOURTYSEC 8000
#define REFRESH_TIME 5 
volatile uint32_t Timer_10m = 0; 
volatile uint16_t Timer_40s = 0, Timer_Refresh = REFRESH_TIME; 

void main()
{
    InitSystick(); 
    InitPanta();
    if(!(CCRCLKEN & 0x01))
    {
        SetTaTeTiDisplay(); 
    }

    TaTeTi_Number = ((GPREG0 >> 2) & 0xFFFF); 
    switch (GPREG0 & 0x03)
    {
    case 0: Init_UART1(9600);
    case 1: Init_UART1(2400);
    case 2: Init_UART1(1200); 
    case 3: Init_UART1(300); 
    break;
    default:
        break;
    }

    while(1){
        Maq_Estado(); 
    }

}

enum ESTADO {REPOSO, PARTIDA};

void Maq_Estado()
{
    static uint8_t ESTADO = REPOSO, match_status; 

    switch (ESTADO)
    {
    case REPOSO:
        ClearPanta(); 
        if(analizar_trama() == INICIO_PARTIDA)
        {
            Timer_10m = TENMINUTES; 
            ESTADO = PARTIDA; 
        }
    case PARTIDA: 
        match_status = analizar_trama(); 
        if(Timer_10m == 0)
        {
            ClearPanta(); 
            ESTADO = REPOSO; 
            break; 
        }
        if(match_status == TR_OK) 
        {
            SetPanta(fila,col,color);
            Timer_10m = TENMINUTES; 
        }
        if(match_status == AZUL_GANO)
        {
            Timer_40s = FOURTYSEC; 
            ESTADO = REPOSO; 
            break; 

        }
        if(match_status == ROJO_GANO)
        {
            Timer_40s = FOURTYSEC; 
            ESTADO = REPOSO; 
            break; 
        }


    
    default:
        break;
    }
}

void SysTickHandler(void)
{
    if(Timer_Refresh == 0)
    {
        RefreshPanta(); 
        Timer_Refresh = REFRESH_TIME; 
    }
    if(Timer_40s > 0)
    {
        Timer_40s--;
       
    }
    else ClearPanta(); 
    if(Timer_10m > 0)
        Timer_10m--; 

}