# Projeto Prático - Redes de Computadores 2S2025

## 📋 Descrição do Projeto

Este projeto implementa uma simulação de rede híbrida (cabeada + wireless) utilizando o Network Simulator 3 (NS-3) versão 3.43. O objetivo é avaliar o desempenho de diferentes protocolos de transporte (TCP, UDP e Misto) em cenários com e sem mobilidade dos nós wireless.

### Topologia Implementada

```
s2 (Servidor) ---- s1 (Roteador) ---- s0 (Roteador) ---- AP (Access Point)
      |                |                    |              |
  10.1.1.0         10.1.2.0            10.1.3.0      192.168.0.0
                                                          |
                                                   ****************
                                                   *  *  *  *  *  *
                                                   c0 c1 c2 c3...cn
                                                   (Clientes Wi-Fi)
```

**Características:**
- **Rede Cabeada:** Conexões Point-to-Point entre s2-s1-s0-AP
- **Rede Wireless:** Clientes Wi-Fi conectados ao AP via IEEE 802.11a
- **SSID:** Equipe6
- **Cenário:** Download (Servidor → Clientes)

---

## ⚙️ Parâmetros de Simulação

### Parâmetros Gerais

| Parâmetro | Valor |
|-----------|-------|
| **Tempo de simulação** | 60 segundos |
| **Número de clientes** | 1, 2, 4, 8, 16 e 32 |
| **Tamanho do cenário** | 140m × 140m |
| **SSID do Access Point** | Equipe6 |

### Parâmetros da Rede Cabeada

| Parâmetro | Valor |
|-----------|-------|
| **Taxa de transmissão** | 100 Mbps |
| **Atraso de propagação** | 2 ms |
| **Tecnologia** | Point-to-Point |

### Parâmetros da Rede Wireless (IEEE 802.11a)

| Parâmetro | Valor |
|-----------|-------|
| **Padrão de comunicação** | IEEE 802.11a |
| **Taxa de dados** | 6 Mbps (OfdmRate6Mbps) |
| **Potência de transmissão** | 16 dBm |
| **Número de antenas no AP** | 1 |
| **Frequência** | 5 GHz |

### Parâmetros de Mobilidade

| Cenário | Modelo | Velocidade |
|---------|--------|------------|
| **Estático** | ConstantPositionMobilityModel | 0 km/h (estacionário) |
| **Móvel** | RandomWalk2dMobilityModel | 3.6 a 7.2 km/h (1.0 a 2.0 m/s) |

**Área de mobilidade:** Retângulo de 140m × 140m  
**Distância de movimento:** 10 metros por etapa

### Parâmetros das Aplicações

#### TCP (BulkSend)
| Parâmetro | Valor |
|-----------|-------|
| **Tipo de socket** | TcpSocketFactory |
| **Tamanho do pacote** | 1500 bytes |
| **Máximo de bytes** | Ilimitado (MaxBytes = 0) |
| **Porta** | 9 |
| **Tipo de aplicação** | Download contínuo (bulk transfer) |

#### UDP (OnOff)
| Parâmetro | Valor |
|-----------|-------|
| **Tipo de socket** | UdpSocketFactory |
| **Taxa de dados** | 512 kbps |
| **Tamanho do pacote** | 512 bytes |
| **Porta** | 10 |
| **Padrão de tráfego** | CBR (Constant Bit Rate) |

#### Cenário Misto (MIXED)
- **50% dos clientes:** Aplicação TCP
- **50% dos clientes:** Aplicação UDP

### Posicionamento dos Nós

| Nó | Tipo | Posição (x, y, z) |
|----|------|-------------------|
| **s2** | Servidor | (0, 70, 0) |
| **s1** | Roteador | (20, 70, 0) |
| **s0** | Roteador | (40, 70, 0) |
| **AP** | Access Point | (70, 70, 0) |
| **Clientes** | Estações Wi-Fi | Aleatório em [30-110, 30-110, 0] |

---

## 📊 Cenários Avaliados

O projeto avalia **36 cenários distintos**, resultantes da combinação:

- **Número de clientes:** 6 valores (1, 2, 4, 8, 16, 32)
- **Protocolos:** 3 tipos (TCP, UDP, MIXED)
- **Mobilidade:** 2 condições (Estático, Móvel)

**Total:** 6 × 3 × 2 = **36 experimentos**

---

## 📈 Métricas Coletadas

Para cada cenário, são coletadas as seguintes métricas através do FlowMonitor:

1. **Vazão (Throughput):** Taxa efetiva de transmissão de dados (Mbps)
2. **Atraso Médio:** Latência média de ponta a ponta (ms)
3. **Taxa de Perda de Pacotes:** Percentual de pacotes perdidos (%)
4. **Pacotes Transmitidos:** Contador total de pacotes enviados
5. **Pacotes Recebidos:** Contador total de pacotes entregues
6. **Pacotes Perdidos:** Contador de pacotes não entregues

