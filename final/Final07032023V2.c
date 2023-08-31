#define MY_BUFF
#define BITS_TRAMA 13
static uint8_t MyBuffer[MY_BUFF]; 
static uint16_t tr_cont = 0; 
static uint8_t out_cont  = 0; 
static uint8_t in_cont = =0; 

uint8_t MyTx (uint8_t data)
{
    if(tr_cont >= MY_BUFF)
    {
        return MY_ERR; 
    }

    MyBuffer[tr_cont] = data;
    tr_cont++;
    tr_cont%=MY_BUFF; 

    return MY_OK; 

}

uint32_t armar_trama()
{
    uint32_t data, aux; // 
    uint16_t par; // sera el bit que asignaremos par acomodar el dato de la paridad 
    uint16_t acum = 0; 
    data = aux = MyBuffer[out_cont]; 
    out_cont++; 
    out_cont%=MY_BUFF; 
    tr_cont--; 

    do 
    {   
        if(aux & 0x01)
        {
            acum++; 
        }
        aux >>= 0x01; 
    }while(aux != -1); 

    if(acum % 2 == 0)
    {
        par = 0x0000;
    }
    else
    {
        par = 0x0100;
    }


    // Ahora en data vos tenes la informacion sin los bits de inicio, ni paridad, ni nada 
    data |= par; 
    data |= 0x600; // bits de stop 
    data <<= 0x02; // bits de start 

    return data; 
}



void MyTick()
{
    static uint32_t data; 
    static uint8_t ESTADO; 
    static uint32_t bit_count = 0; 
    if(flag_start == 0)
    {
        SetPin(PP_TX,QQ_TX, HIGH_STATE); 
            if(tr_cont == 0)
            return; 
        flag_start = 1; 
        ESTADO = TR_START; 
        c_ticks = 0; 

    switch (ESTADO)
    {
    case TR_START:
        data = armar_trama(); 
        cont_data = 0 ; 
        ESTADO = ENVIANDO; 
    case ENVIANDO: 
        if(data & 0x01)
        {
            SetPin(PP_TX, QQ_TX, HIGH_STATE); 
        }
        else SetPin(PP_TX, QQ_TX, LOW_STATE); 

        data >>= 0x01;
        cont_data++; 
        if(cont_data >= BITS_TRAMA)
        {
            flag_start = 0; 
            ESTADO = TR_START; 
        } 

    
    default:
        bit_count = 0; 
        SetPin(PP_TX,QQ_TX, HIGH_STATE);
        flag_start = 0; 
        ESTADO = TR_START; 
        break;
    }

    

}
