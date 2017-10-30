/* 
 * File:   CCalculaScheduling.h
 * Author: urano
 *
 * Created on October 19, 2010, 9:54 AM
 */

#ifndef _CCALCULASCHEDULING_H
#define	_CCALCULASCHEDULING_H

#include <stdio.h>
#include <stdlib.h>
#include "TiposScheduling.h"
#include "CUsuario.h"


// Cabecera
class CCalculaScheduling
{
public:

    CCalculaScheduling(const char *scheduling_results, T_UL_DL ULDL, int Nsubf, int Nsubf_promedia, int Nsubbands, int Nsubbands_user,
										int Nsubbands_CQI, int NRB, int TruncCQI, int Debug, unsigned int Semilla, int Scheduler_type, double Sinr_gain_min,
										double Sinr_gain_max, int Sinr_gain_points, int Schedgain_sector_file, double *SINR_avg, double **Ganancias, int NSectores, int Nusers);
    virtual ~CCalculaScheduling();
    void EjecutaScheduling();
    void CalculaThroughput();

private:

    T_UL_DL m_iULDL;                        // uplink/downlink
    int m_iNsubframes_tot;                  // numero de subframes a simular
    int m_iNsubframes_promedia;             // numero de subframes donde promedia el throughput
    int m_iNsubframes; 						// subframes transcurridas
    int m_iNsubbandas;                      // numero de subbandas totales
    int m_iNsubbandas_usuario;              // numero maximo de subbandas por usuario
    int m_iNsubbandas_CQI;                  // numero de subbandas con informacion de CQI
    int m_iNSectores;						// Número de sectores
    int m_iNRB;                             // numero de RBs totales
    int m_iNusuarios;                       // numero de usuarios
    int m_iScheduler;                       // algoritmo de scheduling
    unsigned int m_iSemilla;                // semilla generacion numeros aleatorios
    int m_iDebug;                           // indica si se da informacion completa en el fichero de log
    int m_iTruncCQI;                        // indica si se emplean valores de CQI truncados
    const char *m_pPath_results;                  // directorio de resultados
    double m_dSinrMejoraUsuario_min;        // sinr minima en la curva de ganancia de scheduling
    double m_dSinrMejoraUsuario_max;        // sinr maxima en la curva de ganancia de scheduling
    int m_iNumIntervalosMejoraUsuario;      // numero puntos en la curva de ganancia de scheduling
    int m_iFlagSectorFile;                  // indica si se generan curvas de scheduling gain por cada sector
    TRATE_UNITS m_iUnits_curve;             // tipo de unidades de la curva de throughput, a la cual se referiran los resultados
    double *m_pSINR_avg;					// valores de SINR medios de cada usuario
    double **m_pGanancias_subbanda;			// matriz con los valores de SINR inst para cada usuario y subbanda
    double **m_pGanancias_subbanda_trunc;    // id, truncadas
    double **m_pThroughput_subbanda;         // throughput de subbanda -> se obtiene de la matriz de ganancias-subbandas
    double **m_pThroughput_subbanda_trunc;   // id, truncados
    double *m_pGanancias_subbanda_usuario;
    double *m_pThroughput_subbanda_usuario;
    double *m_pGanancias_subbanda_usuario_trunc;
    double *m_pThroughput_subbanda_usuario_trunc;



    FILE *m_pFichCurvaTp;
    double *m_pCurva_sinr;                  // valores de sinr en la curva de throughput
    double *m_pCurva_throughput;            // valores de throughput en la curva de throughput
    CUsuario **m_pUsuarios;                 // usuarios activos
    double *m_pMejoraUsuariosSector;        // ganancia de scheduling de usuarios por sector
    double *m_pMejoraUsuariosGlobal;        // id escenario global
    int *m_pNumOcurrenciasMejoraGlobal;     // numero de sectores con aportacion a cada punto de la curva de mejora por scheduling
    FILE *m_pFichMejora;                    // fichero de ganancia de scheduling global
    double m_dDeltasinr_mejora;
    FILE *m_pFichThroughputGlobal;          // fichero global de throughput de cada sector
    double m_dNo_data;
	int m_itruncCQI;

    //int m_iNpuntos_curva_sinr;
    //char **m_pId_sectores;
    //int m_iNsectores;
    //int m_iMaxNsector;

    void ReservaMemoria();
    void LiberaMemoria();
    void AbreFicheros();
    void CierraFicheros();
    void SetGananciasTruncadas(double **m_pGanancias);
    void SetThroughputTruncado(double **m_pThroughput);
    double * GetValorUsuario(double **m_matrix,int row, int cols);
    //bool LeeCurvaThroughput(FILE *fich, int *Nelem_curva, double *sinr, double *tp) const;
    //double DevuelveThroughput(double *curva_sinr, double *curva_throughput, int npuntos_curva, double sinr) const;
    //bool LeeIdSectores(FILE *fich, char **id_sectores, int *nsectores, int *indice_sector_max) const;
    void EjecutaSchedulingSector(int idSector, FILE *fichlog, FILE *fichusers, FILE *fichhist);
//    void EjecutaSchedulingSector(int idSector);
};


#endif	/* _CCALCULASCHEDULING_H */

