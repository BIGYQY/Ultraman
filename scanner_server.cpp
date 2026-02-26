#include "scanner_server.h"
#include <QDebug>

ScannerServer::ScannerServer(QObject *parent) : QObject(parent) {
    server = new QTcpServer(this);
}

bool ScannerServer::start(int port) {
    if (!server->listen(QHostAddress::Any, port)) {
        qDebug() << "Scanner Server listen failed on port" << port;
        return false;
    }
    connect(server, &QTcpServer::newConnection, this, &ScannerServer::onNewConnection);
    qDebug() << "Scanner Server started on port" << port;
    return true;
}

void ScannerServer::stop() {
    if (server->isListening()) {
        server->close();
        for (QTcpSocket *socket : clients) {
            socket->disconnectFromHost();
        }
        qDebug() << "Scanner Server Stopped.";
    }
}

void ScannerServer::onNewConnection() {
    QTcpSocket *socket = server->nextPendingConnection();
    clients.append(socket);
    
    QString ip = socket->peerAddress().toString();
    connect(socket, &QTcpSocket::readyRead, this, &ScannerServer::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &ScannerServer::onDisconnected);
    
    qDebug() << "New Scanner Connected:" << ip;
    emit clientConnected(ip); // 发射信号
}

void ScannerServer::onReadyRead() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    // 读取扫码枪发来的数据并去除换行符等杂质
    QByteArray data = socket->readAll();
    QString qrCode = QString::fromUtf8(data).trimmed();
    
    if (!qrCode.isEmpty()) {
        emit qrCodeReceived(qrCode, socket->peerAddress().toString());
    }
}

void ScannerServer::onDisconnected() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        QString ip = socket->peerAddress().toString();
        clients.removeAll(socket);
        socket->deleteLater();
        qDebug() << "Scanner Disconnected:" << ip;
        emit clientDisconnected(ip); // 发射信号
    }
}
