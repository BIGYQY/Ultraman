#ifndef SCANNER_SERVER_H
#define SCANNER_SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

/*
 * ScannerServer: 面向扫码枪的 TCP 数据接收塔
 * 支持双工位（点胶/热压）连接识别。
 */
class ScannerServer : public QObject
{
    Q_OBJECT
public:
    explicit ScannerServer(QObject *parent = nullptr);
    bool start(int port);
    void stop(); // 新增：停止监听

signals:
    // 当收到有效的二维码 SN 时触发
    void qrCodeReceived(const QString &qrCode, const QString &ip);
    void clientConnected(const QString &ip);    // 新增：连接信号
    void clientDisconnected(const QString &ip); // 新增：断开信号

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();

private:
    QTcpServer *server;
    QList<QTcpSocket*> clients;
};

#endif // SCANNER_SERVER_H
