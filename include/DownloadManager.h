/**
 * @file DownloadManager.h
 * @author Diego França Tozetto (diego@geolumini.com.br)
 * @brief 
 * @version 0.1
 * @date 10/02/2020
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once

////////////////////////////////////////////////////////////////////////
//                          INCLUDES
////////////////////////////////////////////////////////////////////////
#include <Arduino.h>
#include <iostream> 
#include <Preferences.h>
#include <ArduinoJson.h>
#include "CommonStorage.h"
#include "VidoGsmManager.h"
#include "VidoArduinoHttpClient.h"
#include "Utils.h"
#include "Download.h"

using namespace std; 

////////////////////////////////////////////////////////////////////////
//                          DEFINES
////////////////////////////////////////////////////////////////////////
// Diretorios padroes.
#define CURRENT_DIR "/DOWNLOADS/CURRENT"
#define QUEUE_DIR "/DOWNLOADS/QUEUE"
#define FINISHED_DIR "/DOWNLOADS/FINISHED"
#define ERROR_DIR "/DOWNLOADS/ERROR"

// Prefs
// Tamanho máximo das keys e namespace = 15 caracteres
#define NAMESPACE_PREFS "downloadManager"
#define KEY_HAS_DOWNLOAD_IN_PROGRESS "hasDownload"
#define KEY_LAST_DOWNLOAD_ID "lastId"
#define KEY_QUEUED_DOWNLOADS "queuedDownloads"

// TAG 
#define TAG_DOWNLOAD_MANAGER "[DOWNLOAD_MANAGER]"

/////////////////////////////////////////////////////////////////////////
//                    DEFINIÇÕES DE ERROS
/////////////////////////////////////////////////////////////////////////
#define DWN_ERR_MSG "[%lu] Ocorreu um erro. Origem: [%s] | Codigo: %d | Mensagem: (%s)"

// 256~...
#define DWN_ERR_SRC_BEGIN "begin"
#define DWN_ERR_0001 0x100
#define DWN_ERR_MSG_0001 "Falha ao iniciar Download Manager. Semaphore 'xSemaphore' nao foi criado."
#define DWN_ERR_0002 0x101
#define DWN_ERR_MSG_0002 "Falha ao iniciar Download Manager. StorageController nao foi inicializado."
#define DWN_ERR_0003 0x102
#define DWN_ERR_MSG_0003 "Falha ao iniciar Download Manager. Diretorio nao existe."
#define DWN_ERR_0004 0x103
#define DWN_ERR_MSG_0004 "Falha ao iniciar Download Manager. Semaphore 'xSemaphore' nao foi criado."
#define DWN_ERR_0005 0x104
#define DWN_ERR_MSG_0005 "Falha ao iniciar Download Manager. Semaphore 'xSemaphoreList' nao foi criado."

#define DWN_ERR_SRC_ADD "add"
#define DWN_ERR_0006 0x105
#define DWN_ERR_MSG_0006 "Falha ao adicionar download. Gerenciador nao inicializado."
#define DWN_ERR_0007 0x106
#define DWN_ERR_MSG_0007 "Falha ao adicionar download. Download nao foi criado."
#define DWN_ERR_0008 0x107
#define DWN_ERR_MSG_0008 "Falha ao adicionar download. Arquivo ja existe."
#define DWN_ERR_0009 0x108
#define DWN_ERR_MSG_0009 "Falha ao adicionar download. Erro ao carregar preferences."
#define DWN_ERR_0010 0x109
#define DWN_ERR_MSG_0010 "Falha ao adicionar download. Erro ao abrir o arquivo."
#define DWN_ERR_0011 0x10A
#define DWN_ERR_MSG_0011 "Falha ao adicionar download. Erro ao serelizar o arquivo JSON."
#define DWN_ERR_0012 0x10B
#define DWN_ERR_MSG_0012 "Falha ao adicionar download. Erro ao salvar parametros no preferences."

#define DWN_ERR_SRC_START "start"
#define DWN_ERR_0013 0x10C
#define DWN_ERR_MSG_0013 "Falha ao comecar download. Gerenciador nao inicializado."
#define DWN_ERR_0014 0x10D
#define DWN_ERR_MSG_0014 "Falha ao comecar download. Download ja comecou."
#define DWN_ERR_0015 0x10E
#define DWN_ERR_MSG_0015 "Falha ao comecar download. Nao foi possivel carregar downloads do diretorio."

#define DWN_ERR_SRC_STOP "stop"
#define DWN_ERR_0016 0x10F
#define DWN_ERR_MSG_0016 "Falha ao parar download. Gerenciador nao inicializado."
#define DWN_ERR_0017 0x110
#define DWN_ERR_MSG_0017 "Falha ao parar download. Download ainda nao iniciou."

#define DWN_ERR_SRC_MOVE_TO "moveTo"
#define DWN_ERR_0018 0x111
#define DWN_ERR_MSG_0018 "Falha ao mover arquivo. Parametro 'statusMoveTo' incorreto."
#define DWN_ERR_0019 0x112
#define DWN_ERR_MSG_0019 "Falha ao mover arquivo. Caminho incorreto ao tentar mover arquivo."

#define DWN_ERR_SRC_UPDATE_FILE "updateFile"
#define DWN_ERR_0020 0x113
#define DWN_ERR_MSG_0020 "Falha ao atualizar arquivo. Parametro 'updateType' incorreto."
#define DWN_ERR_0021 0x114
#define DWN_ERR_MSG_0021 "Falha ao atualizar download. Erro ao abrir o arquivo."
#define DWN_ERR_0022 0x115
#define DWN_ERR_MSG_0022 "Falha ao atualizar download. Erro ao serelizar o arquivo JSON."

#define DWN_ERR_SRC_GET_NEXT_DOWNLOAD_FOLDER "getNextDownloadFolder"
#define DWN_ERR_0023 0x116
#define DWN_ERR_MSG_0023 "Falha ao obter download. Erro ao carregar preferences."

#define DWN_ERR_SRC_FIND_NEXT_DOWNLOAD "findNextDownload"
#define DWN_ERR_0024 0x117
#define DWN_ERR_MSG_0024 "Falha ao carregar informacoes do download. Erro ao abrir diretorio."
#define DWN_ERR_0025 0x118
#define DWN_ERR_MSG_0025 "Falha ao carregar informacoes do download. Erro ao processar diretorio."
#define DWN_ERR_0026 0x119
#define DWN_ERR_MSG_0026 "Falha ao carregar informacoes do download. Erro ao salvar parametros no preferences 'queuedDownloads'."
#define DWN_ERR_0027 0x11A
#define DWN_ERR_MSG_0027 "Falha ao carregar informacoes do download. Erro ao salvar parametros no preferences 'downloadInProgress'."

#define DWN_ERR_SRC_LOAD_DOWNLOAD_INFO "loadDownloadInfo"
#define DWN_ERR_0028 0x11B
#define DWN_ERR_MSG_0028 "Falha ao carregar informacoes do download. Erro ao abrir arquivo."
#define DWN_ERR_0029 0x11C
#define DWN_ERR_MSG_0029 "Falha ao carregar informacoes do download. Erro ao desirelizar o arquivo JSON."
#define DWN_ERR_0030 0x11D
#define DWN_ERR_MSG_0030 "Falha ao carregar informacoes do download. Informacoes contidas no arquivo estao incorretas."
#define DWN_ERR_0031 0x11E
#define DWN_ERR_MSG_0031 "Falha ao carregar informacoes do download. Falha ao indentificar o tamanho do arquivo."
#define DWN_ERR_0032 0x11F
#define DWN_ERR_MSG_0032 "Falha ao carregar informacoes do download. Arquivo ja existe."
#define DWN_ERR_0033 0x120
#define DWN_ERR_MSG_0033 "Falha ao carregar informacoes do download. Não conseguiu remover o arquivo de destino com o mesmo nome."
#define DWN_ERR_0034 0x121
#define DWN_ERR_MSG_0034 "Falha ao carregar informacoes do download. Erro ao criar diretorio de destino."
#define DWN_ERR_0035 0x122
#define DWN_ERR_MSG_0035 "Falha ao carregar informacoes do download. Erro ao mover arquivo."
#define DWN_ERR_0036 0x123
#define DWN_ERR_MSG_0036 "Falha ao carregar informacoes do download. Erro no preferences 'downloadInProgress'."
#define DWN_ERR_0037 0x124
#define DWN_ERR_MSG_0037 "Falha ao carregar informacoes do download. Erro no preferences 'queuedDownloads'."
#define DWN_ERR_0038 0x125
#define DWN_ERR_MSG_0038 "Falha ao carregar informacoes do download. Erro ao atualizar arquivo."

#define DWN_ERR_SRC_EXECUTE_DOWNLOAD "executeDownload"
#define DWN_ERR_0039 0x126
#define DWN_ERR_MSG_0039 "Falha ao executar download. Falha de conexao ao servidor."
#define DWN_ERR_0040 0x127
#define DWN_ERR_MSG_0040 "Falha ao executar download. Ocorreu Timeout ao tentar acessar o servidor."
#define DWN_ERR_0041 0x128
#define DWN_ERR_MSG_0041 "Falha ao executar download. Resposta invalida do servidor."
#define DWN_ERR_0042 0x129
#define DWN_ERR_MSG_0042 "Falha ao executar download. Status Code: "
#define DWN_ERR_0043 0x12A
#define DWN_ERR_MSG_0043 "Falha ao executar download. Erro ao atualizar arquivo com tamanho do download."

#define DWN_ERR_SRC_SAVE_PROGRESS "saveDownloadProgress"
#define DWN_ERR_0044 0x12B
#define DWN_ERR_MSG_0044 "Falha ao executar download. Erro ao abrir arquivo."
#define DWN_ERR_0045 0x12C
#define DWN_ERR_MSG_0045 "Falha ao executar download. Erro ao atualizar arquivo em execucao."

#define DWN_ERR_SRC_FINISHED_DOWNLOAD "finishedDownload"
#define DWN_ERR_0046 0x12D
#define DWN_ERR_MSG_0046 "Falha ao executar download. Erro ao mover arquivo."
#define DWN_ERR_0047 0x12E
#define DWN_ERR_MSG_0047 "Falha ao executar download. Erro no preferences 'downloadInProgress'."
#define DWN_ERR_0048 0x12F
#define DWN_ERR_MSG_0048 "Falha ao executar download. Erro ao atualizar arquivo finalizado."

#define DWN_ERR_SRC_SET_ERROR_DOWNLOAD "setErrorDownload"
#define DWN_ERR_0049 0x130
#define DWN_ERR_MSG_0049 "Falha ao setar erro. Erro ao mover arquivo."
#define DWN_ERR_0050 0x131
#define DWN_ERR_MSG_0050 "Falha ao setar erro. Erro ao salvar preferences 'downlaodInProgress'."
#define DWN_ERR_0051 0x132
#define DWN_ERR_MSG_0051 "Falha ao setar erro. Erro na atualizacao do arquivo."
#define DWN_ERR_0052 0x133
#define DWN_ERR_MSG_0052 "Falha ao setar erro. Erro ao remover o arquivo baixado."

////////////////////////////////////////////////////////////////////////
//                          ENUMS
////////////////////////////////////////////////////////////////////////
/**
 * @brief Indica o sentido da movimentacao do arquivo entre os diretorios.
 * 
 */
