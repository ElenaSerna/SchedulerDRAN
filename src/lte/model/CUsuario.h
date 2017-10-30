/* 
 * File:   CUsuario.h
 * Author: jlh
 *
 * Created on May 25, 2010, 5:37 PM
 */

#ifndef _CUSUARIO_H
#define	_CUSUARIO_H

class CUsuario
{
public:
    CUsuario(int Nsubbandas, int Nsubbandas_CQI, int Nsubframes_promedia, int truncCQI, int NRB_DL);
        // no necesario constructor de copia
    virtual ~CUsuario();

    //void SetXY(double x, double y);
    void SetSINR(double sinr);
    void SetGanancias(double *ganancias);
    void SetThroughputSubbandas(double *throughput);
    void SetThroughputWideband(double throughput_wideband);

    void ScheduleUser(bool wideband, int Nsubbandas_asign, int *Subbandas_asign);       // asignacion al usuario en la subframe actual
    void NoScheduleUser(bool wideband);

    //void GetXY(double *x, double *y);
    double GetSINR();
    int GetNSubbandas();
    void GetGanancias(double *ganancias);
    void GetGananciasTrunc(double *ganancias_trunc);
    int GetNSubframesPromedia();
    double GetThroughputMedio();
    double GetLastThroughput();
    double GetThroughputWideband();
    double GetRelativeThroughputWideband();
    double GetResourceRelativeThroughputWideband();
    void GetThroughputSubbandas(double *throughput);
    void GetRelativeThroughputSubbandas(double *relative_throughput);
    void GetResourceRelativeThroughputSubbandas(double *resource_relative_throughput);


private:
    double m_dSINR;                     // SINR (wideband) experimentada por el usuario (dB)
    int m_iNsubbandas_tot;              // numero de subbandas totales
    int m_iNsubbandas_CQI;              // numero de subbandas con informacion de CQI (si < m_iNsubbandas_tot, es UE-selected subband feedback)
    int m_iNsubframes_promedia;         // numero de subframes donde promedia el throughput/recursos
    int m_itruncCQI;                   // indica si hay truncamiento
    int m_iNRB_DL;                      // numero de RBs del sistema
    double *m_pGanancias;               // ganancias de cada subbanda, sin truncar: m_pGanancias[0], ...m_pGanancias[m_iNsubbandas_tot - 1]
    double *m_pGanancias_ord;           // ganancias de cada subbanda sin truncar, ordenadas de menor a mayor
    double *m_pGanancias_trunc;         // ganancias de cada subbanda truncadas
    double *m_pThroughput_subbanda;     // throughput de cada subbanda, sin truncar: m_pThroughput_subbanda[0], ...m_pThroughput_subbanda[m_iNsubbandas_tot - 1]
    double *m_pThroughput_subbanda_trunc; // throughput de cada subbanda tras truncar las ganancias
    double *m_pVentanaThroughput;       // ventana de valores de throughput a promediar
    int *m_pVentanaResources;           // ventana de recursos asignados a promediar:
                                        //    en caso wideband, 1 indica subframe asignada, 0 en caso contrario
                                        //    en caso no wideband contiene el numero de subbandas asignadas
    int m_iPosVentana;                  // indice del ultimo elemento de la ventana de throughput
    bool m_iVentanaLlena;               // indica si se ha llenado la ventana de valores de throughput a promediar
    double m_dThroughput_medio;         // throughput medio del usuario en las ultimas m_pVentanaThroughput subframes
    double m_dThroughput_wideband;      // throughput del usuario si se ocupa toda la banda de frecuencias
    double *m_pRelative_throughput;     // throughput de subbanda relativos a su valor medio: m_pThroughput_subbanda[i]/m_dThroughput_medio (FD-ETPF)
    double *m_pRelative_throughput_trunc;// throughput de subbanda relativo a su valor medio, truncados (FD-ETPF, TRUNCADO)
    double m_dRelativeThroughput_wideband;// relative throughput para el caso wideband (FD-ETPF, WIDEBAND)
    double m_dResources_medio;          // numero medio de subbandas asignadas
    double *m_pResourceRelative_throughput; // throughput de subbanda relativos al valor medio de recursos: m_pThroughput_subbanda[i]/m_dResources_medio (ERPF)
    double *m_pResourceRelative_throughput_trunc; // throughput de subbanda relativos al valor medio de recursos, truncados (ERPF, TRUNCADO)
    double m_dResourceRelativeThroughput_wideband; // resource relative throughput para el caso wideband (ERPF, WIDEBAND)
    int m_iNsubframes;                  // numero de subframes transcurridas
    //int *m_pBestSubbandas;              // indices de las m_iNsubbandas_CQI mejores subbandas, de mayor a menor throughput
    int m_iSubbandSize;                 // tamano de la subbanda, en RBs


    static const int m_iNsubframes_max;
    static const double m_dThroughput_inicial;
    static const int m_iResources_inicial;
    
    void ReservaMemoria();
    void ActualizaThroughput(bool wideband);
    //void CalcBestSubbandas();
//    void GetThroughputBestSubbandas(double *throughput);
//    void GetRelativeThroughputBestSubbandas(double *relative_throughput);
//    void GetResourceRelativeThroughputBestSubbandas(double *resource_relative_throughput);
//    void GetThroughputBestSubbandasTrunc(double *throughput_trunc);
//    void GetRelativeThroughputBestSubbandasTrunc(double *relative_throughput_trunc);
//    void GetResourceRelativeThroughputBestSubbandasTrunc(double *resource_relative_throughput_trunc);
 
};

#endif	/* _CUSUARIO_H */

