/*
  RS845.h - Comunicacion multipunto por RS485 con el integrado MAX485
  Copyright (c) 2019 Carlos Andres Betancourt.  All right reserved.
*/

#ifndef Rs485_h
#define Rs485_h

#include "Arduino.h"

//Parametros constantes
#define STX   0xAA  //byte de inicio paquete
#define ETX   0xEE  //byte de final paquete
#define OPR   0x0F  //byte operador para obtener el complemento

//Descripción de la interfaz de la liberia
class Rs485 {
  public:   //Accesible al usuario "public" interface (metodos)
    //metodos
    Rs485(uint8_t controlPin);  //Constructor de la clase
    void enviar (const byte* data, const size_t numData);
    byte recibir(byte* data, const size_t numData);

  private:  //Accesible por la libreria "private" interface (atributos)
    uint8_t _controlPin;
};

#endif
