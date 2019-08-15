/*
  RS845.h - Comunicacion multipunto por RS485 con el integrado MAX485
  Copyright (c) 2019 Carlos Andres Betancourt.  All right reserved.
*/

#include "Arduino.h"
#include "RS485.h"

//---------------CONSTRUCTOR-----------------------//
//Función que maneja la creación y configuración de instancias
Rs485::Rs485(uint8_t controlPin) {
  pinMode(controlPin, OUTPUT);
  _controlPin = controlPin;
  digitalWrite(_controlPin, LOW);
}
//----------------------------------------------------------------

// calculamos el CRC de 8 bits
static byte crc8 (const byte *addr, byte len) {
  byte crc = 0;
  while (len--) {
    byte inbyte = *addr++;
    for (byte i = 8; i; i--) {
      byte mix = (crc ^ inbyte) & 0x01;
      crc >>= 1;
      if (mix)
        crc ^= 0x8C;
      inbyte >>= 1;
    }  // fin ciclo for
  }  // fin ciclo while
  return crc;
}
//----------------------------------------------------------------

void Rs485::enviar (const byte* data, const size_t numData) {
  byte pkg[numData + 3];
  pkg[0] = STX;
  for (int i = 0; i < numData; i++) {
    pkg[i + 1] = data[i];
  }
  pkg[numData + 1] = ETX;
  pkg[numData + 2] = crc8(data, numData);

  digitalWrite(_controlPin, HIGH);
  Serial.write(pkg, sizeof(pkg));
  Serial.flush();
  digitalWrite(_controlPin, LOW);

  /*Calcula el complemento que acompañara cada byte del pkg
    //Los valores enviados solo podran ser los siguientes:
    //0F, 1E, 2D, 3C, 4B, 5A, 69, 78, 87, 96, A5, B4, C3, D2, E1, F0
    for (int i = 0; i < numData; i++) {
    //Primer nibble
    pkg[i * 2] = data[i] >> 4;
    pkg[i * 2] = (pkg[i * 2] << 4) | (pkg[i * 2] ^ OPR);

    //Segundo nibble
    uint8_t j = (i * 2) + 1;
    pkg[j] = data[i] & OPR;
    pkg[j] = (pkg[j] << 4) | (pkg[j] ^ OPR);

    Serial.print("Nibble_1:");
    Serial.print(pkg[i * 2], HEX);
    Serial.print("  Nibble_2:");
    Serial.print(pkg[j], HEX);
    Serial.println();
    }*/
}
//----------------------------------------------------------------

byte Rs485::recibir (byte* data, const size_t numData) {
  digitalWrite(_controlPin, LOW);
  if (Serial.available() >= numData + 3 ) {
    const size_t pkgLong = Serial.available();
    byte pkg[pkgLong]; //Buffer para almacenar los bytes recibidos
    Serial.readBytes(pkg, pkgLong);

    //Leemos los datos del buffer hasta encontrar el STX
    uint8_t index = 0;
    bool stxOk = false;
    for (int i = 0; i < pkgLong; i++) {
      if (pkg[i] == STX) {
        stxOk = true;
        index = i;
        break;
      }
    }
    if (stxOk == true) {
      for (int i = index; i < index + numData; i++) {
        data[i] = pkg[i + 1];
      }
      if (pkg[index + numData + 1] == ETX) {
        if (crc8(data, numData) == pkg[index + numData + 2]) {
          return 1;
        }
        else {
          return 3;
        }
      }
      else {
        return 2;
      }
    }
    else {
      return 0;
    }
  }
  else{
    return 0;
  }
}
//----------------------------------------------------------------
