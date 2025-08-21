#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDateTime>
#include <QDir>

/**
 * @brief Класс Logger (синглтону).
 *
 * Предоставляет методы для запси логов.
 * Если основной файл для логов недоступен,
 * логи сохраняются во временной папке (Temp).
 */


class Logger : public QObject {
    Q_OBJECT
private:
    static Logger* Instance;
    QFile logFile;
    QTextStream logStream;
    QString currentFile;
    bool is_open;

    Logger();

public:
    void createLogFile();
    static Logger& getInstance();
    void setLogFile(const QString& filename);
    void open();
    void close();
    void log(QtMsgType level, const QString& message);
    bool isOpen() const;
    QString getLogFile() const;
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

signals:
    void logReceived(const QString& message);
};


#endif // LOGGER_H
