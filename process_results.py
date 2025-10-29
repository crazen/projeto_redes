#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import os
import glob
import csv
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

def safe_float_convert(value, unit=''):
    """Converte string para float removendo unidades"""
    try:
        if value is None:
            return 0.0
        cleaned = str(value).replace(unit, '').strip()
        return float(cleaned)
    except (ValueError, AttributeError):
        return 0.0

def process_results():
    results_dir = "resultados_projeto"
    
    if not os.path.exists(results_dir):
        print(f"Erro: Diretorio {results_dir} nao encontrado!")
        return []
    
    print(f"\nProcessando arquivos em: {results_dir}/")
    print("-" * 60)
    
    data = []
    files = sorted(glob.glob(f"{results_dir}/resultados_*.txt"))
    
    if not files:
        print("ERRO: Nenhum arquivo de resultado encontrado!")
        return []
    
    print(f"Encontrados {len(files)} arquivos para processar\n")
    
    for filename in files:
        try:
            result = {}
            with open(filename, 'r', encoding='utf-8', errors='ignore') as f:
                for line in f:
                    if ',' in line:
                        parts = line.strip().split(',', 1)
                        if len(parts) == 2:
                            key, value = parts
                            result[key] = value
            
            if result:
                basename = os.path.basename(filename)
                
                # Extrair dados
                clientes = int(result.get('Clientes', 0))
                protocolo = result.get('Protocolo', 'TCP')
                mobilidade = result.get('Mobilidade', 'Nao')
                
                data.append({
                    'Clientes': clientes,
                    'Protocolo': protocolo,
                    'Mobilidade': mobilidade,
                    'TaxaPerda': safe_float_convert(result.get('TaxaPerda', '0')),
                    'AtrasoMedio': safe_float_convert(result.get('AtrasoMedio_ms', '0')),
                    'Vazao': safe_float_convert(result.get('Vazao_Mbps', '0')),
                    'PacotesTransmitidos': int(result.get('PacotesTransmitidos', 0)),
                    'PacotesRecebidos': int(result.get('PacotesRecebidos', 0)),
                    'PacotesPerdidos': int(result.get('PacotesPerdidos', 0))
                })
                print(f"  OK: {basename}")
                
        except Exception as e:
            print(f"  ERRO: {os.path.basename(filename)} - {e}")
            continue
    
    if not data:
        print("\nERRO: Nenhum dado valido processado!")
        return []
    
    print(f"\nTotal processado: {len(data)} experimentos")
    
    # Salvar CSV
    csv_file = f"{results_dir}/resultados_consolidados.csv"
    with open(csv_file, 'w', newline='', encoding='utf-8') as f:
        writer = csv.DictWriter(f, fieldnames=['Clientes', 'Protocolo', 'Mobilidade', 
                                              'TaxaPerda', 'AtrasoMedio', 'Vazao',
                                              'PacotesTransmitidos', 'PacotesRecebidos', 
                                              'PacotesPerdidos'])
        writer.writeheader()
        writer.writerows(data)
    
    print(f"CSV salvo: {csv_file}")
    
    # Gerar gráficos
    generate_plots(data, results_dir)
    
    return data

