/*
 * Projeto Prático - Redes de Computadores 2S2025
 * UNIFESP/ICT - Prof. Dr. Joahannes Bruno Dias da Costa
 *
 * CÓDIGO FINAL EQUIPE 6 2s2025
 * Jorge Moreira Dourado Neto - 168900
 * Anna Clara Medina Roissmann - 
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include <fstream>
#include <iomanip>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("ProjetoRedesUNIFESP");

int main(int argc, char* argv[])
{
    // ===== PARÂMETROS DE SIMULAÇÃO (Valores Padrão) =====
    uint32_t nClients = COMO_CLIENTES;      // Default: 1 cliente
    std::string protocol = "COMO_PROTOCOLO"; // Default: TCP
    bool mobility = COMO_MOBILIDADE;        // Default: Estático
    double simTime = 60.0;        // Default: 60s
    
    // ===== PARSER DA LINHA DE COMANDO =====
    // Lê os argumentos passados (ex: --nClients=4)
    CommandLine cmd(__FILE__);
    cmd.AddValue("nClients", "Número de clientes Wi-Fi", nClients);
    cmd.AddValue("protocol", "Protocolo (TCP, UDP, Misto ou MIXED)", protocol);
    cmd.AddValue("mobility", "Ativar mobilidade (true/false)", mobility);
    cmd.AddValue("simTime", "Tempo de simulação em segundos", simTime);
    cmd.Parse(argc, argv);

    // ===== PARÂMETROS FIXOS (Tabela 1) =====
    double scenarioSize = 140.0;        // 140m x 140m
    std::string equipe = "Equipe6";     // SSID
    
    Time::SetResolution(Time::NS);
    
    std::cout << "\n==========================================\n";
    std::cout << "  PROJETO REDES 2S2025 - UNIFESP/ICT\n";
    std::cout << "==========================================\n";
    std::cout << "Config: " << nClients << " clientes, " << protocol 
              << ", " << (mobility ? "móvel" : "estático") << "\n";
    std::cout << "Tempo: " << simTime << "s\n";
    std::cout << "CENÁRIO: DOWNLOAD (Servidor -> Clientes)\n";
    std::cout << "==========================================\n\n";

    // ===== CRIAÇÃO DOS NÓS (Figura 1) =====
    NodeContainer s2; s2.Create(1);  // Servidor
    NodeContainer s1; s1.Create(1);  // Roteador
    NodeContainer s0; s0.Create(1);  // Roteador
    NodeContainer ap; ap.Create(1);  // Access Point
    NodeContainer sta; sta.Create(nClients); // Clientes

    // ===== REDE CABEADA (Figura 1) =====
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));
    NetDeviceContainer d_s2s1 = p2p.Install(s2.Get(0), s1.Get(0));
    NetDeviceContainer d_s1s0 = p2p.Install(s1.Get(0), s0.Get(0));
    NetDeviceContainer d_s0ap = p2p.Install(s0.Get(0), ap.Get(0));

    // ===== REDE SEM FIO (Tabela 1) =====
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());
    phy.Set("TxPowerStart", DoubleValue(16.0));
    phy.Set("TxPowerEnd", DoubleValue(16.0));
    
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211a);
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                 "DataMode", StringValue("OfdmRate6Mbps"),
                                 "ControlMode", StringValue("OfdmRate6Mbps"));
    
    WifiMacHelper mac;
    Ssid ssid = Ssid(equipe);
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid));
    NetDeviceContainer staDevs = wifi.Install(phy, mac, sta);
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    NetDeviceContainer apDev = wifi.Install(phy, mac, ap);

    // ===== MOBILIDADE (Tabela 1) =====
    MobilityHelper mob;
    mob.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    Ptr<ListPositionAllocator> fixedPos = CreateObject<ListPositionAllocator>();
    fixedPos->Add(Vector(0, 70, 0));    // s2 (servidor)
    fixedPos->Add(Vector(20, 70, 0));   // s1
    fixedPos->Add(Vector(40, 70, 0));   // s0
    fixedPos->Add(Vector(70, 70, 0));   // AP (centro do cenário 140x140)
    mob.SetPositionAllocator(fixedPos);
    mob.Install(s2);
    mob.Install(s1);
    mob.Install(s0);
    mob.Install(ap);
    
    MobilityHelper mobSta;
    // Posição aleatória dentro do alcance [30|110] x [30|110]
    mobSta.SetPositionAllocator("ns3::RandomRectanglePositionAllocator",
                                  "X", StringValue("ns3::UniformRandomVariable[Min=30|Max=110]"), // CORRIGIDO
                                  "Y", StringValue("ns3::UniformRandomVariable[Min=30|Max=110]")); // CORRIGIDO

    if (mobility)
    {
        // Tabela 1: Móvel (3.6-7.2 km/h) = (1.0-2.0 m/s), RandomWalk
        mobSta.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                                "Bounds", RectangleValue(Rectangle(0, scenarioSize, 0, scenarioSize)),
                                "Speed", StringValue("ns3::UniformRandomVariable[Min=1.0|Max=2.0]"), // CORRIGIDO
                                "Distance", DoubleValue(10.0));
    }
    else
    {
        // Tabela 1: Estático (0 km/h)
        mobSta.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    }
    mobSta.Install(sta);

    // ===== PILHA TCP/IP =====
    InternetStackHelper stack;
    stack.Install(s2);
    stack.Install(s1);
    stack.Install(s0);
    stack.Install(ap);
    stack.Install(sta);

    // ===== ENDEREÇAMENTO IP (Figura 1) =====
    Ipv4AddressHelper addr;
    addr.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer i_s2s1 = addr.Assign(d_s2s1);
    addr.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer i_s1s0 = addr.Assign(d_s1s0);
    addr.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer i_s0ap = addr.Assign(d_s0ap);
    addr.SetBase("192.168.0.0", "255.255.255.0");
    Ipv4InterfaceContainer i_ap = addr.Assign(apDev);
    Ipv4InterfaceContainer i_sta = addr.Assign(staDevs);
    
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    
    Ipv4Address serverIP = i_s2s1.GetAddress(0); // IP do s2
    
    std::cout << "Servidor (s2): " << serverIP << "\n";
    std::cout << "AP: " << i_ap.GetAddress(0) << "\n";
    std::cout << "Clientes: " << i_sta.GetAddress(0) << " a " 
              << i_sta.GetAddress(nClients-1) << "\n\n";

    // ===== APLICAÇÕES (Tabela 1) =====
    // *** LÓGICA DE DOWNLOAD (Servidor -> Cliente) ***

    uint16_t portTcp = 9;
    uint16_t portUdp = 10;
    
    uint32_t nTcp = 0, nUdp = 0;
    // O 'else' captura "Misto", "MIXED", ou qualquer outra string
    if (protocol == "TCP") nTcp = nClients;
    else if (protocol == "UDP") nUdp = nClients;
    else { nTcp = nClients/2; nUdp = nClients - nTcp; } 

    ApplicationContainer clientSinks; // Receptores (Sinks) nos clientes

    // ===== CLIENTES TCP (Sinks) =====
    if (nTcp > 0)
    {
        NodeContainer tcpStaNodes;
        for(uint32_t i = 0; i < nTcp; i++)
        {
            tcpStaNodes.Add(sta.Get(i));
        }
        
        PacketSinkHelper sink("ns3::TcpSocketFactory", 
                              InetSocketAddress(Ipv4Address::GetAny(), portTcp));
        clientSinks.Add(sink.Install(tcpStaNodes));
    }
    
    // ===== CLIENTES UDP (Sinks) =====
    if (nUdp > 0)
    {
        NodeContainer udpStaNodes;
        for(uint32_t i = nTcp; i < nClients; i++)
        {
            udpStaNodes.Add(sta.Get(i));
        }

        PacketSinkHelper sink("ns3::UdpSocketFactory",
                              InetSocketAddress(Ipv4Address::GetAny(), portUdp));
        clientSinks.Add(sink.Install(udpStaNodes));
    }
    clientSinks.Start(Seconds(0.0));
    clientSinks.Stop(Seconds(simTime + 10)); // Roda 10s a mais para pegar pacotes atrasados

    ApplicationContainer serverSenders; // Emissores (Senders) no servidor

    // ===== SERVIDOR TCP (Senders) =====
    for (uint32_t i = 0; i < nTcp; i++)
    {
        Ipv4Address clientIP = i_sta.GetAddress(i);
        BulkSendHelper server("ns3::TcpSocketFactory",
                              InetSocketAddress(clientIP, portTcp));
        server.SetAttribute("MaxBytes", UintegerValue(0)); 
        server.SetAttribute("SendSize", UintegerValue(1500));
        serverSenders.Add(server.Install(s2));
    }

    // ===== SERVIDOR UDP (Senders) =====
    for (uint32_t i = 0; i < nUdp; i++)
    {
        Ipv4Address clientIP = i_sta.GetAddress(nTcp + i);
        OnOffHelper server("ns3::UdpSocketFactory",
                           InetSocketAddress(clientIP, portUdp));
        server.SetConstantRate(DataRate("512kbps"), 512);
        serverSenders.Add(server.Install(s2));
    }

    serverSenders.Start(Seconds(1.0)); // Inicia 1s depois dos sinks
    serverSenders.Stop(Seconds(simTime));

    std::cout << nTcp << " clientes TCP, " << nUdp << " clientes UDP\n";
    std::cout << "\n==========================================\n";
    std::cout << "     EXECUTANDO SIMULAÇÃO (" << simTime << "s)\n";
    std::cout << "==========================================\n";

    // ===== FLOW MONITOR =====
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();

    Simulator::Stop(Seconds(simTime + 5)); // Roda 5s a mais para garantir a coleta
    Simulator::Run();

    // ===== RESULTADOS =====
    std::cout << "\n==========================================\n";
    std::cout << "            RESULTADOS\n";
    std::cout << "==========================================\n\n";

    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();

    uint64_t totalTx = 0, totalRx = 0, totalLost = 0, totalBytes = 0;
    double totalDelay = 0;

    for (auto const& f : stats)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(f.first);
        
        bool isDownloadFlow = false;
        if (t.sourceAddress == serverIP)
        {
            for (uint32_t i = 0; i < nClients; i++)
            {
                if (t.destinationAddress == i_sta.GetAddress(i))
                {
                    isDownloadFlow = true;
                    break;
                }
            }
        }

        if (isDownloadFlow)
        {
            totalTx += f.second.txPackets;
            totalRx += f.second.rxPackets;
            totalLost += f.second.lostPackets;
            totalBytes += f.second.rxBytes;
            
            if (f.second.rxPackets > 0)
            {
                totalDelay += f.second.delaySum.GetSeconds();
            }
        }
    }

    double avgDelay = (totalRx > 0) ? (totalDelay / totalRx) * 1000 : 0; // em ms
    double throughput = (totalBytes * 8.0) / (simTime * 1e6); // em Mbps
    double lossRate = (totalTx > 0) ? (100.0 * totalLost / totalTx) : 0; // em %

    std::cout << "\n---------------------------------------\n";
    std::cout << "TX: " << totalTx << " | RX: " << totalRx 
              << " | Lost: " << totalLost << "\n";
    std::cout << "Perda: " << std::fixed << std::setprecision(2) << lossRate << "%\n";
    std::cout << "Atraso: " << std::setprecision(3) << avgDelay << " ms\n";
    std::cout << "Vazão: " << std::setprecision(3) << throughput << " Mbps\n";
    std::cout << "==========================================\n\n";

    // ===== SALVAR RESULTADOS =====
    // Gera um nome de arquivo único para esta configuração
    std::string filename = "resultados_" + protocol + "_" +
                           std::to_string(nClients) + "clientes_" +
                           (mobility ? "movel" : "estatico") + ".txt";
    
    std::ofstream out(filename);
    out << "Clientes," << nClients << "\n";
    out << "Protocolo," << protocol << "\n";
    out << "Mobilidade," << (mobility ? "Sim" : "Nao") << "\n";
    out << "TempoSim," << simTime << "\n";
    out << "PacotesTransmitidos," << totalTx << "\n";
    out << "PacotesRecebidos," << totalRx << "\n";
    out << "PacotesPerdidos," << totalLost << "\n";
    out << "TaxaPerda," << std::fixed << std::setprecision(2) << lossRate << "\n";
    out << "AtrasoMedio_ms," << std::setprecision(3) << avgDelay << "\n";
    out << "Vazao_Mbps," << std::setprecision(3) << throughput << "\n";
    out.close();

    std::cout << "Salvo em: " << filename << "\n\n";

    Simulator::Destroy();
    return 0;
}
