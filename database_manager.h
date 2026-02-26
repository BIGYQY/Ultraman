#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateTime>

struct ProductionRecord {
    int id;
    QString qrCode;
    QDateTime glueTime;
    QDateTime pressTime;
    int duration; // 秒
    QString status; // OK/NG/PENDING
    QString source; // 数据来源：IP 或 别名
};

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    static DatabaseManager& instance();
    bool init();

    // 状态记录逻辑
    bool recordGlueStart(const QString &qrCode, const QString &source);
    bool recordPressStart(const QString &qrCode, int threshold, ProductionRecord &outRecord);
    
    QList<ProductionRecord> getRecentRecords(int limit = 100);
    QList<ProductionRecord> getRecordsByDate(const QDate &date); // 按日期筛选

private:
    explicit DatabaseManager(QObject *parent = nullptr);
    QSqlDatabase db;
};

#endif // DATABASE_MANAGER_H
