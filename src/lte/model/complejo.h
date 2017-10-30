/* 
 * File:   complejo.h
 * Author: jlh
 *
 * Created on April 16, 2010, 11:33 AM
 */

#ifndef _COMPLEJO_H
#define	_COMPLEJO_H

#include <math.h>

class complejo
{
public:
    complejo(double re, double im): real(re), imag(im) {};
    complejo(): real(0), imag(0) {};
    complejo(double re): real(re), imag(0) {};
    complejo(const complejo &orig)
    {
        real = orig.real;
        imag = orig.imag;
    }

public:
    double real;
    double imag;

public:
    // operadores
    const complejo& operator= (const complejo &orig)
    {
        real = orig.real;
        imag = orig.imag;
        return (*this);
    }
    const complejo& operator= (const double &num)
    {
        real = num;
        imag = 0;
        return (*this);
    }
    const complejo operator+ (const complejo &orig) const
        {return complejo(real + orig.real, imag + orig.imag);}
    const complejo operator+ (const double &num) const
        {return complejo(real + num, imag);}
    const complejo& operator+= (const complejo &orig)
    {
        real += orig.real;
        imag += orig.imag;
        return (*this);
    }
    const complejo& operator+= (const double &num)
    {
        real += num;
        return (*this);
    }
    const complejo operator- (const complejo &orig) const
        {return complejo(real - orig.real, imag - orig.imag);}
    const complejo operator- (const double &num) const
        {return complejo(real - num, imag);}
    const complejo operator- () const
        {return complejo(-real, -imag);}
    const complejo& operator-= (const complejo &orig)
    {
        real -= orig.real;
        imag -= orig.imag;
        return (*this);
    }
    const complejo& operator-= (const double &num)
    {
        real -= num;
        return (*this);
    }
    const complejo operator* (const complejo &orig) const
        {return complejo(real*orig.real - imag*orig.imag, real*orig.imag + imag*orig.real);}
    const complejo operator* (const double &num) const
        {return complejo(real*num, imag*num);}
    const complejo& operator*= (const complejo &orig)
    {
        double realAux = real*orig.real - imag*orig.imag;
        double imagAux = real*orig.imag + imag*orig.real;
        real = realAux;
        imag = imagAux;
        return (*this);
    }
    const complejo& operator*= (const double &num)
    {
        real *= num;
        imag *= num;
        return (*this);
    }
    const complejo operator/ (const complejo &orig) const
    {
        double realAux = (real*orig.real + imag*orig.imag) / (orig.real*orig.real + orig.imag*orig.imag);
        double imagAux = (imag*orig.real - real*orig.imag) / (orig.real*orig.real + orig.imag*orig.imag);
        return complejo(realAux,imagAux);
    }
    const complejo operator/ (const double &num) const
        {return complejo(real/num, imag/num);}
    const complejo& operator/= (const complejo &orig)
    {
        double realAux = (real*orig.real+imag*orig.imag)/(orig.real*orig.real+orig.imag*orig.imag);
        double imagAux = (imag*orig.real-real*orig.imag)/(orig.real*orig.real+orig.imag*orig.imag);
        real = realAux;
        imag = imagAux;
        return (*this);
    }
    const complejo& operator/= (const double &num)
    {
        real /= num;
        imag /= num;
        return (*this);
    }
    const bool operator== (const complejo &orig) const
        {return ((real == orig.real) && (imag == orig.imag));}
    const bool operator== (const double &num) const
        {return ((real == num) && (imag == 0));}
    const bool operator!= (const complejo &orig) const
        {return !(*this == orig);}
    const bool operator!= (const double &num) const
        {return !(*this == num);}
    const complejo conjugado () const
        {return complejo(real, -imag);}
    const double valorAbs () const
        {return sqrt(real*real + imag*imag);}
    const double valorAbsCuad () const
        {return (real*real + imag*imag);}
    const double fase() const       // devuelve la fase en rad entre [-PI,PI]
        {return atan2(imag,real);
    }
};



#endif	/* _COMPLEJO_H */

