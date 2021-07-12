/************************************************************************
* Arquivo: GeoCoordinate.cpp
* Autor: Vinicius de Sa
* Data: 12/12/2017
* Objetivo: Contem a classe GeoCoordinate que define uma localizacao
* global.
************************************************************************/
#include "GeoCoordinate.h"

/**
 *   @brief      construtor da classe GeoCoordinate.
 */
GeoCoordinate::GeoCoordinate()
{
    _latitude = 0.0;
    _longitude = 0.0;
    _azimuth = 0.0;
    _isValid = false;
}

/**
 *   @brief      construtor da classe GeoCoordinate.
 *   @param      double latitude     latitude desta coordenada.
 *   @param      double longitude    longitude desta coordenada.
 */
GeoCoordinate::GeoCoordinate(double latitude, double longitude)
{
    _latitude = latitude;
    _longitude = longitude;
    _azimuth = 0.0;
    _isValid = false;
}

/**
 *   @brief      construtor da classe GeoCoordinate.
 *   @param      double latitude     latitude desta coordenada.
 *   @param      double longitude    longitude desta coordenada.
 *   @param      double azimuth      azimute desta coordenada.
 */
GeoCoordinate::GeoCoordinate(double latitude, double longitude, double azimuth)
{
    _latitude = latitude;
    _longitude = longitude;
    _azimuth = azimuth;
    _isValid = false;
}

/**
 *   @brief      retornar a latitude desta coordenada.
 *   @return     double      latitude desta coordenada em graus decimais.
 */
double GeoCoordinate::getLatitude()
{
    return _latitude;
}

/**
 *   @brief      retornar a longitude desta coordenada.
 *   @return     double      longitude desta coordenada em graus decimais.
 */
double GeoCoordinate::getLongitude()
{
    return _longitude;
}

/**
 *   @brief      retornar o azimute desta coordenada.
 *   @return     double      azimute desta coordenada.
 */
double GeoCoordinate::getAzimuth()
{
    return _azimuth;
}

/**
 *   @brief      configurar a latitude desta coordenada.
 *   @param      double      latitude desta coordenada em graus decimais.
 */
void GeoCoordinate::setLatitude(double latitude)
{
    _latitude = latitude;
}

/**
 *   @brief      configurar a longitude desta coordenada.
 *   @param      double      longitude desta coordenada
 */
void GeoCoordinate::setLongitude(double longitude)
{
    _longitude = longitude;
}

/**
 *   @brief      configurar o azimute desta coordenada.
 *   @param      double      azimute desta coordenada
 */
void GeoCoordinate::setAzimuth(double azimuth)
{
    _azimuth = azimuth;
}

/**
 *  @brief  retornar a distancia em metros entre duas posicoes, ambas
 *	        especificadas como graus decimais (com sinal) de latitude e longitude.
 *			Usa a computacao de distancia great-circle para a esfera hipotetica
 *			de raio de 6372795 metros. Como a Terra nao e uma esfera exata, erros
 *			de arredondamento podem ser de ate 0.5. Cortesia de Maarten Lamers.
 *  @param      GeoCoordinate coordinate    coordenada a ser comparada.
 *	@return     double                      Distancia.
 */
double GeoCoordinate::distanceTo(GeoCoordinate coordinate)
{
    double lat1 = _latitude;
    double long1 = _longitude;
    double lat2 = coordinate.getLatitude();
    double long2 = coordinate.getLongitude();

    double delta = radians(long1 - long2);
    double sdlong = sin(delta);
    double cdlong = cos(delta);

    lat1 = radians(lat1);
    lat2 = radians(lat2);

    double slat1 = sin(lat1);
    double clat1 = cos(lat1);
    double slat2 = sin(lat2);
    double clat2 = cos(lat2);

    delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
    delta = sq(delta);
    delta += sq(clat2 * sdlong);
    delta = sqrt(delta);

    double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
    delta = atan2(delta, denom);

    return delta * 6372795;
}

/**
 *  @brief      retornar uma string contendo os dados de geolocalizacao.
 *	@return     String contendo os dados de geolocalizacao.
 */
String GeoCoordinate::toString(String separator)
{
    return String(_latitude, 6) + separator + String(_longitude, 6) + separator + String(_azimuth);
}

/**
 *  @brief      retornar uma flag informando se a coordenada eh valida ou nao.
 *  @return     True para coordenada valida, false caso contrario.
 */
bool GeoCoordinate::isValid()
{
    return _isValid;
}

/**
 *  @brief      retornar as coordenadas do quadrante ao qual esta coordenada faz parte.
 *  @return     GeoCoordinate contendo as coordenadas do quadrante.
 */
GeoCoordinate GeoCoordinate::getQuadrantCoordinates()
{
    GeoCoordinate quadrantCoordinates;

    quadrantCoordinates.setLatitude(ceil(_latitude * 4) / 4);
    quadrantCoordinates.setLongitude(ceil(_longitude * 4) / 4);

    return quadrantCoordinates;
}
