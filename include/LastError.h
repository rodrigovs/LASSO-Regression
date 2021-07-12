#pragma once

#include <Arduino.h>

class LastError
{
public:
    LastError()
    {
        m_source = "";
        m_code = 0;
        m_message = "";
    }

    LastError(String source, int code, String message)
    {
        m_source = source;
        m_code = code;
        m_message = message;
    }

    void copyTo(LastError &error)
    {
        error.setSource(m_source);
        error.setCode(m_code);
        error.setMessage(m_message);
    }

    inline String toString() { return "Erro [" + m_source + ":" + String(m_code) + "]: " + m_message; }
    inline String getSource() { return m_source; }
    inline int getCode() { return m_code; }
    inline String getMessage() { return m_message; }
    inline void setSource(String source) { m_source = source; }
    inline void setCode(int code) { m_code = code; }
    inline void setMessage(String message) { m_message = message; }
private:
    String m_source;
    int m_code;
    String m_message;
};