/**
 * @file CacheStatic.cpp
 * @author Vinicius de Sa (vinicius@vido-la.com)
 * @brief 
 * @version 0.1
 * @date 16/08/2019
 * 
 * @copyright Copyright VIDO.LA 2019: Todos os direitos reservados.
 */
#include "CacheStatic.h"

CacheStaticClass::CacheStaticClass()
{
    SystemStatus = SystemStatus_t();
}

CacheStaticClass CacheStatic;