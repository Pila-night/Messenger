#pragma once

#include <cstdint>
#include <memory>
#include <QByteArray>
#include <QIODevice>
#include <QString>
#include <CRC.h> // Библиотека с https://github.com/d-bahr/CRCpp
#include "ByteBuffer.h"

enum class PacketType : uint8_t {
    Register,
    Auth,
    Message,
    ServerResponse,
    AuthChallenge
};

class Packet {
protected:
    virtual void serializeData(ByteBuffer& buffer) const = 0;
    virtual void deserializeData(ByteBuffer& buffer) = 0;

    static void serializeString(ByteBuffer& buffer, const QString& str);
    static QString deserializeString(ByteBuffer& buffer);

public:
    QByteArray serialize() const;
    static std::shared_ptr<Packet> deserialize(const QByteArray& data);

    virtual PacketType getType() const = 0;
    virtual ~Packet() = default;

private:
    QByteArray crc(const ByteBuffer& buffer) const;
};

class Register : public Packet {
private:
    QString username;
    QString password;

protected:
    void serializeData(ByteBuffer& buffer) const override;
    void deserializeData(ByteBuffer& buffer) override;

public:
    PacketType getType() const override;
    QString getUsername() const;
    void setUsername(const QString &text);
    QString getPassword() const;
    void setPassword(const QString &text);
};

class Auth : public Packet {
private:
    QString username;
    QString password;

protected:
    void serializeData(ByteBuffer& buffer) const override;
    void deserializeData(ByteBuffer& buffer) override;

public:
    PacketType getType() const override;
    QString getUsername() const;
    void setUsername(const QString &text);
    QString getPassword() const;
    void setPassword(const QString &text);
};

class Message : public Packet {
private:
    QString from;
    QString text;

protected:
    void serializeData(ByteBuffer& buffer) const override;
    void deserializeData(ByteBuffer& buffer) override;

public:
    PacketType getType() const override;
    QString getFrom() const;
    void setFrom(const QString &text);
    QString getText() const;
    void setText(const QString &str);
};

class ServerResponse : public Packet {
private:
    QString message;

protected:
    void serializeData(ByteBuffer& buffer) const override;
    void deserializeData(ByteBuffer& buffer) override;

public:
    PacketType getType() const override;
    QString getResponse() const;
    void SetResponse(const QString &text);
};

class AuthChallenge : public Packet {
private:
    QString salt;

protected:
    void serializeData(ByteBuffer& buffer) const override;
    void deserializeData(ByteBuffer& buffer) override;

public:
    PacketType getType() const override;
    QString getSalt() const;
    void setSalt(const QString &s);
};