---

## 🗂️ Estrutura de Arquivos

```
projeto-redes-2s2025/
├── scratch/
    └──  equipe_6_2s2025.cc           # Código-fonte da simulação NS-3
├── run.sh                        # Script de automação (executa 36 cenários)
├── process_results.py            # Script Python para processar resultados
├── README.md                     # Este arquivo
├── COMO_EXECUTAR.txt            # Guia rápido de execução
└── resultados_projeto/           # Diretório de saída (gerado após execução)
    ├── resultados_TCP_1clientes_estatico.txt
    ├── resultados_TCP_2clientes_estatico.txt
    ├── ... (36 arquivos no total)
    ├── resultados_consolidados.csv
    ├── estatisticas_detalhadas.csv
    └── comparacao_completa.png
```

---

## 🚀 Como Executar

### Pré-requisitos

**Software Necessário:**
- NS-3 versão 3.43 instalado
- Python 3.x
- Bibliotecas Python: pandas, matplotlib, numpy

**Instalação das bibliotecas Python:**
```bash
pip3 install pandas matplotlib numpy
```

### Passo 1: Preparar o Ambiente

```bash
# Navegar até o diretório do NS-3
cd ~/ns-allinone-3.43/ns-3.43/

# Copiar o código para a pasta scratch
cp /caminho/para/equipe_6_2s2025.cc scratch/

# Copiar o script de execução
cp /caminho/para/run.sh .

# Dar permissão de execução
chmod +x run.sh
```

### Passo 2: Executar as Simulações

```bash
./run.sh
```

**Tempo estimado:** -> (depende do hardware)


Especificações da máquina utilizada para processamento:
Processador	AMD Ryzen 7 3700U with Radeon Vega Mobile Gfx     2.30 GHz
RAM instalada	8,00 GB (utilizável: 5,88 GB)
Tipo de sistema	Sistema operacional de 64 bits, processador baseado em x64
Windows 11
-> Projeto realizado pelo Ubuntu WSL


**O que acontece:**
- Executa automaticamente os 36 cenários
- Gera 36 arquivos `resultados_*.txt` no diretório atual
- Exibe progresso no terminal

### Passo 3: Criar Diretório de Resultados

```bash
mkdir -p resultados_projeto
mv resultados_*.txt resultados_projeto/
```

### Passo 4: Processar Resultados e Gerar Gráficos

```bash
# Copiar script Python
cp /caminho/para/process_results.py .

# Executar processamento
python3 process_results.py
```

**Saída gerada:**
- `resultados_projeto/comparacao_completa.png` - 4 gráficos principais
- `resultados_projeto/resultados_consolidados.csv` - Dados agregados
- `resultados_projeto/estatisticas_detalhadas.csv` - Estatísticas por protocolo
- Relatório estatístico impresso no terminal

---

## 📊 Resultados Esperados

### Gráficos Gerados

1. **Vazão vs Número de Clientes:** Mostra como a vazão varia com o aumento de clientes para cada protocolo
2. **Atraso vs Número de Clientes:** Demonstra o impacto da carga na latência
3. **Perda de Pacotes vs Número de Clientes:** Evidencia a degradação com congestionamento
4. **Comparação Estático vs Móvel:** Barras comparando desempenho médio por protocolo

### Comportamentos Observados

- **TCP:** Alta vazão (~3.5-4.0 Mbps), atraso crescente, baixa perda
- **UDP:** Vazão decrescente (~1.5 → 0.5 Mbps), baixo atraso, alta perda (>70%)
- **MIXED:** Comportamento intermediário
- **Mobilidade:** Reduz vazão em ~30-35% comparado ao cenário estático

---

## 🛠️ Detalhes de Implementação

### Automação via Script Bash

O script `run.sh` utiliza substituição de texto (`sed`) para:
1. Modificar placeholders no código-fonte (`COMO_CLIENTES`, `COMO_PROTOCOLO`, `COMO_MOBILIDADE`)
2. Compilar e executar com valores específicos
3. Restaurar código original para próxima iteração

### Coleta de Métricas

Utiliza **FlowMonitor** do NS-3 para coletar estatísticas de todos os fluxos de rede, filtrando apenas tráfego de download (Servidor → Clientes).

### Processamento de Dados

Script Python:
- Lê 36 arquivos TXT individuais
- Consolida em formato CSV
- Calcula estatísticas descritivas
- Gera visualizações com Matplotlib

---

## 📚 Referências

- [NS-3 Documentation](https://www.nsnam.org/documentation/)
- [NS-3 Tutorial](https://www.nsnam.org/docs/tutorial/html/)
  