enum DownloadStatusMoveTo_e
{
    MOVE_QUEUE_TO_CURRENT = 0x00,
    MOVE_CURRENT_TO_FINISHED,
    MOVE_CURRENT_TO_ERROR,
    MOVE_QUEUE_TO_ERROR,
    MOVE_NONE
};

/**
 * @brief Indica o estatus do download a ser obtido.
 * 
 */
enum FileSystemOperationResult_e
{        
    FS_RESULT_OK = 0x00,   
    FS_RESULT_DIRECTORY_EMPTY, 
    FS_RESULT_ERROR,
    FS_RESULT_ERROR_MOVE_FROM_QUEUE,
    FS_RESULT_ERROR_MOVE_FROM_CURRENT    
};

/**
 * @brief Indica os Status do download em execucao.
 * 
 */
enum DownloadStatusExecute_e
{
    STATUS_EXECUTE_BREAK = 0x00,
    STATUS_EXECUTE_DISCONNECTED_NETWORK,
    STATUS_EXECUTE_DISCONNECTED_CLIENT,
    STATUS_EXECUTE_ERROR,
    STATUS_EXECUTE_OK
};

/**
 * @brief Indica em qual diretorio o arquivo deve ser atulizado.
 * 
 */
enum DownloadUpdateType_e
{
    UPDATE_TYPE_IN_PROGRESS = 0x00,
    UPDATE_TYPE_FINISHED,
    UPDATE_TYPE_ERROR
};

