/**
 * @file CacheStatic.h
 * @author Vinicius de Sa (vinicius@vido-la.com)
 * @brief Cache de aplicação.
 * @version 0.1
 * @date 17/10/2019
 * 
 * @copyright Copyright VIDO.LA 2019: Todos os direitos reservados.
 */
#pragma once

#include "StructLibrary.h"

/**
 * @brief Cache da aplicação.
 * 
 */
class CacheStaticClass
{
    public:
        /**
         * @brief Inicializar o objeto CacheStaticClass.
         * 
         */
        CacheStaticClass();
        /**
         * @brief Indica se o engine está habilitado.
         * 
         */
        volatile bool IsEngineEnabled;
        /**
         * @brief Indica o status atual do sistema.
         * 
         */
        SystemStatus_t SystemStatus;
};

extern CacheStaticClass CacheStatic;