#!/bin/bash

echo "Iniciando bateria de simulações do Projeto Redes..."
echo "Usando workaround de 'hardcoding' para evitar bug do ns-3.43 c/ GCC 13"

# Caminho para o arquivo C++
CC_FILE="scratch/equipe_6_2s2025.cc"
NS3_RUN_CMD="./ns3 run $CC_FILE"

# --- Verificação ---
if [ ! -f "$CC_FILE" ]; then
    echo "Erro: Arquivo '$CC_FILE' não encontrado."
    echo "Salve o código C++ ajustado nesse local."
    exit 1
fi

# Requisito 6: Lista de clientes
CLIENTES=(1 2 4 8 16 32)
# Requisitos 3, 4, 5: Lista de protocolos
PROTOCOLOS=("TCP" "UDP" "MIXED")
# Requisitos 1, 2: Lista de mobilidade
MOBILIDADE=("false" "true")

TOTAL_RUNS=$(( ${#CLIENTES[@]} * ${#PROTOCOLOS[@]} * ${#MOBILIDADE[@]} ))
COUNT=1

# Limpar o build *antes* de começar
echo "Limpando cache de build (ns3 clean)..."
./ns3 clean

# ### ADICIONAR ESTA LINHA ###
# Configurar o build uma vez
echo "Configurando o ambiente de build (ns3 configure)..."
./ns3 configure
# ### FIM DA ADIÇÃO ###


# Loop principal
for n in "${CLIENTES[@]}"; do
    for p in "${PROTOCOLOS[@]}"; do
        for m in "${MOBILIDADE[@]}"; do
        
            MOBIL_STR="estatico"
            if [ "$m" = "true" ]; then
                MOBIL_STR="movel"
            fi
            
            echo ""
            echo "=========================================================="
            echo "Preparando Simulação $COUNT / $TOTAL_RUNS"
            echo "Clientes: $n, Protocolo: $p, Mobilidade: $MOBIL_STR"
            echo "=========================================================="
            
            # Etapa 1: Modificar o arquivo C++
            echo "Configurando $CC_FILE..."
            sed -i "s/nClients = COMO_CLIENTES/nClients = $n/g" $CC_FILE
            sed -i "s/protocol = \"COMO_PROTOCOLO\"/protocol = \"$p\"/g" $CC_FILE
            sed -i "s/mobility = COMO_MOBILIDADE/mobility = $m/g" $CC_FILE

            # Etapa 2: Compilar e Rodar
            echo "Executando (ns3 run)..."
            $NS3_RUN_CMD
            
            # Etapa 3: Resetar o arquivo C++
            echo "Resetando $CC_FILE..."
            sed -i "s/nClients = $n/nClients = COMO_CLIENTES/g" $CC_FILE
            sed -i "s/protocol = \"$p\"/protocol = \"COMO_PROTOCOLO\"/g" $CC_FILE
            sed -i "s/mobility = $m/mobility = COMO_MOBILIDADE/g" $CC_FILE

            COUNT=$((COUNT + 1))
        done
    done
done

echo ""
echo "Bateria de simulações concluída! 36 arquivos 'resultados_*.txt' gerados."