/**
 * @brief Esta classe realiza o gerenciamento dos downloads.
 * 
 */
class DownloadManagerClass
{
public:
    /**
     * @brief Contrutor do DownloadManager
     * 
     */
    DownloadManagerClass();
    /**
     * @brief Inicia o DownloadManager.
     * 
     * @return true Iniciado com sucesso.
     * @return false Ocorreu um erro ao iniciar.
     */
    bool begin();
    /**
     * @brief Adicionar um novo download.
     * 
     * @param download Download a ser adicionado.
     * @return true O download foi adicionado com sucesso.
     * @return false Ocorreu um erro ao adicionar o download.
     */
    bool add(Download &download);
    /**
     * @brief Iniciar o download especificado.
     * 
     * @return true O download foi iniciado com sucesso.
     * @return false Ocorreu um erro ao iniciar o download.
     */
    bool start(int idClient);
    /**
     * @brief Parar o download.
     * 
     * @return true Download para com sucesso.
     * @return false Ocorreu um erro ao parar o download.
     */
    bool stop();
    /**
     * @brief Set the Download object
     * 
     * @param download 
     */
    void updateDownload(Download download);
    /**
     * @brief Get the Download object
     * 
     * @param id 
     * @return Download 
     */
    Download getDownload(int id);
    /**
     * @brief Verificar se o Download Manager está rodando.
     * 
     * @return true Caso esteja rodando.
     * @return false Caso não esteja rodando.
     */
    bool isRunning();
    /**
     * @brief Definir o erro mais recente.
     * 
     * @param source Origem do erro.
     * @param id Id do erro.
     * @param message Mensage do erro.
     * @return esp_err_t 
     */
    esp_err_t setLastError(const char *source, int id, const char *message);
    /**
     * @brief Get the Last Error object
     * 
     * @return LastError_t 
     */
    LastError_t getLastError();
    /**
     * @brief 
     * 
     */
    int getIdClient();
private:
    /**
     * @brief Verifica se as pastas padroes existem.
     * 
     * @return true Pastas existem ou foram criadas.
     * @return false Pastas nao existem e nao foram criadas.
     */
    bool createDefaultFolder();
    /**
     * @brief Salva os parametros no preferences.
     * 
     * @param lastId Ultimo 'id' registrado.
     * @param queueDir Numero de arquivos na pasta QUEUE.
     * @return true Parametros salvos com sucesso.
     * @return false Erro ao salvar parametros.
     */
    bool saveParameters(int lastId, int queueDir);
    /**
     * @brief Carrega os parametros do preferences.
     * 
     * @return true Parametros carregados com sucesso.
     * @return false Erro ao carregar parametros.
     */
    bool loadParameters(int &lastId, int &queuedDownloads);
    /**
     * @brief Carrega downloads no vector.
     * 
     * @return true Carregou corretamente.
     * @return false Nao conseguiu cerregar.
     */
    bool loadDownloadVector();
    /**
     * @brief Busca o download no vector.
     * 
     * @param id Identificador do download.
     * @return pair <int, Download> Posicao e objeto do download.
     */
    pair <int, Download> searchInfo(int id);
    /**
     * @brief Último erro.
     * 
     */
    LastError_t lastError;
    /**
     * @brief Controla se o Downlaod Manager foi inicializado.
     * 
     */
    bool isinitialized;
    /**
     * @brief Lista com os downloads.
     * 
     */
    vector<Download> listDownload;
    /**
     * @brief Id do cliente 
     * 
     */
    int m_idClient;
};

/**
 * @brief Objeto exportado.
 * 
 */
extern DownloadManagerClass DownloadManager;