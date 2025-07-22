#include "logger.h"

Logger* Logger::Instance = nullptr;


/**
 * @brief Приватный конструктор класса Logger.
 * @details Инициализирует флаг состояния файла как "закрыт".
 */
Logger::Logger() : is_open(false) {}

/**
 * @brief Возвращает единственный экземпляр класса Logger (синглтон).
 * @return Ссылка на экземпляр Logger.
 */
Logger& Logger::getInstance() {
    if (!Instance) {
        Instance = new Logger();
    }
    return *Instance;
}

/**
 * @brief Устанавливает путь к файлу для записи логов.
 * @param filename Путь к файлу логов.
 * @details Если путь пустой, выводится сообщение об ошибке.
 */
void Logger::setLogFile(const QString& filename) {
    if (filename.isEmpty()) {
        qCritical() << "Путь к журналу событий не может быть пустым";
        return;
    }
    currentFile = filename;
}

/**
 * @brief Открывает файл для записи логов.
 * @details Если файл недоступен, создается временный файл в папке Temp.
 */
void Logger::open() {
    if (!is_open) {
        logFile.setFileName(currentFile);
        if (!logFile.open(QIODevice::Append | QIODevice::Text)) {
            qCritical() << "Не удалось открыть файл журнала:" << currentFile;
            qCritical() << "Попытка создания временного файла в Temp.";

            QString tempFilePath = QDir::tempPath() + "/mes.log";
            logFile.setFileName(tempFilePath);
            if (!logFile.open(QIODevice::Append | QIODevice::Text)) {
                qCritical() << "Не удалось открыть временный файл журнала:" << tempFilePath;
                return;
            }
            currentFile = tempFilePath;
        }
        logStream.setDevice(&logFile);
        is_open = true;
    }
}

/**
 * @brief Закрывает файл логов.
 */
void Logger::close() {
    if (is_open) {
        logFile.close();
        is_open = false;
    }
}

/**
 * @brief Записывает сообщение в лог с указанным уровнем.
 * @param level Уровень логирования (DEBUG, INFO, WARNING, CRITICAL, FATAL).
 * @param message Сообщение для записи.
 */
void Logger::log(QtMsgType level, const QString& message) {
    if (!is_open) {
        qCritical() << "Файл журнала закрыт:" << currentFile;
        return;
    }

    QString levelString;
    switch (level) {
    case QtDebugMsg:
        levelString = "DEBUG";
        break;
    case QtInfoMsg:
        levelString = "INFO";
        break;
    case QtWarningMsg:
        levelString = "WARNING";
        break;
    case QtCriticalMsg:
        levelString = "CRITICAL";
        break;
    case QtFatalMsg:
        levelString = "FATAL";
        break;
    }

    QString logMessage = QString("%1 [%2] %3")
                             .arg
                              (QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"),
                              levelString,
                               message);

    logStream << logMessage << Qt::endl;
    qDebug() << logMessage;

    if (level == QtCriticalMsg || level == QtFatalMsg) {
        qCritical() << "Критическая ошибка:" << message;
    }
}

/**
 * @brief Проверяет, открыт ли файл логов.
 * @return true, если файл открыт, иначе false.
 */
bool Logger::isOpen() const {
    return is_open;
}

/**
 * @brief Возвращает текущий путь к файлу логов.
 * @return Путь к файлу логов.
 */
QString Logger::getLogFile() const {
    return currentFile;
}

/**
 * @brief Деструктор класса Logger.
 * @details Закрывает файл логов при уничтожении объекта.
 */
Logger::~Logger() {
    close();
}
