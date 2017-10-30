/* 
 * File:   CScheduler.h
 * Author: jlh
 *
 * Created on May 25, 2010, 5:36 PM
 */

#ifndef _CSCHEDULER_H
#define	_CSCHEDULER_H


#include "CUsuario.h"
#include "TiposScheduling.h"


class CScheduler
{
public:
    CScheduler(T_SCHEDULER Scheduler, T_UL_DL uldl, int Nusuarios, int Nsubbandas, int Nsubbandas_usuario,
               int m_idSector, CUsuario** pusuarios, int debug, double no_data,  FILE *fichlog, FILE *fichusers,
			   double sinrmin_schedgain, double sinrmax_schedgain, int npuntos_schedgain, TRATE_UNITS units_curve);
//	CScheduler(T_SCHEDULER Scheduler, T_UL_DL uldl, int Nusuarios, int Nsubbandas, int Nsubbandas_usuario,
//	               int m_idSector, int debug, double no_data,
//	               double sinrmin_schedgain, double sinrmax_schedgain, int npuntos_schedgain, TRATE_UNITS units_curve);
	// no necesario constructor de copia
    virtual ~CScheduler();

    void RunScheduler(int Nsubframes_tot);

    void GetMejoraUsuario(double* mejora) const;
    double GetCellThroughput() const;

    double * GetValorUsuario(double **m_matrix,int row, int cols);

private:

    T_SCHEDULER m_iScheduler;
    T_UL_DL m_iULDL;
    int m_iNUsuarios;
    int m_iNsubbandas;
    int m_iNsubbandas_usuario;
    int m_idSector;							// sector sobre el que se aplicará el scheduler
    CUsuario **m_pUsuarios;
    int m_iDebug;
    double m_dNo_data;
    FILE *m_pFichLog;                           // fichero de log
    FILE *m_pFichUsers;                         // fichero con informacion inicial y final de los usuarios
    double m_dSinrMejoraUsuario_min;            // sinr minima en la curva de ganancia de scheduling
    double m_dSinrMejoraUsuario_max;            // sinr maxima en la curva de ganancia de scheduling
    int m_iNumIntervalosMejoraUsuario;          // numero puntos en la curva de ganancia de scheduling
    TRATE_UNITS m_iUnits_curve;                 // tipo de unidades de tp para representar resultados
    int m_iNsubframes_tot;
    double m_dThroughput_subframe;
    double m_dThroughput_global;
    double **m_pMetricas;
    int *m_pUsuarioSubbanda;                    // usuario asignado a cada subbanda; si -1, es que no ha recibido asignacion
    int *m_pSubbandasUsuario;                   // subbandas asignadas a cada usuario (pueden ser mas de una)
    double **m_pThroughputUsuarios;             // throughput de cada usuario y subframe --> ¿¿ ES UN VECTOR ??
    double *m_pThroughput_medio;                // throughput medio de cada usuario
    double *m_pThroughput_RR;                   // throughput RR de cada usuario
    int *m_pNsubframes_asign;                   // numero de subframes asignadas a cada usuario
    double m_dMejora_media;                     // mejora media respecto a RR en la celda
    double *m_pMejoraUsuarioVsSinr;             // ganancias de scheduling de usuario vs sinr
    int *m_pNumOcurrenciasMejora_Sinr;          // numero de ocurrencias de cada valor de sinr
    double m_dDeltasinr_mejora;

    void SetMetricas(int fila,double a,double b,double c,double d,double e,double f,double g,double h);
    int MaxColumna(int col, int usuario_excepcion) const;
    bool EsUsuarioAsignado(int usuario, int nsubbandas) const;
//    bool SubbandasLibres(int subbanda, int nsubbandas) const;
    int CalculaK_col(int col) const;
    double Summ(int j, int k, int K_k) const;
    int MaxSummColumna(int col, int K_col_col, int usuario_excepcion) const;
    void AsignaSubbandas(int subbanda, int nsubbandas, int usuario);
    void ScheduleUsers_freq(int subframe);
    void ScheduleUsers_nofreq(int user, int subframe);
    void RunScheduler_freq_DL(int Nsubframes_tot);
    void RunScheduler_freq_UL(int Nsubframes_tot);
    void RunScheduler_nofreq(int Nsubframes_tot);
    void EscribeFicherosLogUsuarios();
    void CalculaMejoraUsuario();

    static const int m_iNSubframes_max;

};

#endif	/* _CSCHEDULER_H */