def generate_plots(data, output_dir):
    """Gera gráficos principais"""
    
    df = pd.DataFrame(data)
    
    # Configurar estilo
    plt.style.use('seaborn-v0_8-darkgrid')
    fig, axes = plt.subplots(2, 2, figsize=(16, 12))
    fig.suptitle('ANALISE DE DESEMPENHO - PROJETO REDES 2S2025\nUNIFESP/ICT', 
                 fontsize=16, fontweight='bold')
    
    colors = {'TCP': '#1f77b4', 'UDP': '#ff7f0e', 'MIXED': '#2ca02c'}
    markers = {'Nao': 'o', 'Sim': 's'}
    linestyles = {'Nao': '-', 'Sim': '--'}
    
    # Gráfico 1: Vazão
    ax1 = axes[0, 0]
    for prot in df['Protocolo'].unique():
        for mob in df['Mobilidade'].unique():
            subset = df[(df['Protocolo'] == prot) & (df['Mobilidade'] == mob)]
            if not subset.empty:
                label = f"{prot} {'Movel' if mob == 'Sim' else 'Estatico'}"
                subset = subset.sort_values('Clientes')
                ax1.plot(subset['Clientes'], subset['Vazao'],
                        marker=markers[mob], linestyle=linestyles[mob],
                        color=colors.get(prot, '#000'), label=label, 
                        linewidth=2.5, markersize=8)
    
    ax1.set_xlabel('Numero de Clientes', fontsize=12, fontweight='bold')
    ax1.set_ylabel('Vazao (Mbps)', fontsize=12, fontweight='bold')
    ax1.set_title('VAZAO vs CLIENTES', fontsize=13, fontweight='bold')
    ax1.legend(loc='best', framealpha=0.9)
    ax1.grid(True, alpha=0.3)
    ax1.set_xticks([1, 2, 4, 8, 16, 32])
    
    # Gráfico 2: Atraso
    ax2 = axes[0, 1]
    for prot in df['Protocolo'].unique():
        for mob in df['Mobilidade'].unique():
            subset = df[(df['Protocolo'] == prot) & (df['Mobilidade'] == mob)]
            if not subset.empty:
                label = f"{prot} {'Movel' if mob == 'Sim' else 'Estatico'}"
                subset = subset.sort_values('Clientes')
                ax2.plot(subset['Clientes'], subset['AtrasoMedio'],
                        marker=markers[mob], linestyle=linestyles[mob],
                        color=colors.get(prot, '#000'), label=label,
                        linewidth=2.5, markersize=8)
    
    ax2.set_xlabel('Numero de Clientes', fontsize=12, fontweight='bold')
    ax2.set_ylabel('Atraso Medio (ms)', fontsize=12, fontweight='bold')
    ax2.set_title('ATRASO vs CLIENTES', fontsize=13, fontweight='bold')
    ax2.legend(loc='best', framealpha=0.9)
    ax2.grid(True, alpha=0.3)
    ax2.set_xticks([1, 2, 4, 8, 16, 32])
    
    # Gráfico 3: Taxa de Perda
    ax3 = axes[1, 0]
    for prot in df['Protocolo'].unique():
        for mob in df['Mobilidade'].unique():
            subset = df[(df['Protocolo'] == prot) & (df['Mobilidade'] == mob)]
            if not subset.empty:
                label = f"{prot} {'Movel' if mob == 'Sim' else 'Estatico'}"
                subset = subset.sort_values('Clientes')
                ax3.plot(subset['Clientes'], subset['TaxaPerda'],
                        marker=markers[mob], linestyle=linestyles[mob],
                        color=colors.get(prot, '#000'), label=label,
                        linewidth=2.5, markersize=8)
    
    ax3.set_xlabel('Numero de Clientes', fontsize=12, fontweight='bold')
    ax3.set_ylabel('Taxa de Perda (%)', fontsize=12, fontweight='bold')
    ax3.set_title('PERDA DE PACOTES vs CLIENTES', fontsize=13, fontweight='bold')
    ax3.legend(loc='best', framealpha=0.9)
    ax3.grid(True, alpha=0.3)
    ax3.set_xticks([1, 2, 4, 8, 16, 32])
    
    # Gráfico 4: Comparação de Protocolos
    ax4 = axes[1, 1]
    protocolos = df['Protocolo'].unique()
    x = np.arange(len(protocolos))
    width = 0.35
    
    medias_estatico = [df[(df['Protocolo'] == p) & (df['Mobilidade'] == 'Nao')]['Vazao'].mean() 
                       for p in protocolos]
    medias_movel = [df[(df['Protocolo'] == p) & (df['Mobilidade'] == 'Sim')]['Vazao'].mean() 
                    for p in protocolos]
    
    bars1 = ax4.bar(x - width/2, medias_estatico, width, label='Estatico', 
                    alpha=0.8, color='steelblue', edgecolor='black', linewidth=1.5)
    bars2 = ax4.bar(x + width/2, medias_movel, width, label='Movel',
                    alpha=0.8, color='coral', edgecolor='black', linewidth=1.5)
    
    for bar in bars1:
        height = bar.get_height()
        ax4.text(bar.get_x() + bar.get_width()/2., height,
                f'{height:.1f}', ha='center', va='bottom', fontweight='bold')
    
    for bar in bars2:
        height = bar.get_height()
        ax4.text(bar.get_x() + bar.get_width()/2., height,
                f'{height:.1f}', ha='center', va='bottom', fontweight='bold')
    
    ax4.set_xlabel('Protocolo', fontsize=12, fontweight='bold')
    ax4.set_ylabel('Vazao Media (Mbps)', fontsize=12, fontweight='bold')
    ax4.set_title('COMPARACAO: ESTATICO vs MOVEL', fontsize=13, fontweight='bold')
    ax4.set_xticks(x)
    ax4.set_xticklabels(protocolos)
    ax4.legend(framealpha=0.9)
    ax4.grid(True, alpha=0.3, axis='y')
    
    plt.tight_layout()
    output_file = f'{output_dir}/comparacao_completa.png'
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    plt.close()
    
    print(f"Grafico salvo: {output_file}")
    
    # Estatísticas
    print_statistics(df, output_dir)

def print_statistics(df, output_dir):
    """Imprime estatísticas resumidas"""
    
    print("\n" + "="*60)
    print("RELATORIO ESTATISTICO")
    print("="*60)
    
    for prot in df['Protocolo'].unique():
        print(f"\n{prot}:")
        for mob in ['Nao', 'Sim']:
            subset = df[(df['Protocolo'] == prot) & (df['Mobilidade'] == mob)]
            if not subset.empty:
                mob_txt = 'Movel' if mob == 'Sim' else 'Estatico'
                print(f"  {mob_txt}:")
                print(f"    Vazao media: {subset['Vazao'].mean():.2f} Mbps")
                print(f"    Atraso medio: {subset['AtrasoMedio'].mean():.2f} ms")
                print(f"    Perda media: {subset['TaxaPerda'].mean():.2f} %")
    
    # Salvar CSV de estatísticas
    stats = df.groupby(['Protocolo', 'Mobilidade']).agg({
        'Vazao': ['mean', 'std', 'min', 'max'],
        'AtrasoMedio': ['mean', 'std', 'min', 'max'],
        'TaxaPerda': ['mean', 'std', 'min', 'max']
    }).round(3)
    
    stats_file = f'{output_dir}/estatisticas_detalhadas.csv'
    stats.to_csv(stats_file)
    print(f"\nEstatisticas salvas: {stats_file}")

if __name__ == "__main__":
    print("\n" + "="*60)
    print("  PROCESSAMENTO DE RESULTADOS")
    print("  Redes de Computadores - UNIFESP/ICT 2S2025")
    print("="*60)
    
    try:
        data = process_results()
        
        if data:
            print("\n" + "="*60)
            print("  PROCESSAMENTO CONCLUIDO COM SUCESSO!")
            print("="*60)
        else:
            print("\nERRO: Nenhum dado processado.")
            
    except Exception as e:
        print(f"\nERRO: {e}")
        import traceback
        traceback.print_exc()
