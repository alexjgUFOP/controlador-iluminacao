#!/bin/bash


# ================= VARS GLOBAIS  =================

# Opcoes de configuracao
PARAM=$1

# Porta serial via console - passado por parametro
SERIAL=$2

# Arquivo final - diretorio atual + nome passado por parametro
FILE=$(echo "$(pwd)/$3")

# Flags de controle
FLAG=0



## ================= DEF. DAS FUNCOES  ===========

# Funcao para ler serial
serialRead()
  {
    
    # Parametros de conf
    OP=$1
    # Porta serial
    PORT=$2
    # Arquivo destino
    ARQV=$3
    
    
    # Lendo porta serial
    OUTPUT=$(cat $PORT)
    
    # Verifica se leu a porta serial com sucesso
    if [ $? -ne 0 ]
    then
        logError
    fi
    
    # Exibindo dados da serial
    echo $OUTPUT
    
    # filtrando serial
    OUTPUT=$(serialFilter "$OUTPUT")
    
    if [ $OP = "-o" ];
    then
    
        # Gravando sobrescrevendo arquivo csv
        #echo $ARQV
        
        
        if [ $FLAG -eq 0 ]
        then
            echo $OUTPUT > $ARQV
            FLAG=1
        fi
        
        echo $OUTPUT >> $ARQV
        
        # Verifica se operacao foi realizada com sucesso
        if [ $? -ne 0 ]
        then
            logError
        fi
        
    elif [ $OP = "-e" ];
    then
    
        # Gravando no final do arquivo csv
        echo $OUTPUT >> $ARQV
        
        # Verifica se operacao foi realizada com sucesso
        if [ $? -ne 0 ]
        then
            logError
        fi
        
    else
        # Erro de parametros, encerra script
        logError
    fi

}

# Funcao para filtrar dados de entrada
serialFilter()
{

    # String a ser filtrado
    DATA=$1
            
    # Substituindo " " por ","
    DATA=$(echo "$DATA" | sed -r 's/[ ]+/,/g')
    
    # Subtituindo _ por " "
    DATA=$(echo "$DATA" | sed -r 's/[ ]+/,/g')


    #retornando valor
    echo $DATA
    
}

# Funcao para mostrar erros de entrada
logError(){

    # Apresenta erro e encerra script
    echo "Erro na execução!"
    echo "Cheque parâmetros ou porta e tente novamente"
    echo ""
    echo "serial.h -op /dev/ttyUSBX file.csv"
    echo "-o: overwrite file"
    echo "-e: end file "
    exit 1

}
   

# ================= COD. PRINCIP.  ================


while :
do
    serialRead $PARAM $SERIAL $FILE
done
