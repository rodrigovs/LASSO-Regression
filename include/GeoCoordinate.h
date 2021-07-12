/************************************************************************
* Arquivo: GeoCoordinate.h
* Autor: Vinicius de Sa
* Data: 12/12/2017
* Objetivo: Contem a classe GeoCoordinate que define uma localizacao
* global.
************************************************************************/
#pragma once

#include <Arduino.h>

class GeoCoordinate
{
  public:
    /**
     *   @brief      construtor da classe GeoCoordinate.
     */
    GeoCoordinate();
    /**
     *   @brief      construtor da classe GeoCoordinate.
     *   @param      double latitude     latitude desta coordenada.
     *   @param      double longitude    longitude desta coordenada.
     */
    GeoCoordinate(double latitude, double longitude);
    /**
     *   @brief      construtor da classe GeoCoordinate.
     *   @param      double latitude     latitude desta coordenada.
     *   @param      double longitude    longitude desta coordenada.
     *   @param      double azimuth      azimute desta coordenada.
     */
    GeoCoordinate(double latitude, double longitude, double azimuth);
    /**
     *   @brief      retornar a latitude desta coordenada.
     *   @return     double      latitude desta coordenada em graus decimais.
     */
    double getLatitude();
    /**
     *   @brief      retornar a longitude desta coordenada.
     *   @return     double      longitude desta coordenada em graus decimais.
     */
    double getLongitude();
    /**
     *   @brief      retornar o azimute desta coordenada.
     *   @return     double      azimute desta coordenada.
     */
    double getAzimuth();
    /**
     *   @brief      configurar a latitude desta coordenada.
     *   @param      double      latitude desta coordenada em graus decimais.
     */
    void setLatitude(double latitude);
    /**
     *   @brief      configurar a longitude desta coordenada.
     *   @param      double      longitude desta coordenada
     */
    void setLongitude(double longitude);
    /**
     *   @brief      configurar o azimute desta coordenada.
     *   @param      double      azimute desta coordenada
     */
    void setAzimuth(double azimuth);
    /**
     *  @brief  retornar a distancia em metros entre duas posicoes, ambas
     *	        especificadas como graus decimais (com sinal) de latitude e longitude.
     *			Usa a computacao de distancia great-circle para a esfera hipotetica
     *			de raio de 6372795 metros. Como a Terra nao e uma esfera exata, erros
     *			de arredondamento podem ser de ate 0.5. Cortesia de Maarten Lamers.
     *  @param      GeoCoordinate coordinate    coordenada a ser comparada.
     *	@return     double                      Distancia.
     */
    double distanceTo(GeoCoordinate coordinate);
    /**
     *  @brief      retornar uma string contendo os dados de geolocalizacao.
     *  @param      separator       separador
     *	@return     String contendo os dados de geolocalizacao.
     */
    String toString(String separator = ",");
    /**
     *  @brief      retornar as coordenadas do quadrante ao qual esta coordenada faz parte.
     *  @return     GeoCoordinate contendo as coordenadas do quadrante.
     */
    GeoCoordinate getQuadrantCoordinates();
    /**
     *  @brief      retornar uma flag informando se a coordenada eh valida ou nao.
     *  @return     True para coordenada valida, false caso contrario.
     */
    bool isValid();

  private:
    double _latitude;
    double _longitude;
    double _azimuth;
    bool _isValid;
};