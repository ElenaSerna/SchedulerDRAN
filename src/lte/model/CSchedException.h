/* 
 * File:   CSchedException.h
 * Author: jlh
 *
 * Created on May 27, 2010, 4:12 PM
 */

#ifndef _CSCHEDEXCEPTION_H
#define	_CSCHEDEXCEPTION_H


typedef enum
{
    ERROR_MEMORIA,                  // error de memoria
    ERROR_FICHEROS,                 // error abriendo ficheros
    ERROR_MAB,                      // no se puede usar MAB cuando NRB_DL < 8
    ERROR_PARAMETROS,               // error en la configuracion
    ERROR_SEMILLA,                  // error al establecer la semilla
    ERROR_ESCENARIO                 // error leyendo escenario
} T_ERROR_SCHEDULER;


class CSchedException {
public:
    CSchedException(T_ERROR_SCHEDULER error): m_iError(error) {};
    virtual ~CSchedException() {};

    T_ERROR_SCHEDULER m_iError;

};



#endif	/* _CSCHEDEXCEPTION_H */

