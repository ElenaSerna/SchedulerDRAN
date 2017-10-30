/* 
 * File:   TiposScheduling.h
 * Author: urano
 *
 * Created on July 6, 2010, 2:48 PM
 */

#ifndef _TIPOSSCHEDULING_H
#define	_TIPOSSCHEDULING_H


#define MODSEMILLA                      571223
#define MAX_LINE                        10000	// maximo caracteres por linea??
#define MAX_ELEM_CURVA_TP               1000    // maximo numero de puntos en la curva de throughput vs sinr
#define MAX_SECTORES                    10000   // maximo numero de identificadores de sector en el fichero id_sectores.txt
#define NPARAMS                         26      // numero de parametros de entrada


typedef enum
{
    ROUND_ROBIN,                // RR en t (sin informacion de banda ancha)
    MAXIMUM_RATE,               // Max C/I en t (sin informacion de banda ancha)
    PROPORTIONAL_FAIR,          // PF en t (sin informacion de banda ancha), relativo al throughput
    FREQ_MAXIMUM_RATE,          // Max C/I en tiempo-frecuencia, con informacion de banda ancha
    FREQ_PROPORTIONAL_FAIR,     // PF en tiempo-frecuencia, con informacion de banda ancha, relativo al throughput
    PROPORTIONAL_FAIR_RES,      // PF en t (sin informacion de banda ancha), relativo al numero de subframes asignadas
    FREQ_PROPORTIONAL_FAIR_RES  // PF en tiempo-frecuencia, relativo al numero de subbandas asignadas
} T_SCHEDULER;


typedef enum
{
    UL,
    DL
} T_UL_DL;


typedef enum
{
    BPS,
    KBPS,
    MBPS,
    GBPS
} TRATE_UNITS;


#endif	/* _TIPOSSCHEDULING_H */

