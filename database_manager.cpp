#include "database_manager.h"
#include <QSqlError>
#include <QDebug>

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent) {}

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager inst;
    return inst;
}

bool DatabaseManager::init() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("ultraman_mes.db");

    if (!db.open()) {
        qDebug() << "Database Error:" << db.lastError().text();
        return false;
    }

    QSqlQuery query;
    // 创建核心表结构
    QString sql = "CREATE TABLE IF NOT EXISTS production_records ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "qr_code TEXT, "
                  "scanner_source TEXT, "
                  "glue_time DATETIME, "
                  "press_time DATETIME, "
                  "duration INTEGER, "
                  "status TEXT)";
    return query.exec(sql);
}

bool DatabaseManager::recordGlueStart(const QString &qrCode, const QString &source) {
    QSqlQuery query;
    query.prepare("INSERT INTO production_records (qr_code, scanner_source, glue_time, status) "
                  "VALUES (:qr, :src, :time, 'PENDING')");
    query.bindValue(":qr", qrCode);
    query.bindValue(":src", source);
    query.bindValue(":time", QDateTime::currentDateTime());
    return query.exec();
}

bool DatabaseManager::recordPressStart(const QString &qrCode, int threshold, ProductionRecord &outRecord) {
    QSqlQuery query;
    // 查找最近一条该 SN 的点胶记录
    query.prepare("SELECT id, glue_time FROM production_records "
                  "WHERE qr_code = :qr AND status = 'PENDING' "
                  "ORDER BY glue_time DESC LIMIT 1");
    query.bindValue(":qr", qrCode);

    if (!query.exec() || !query.next()) return false;

    int id = query.value(0).toInt();
    QDateTime glueTime = query.value(1).toDateTime();
    QDateTime pressTime = QDateTime::currentDateTime();
    
    int duration = glueTime.secsTo(pressTime);
    QString status = (duration <= threshold) ? "OK" : "NG";

    // 更新记录
    QSqlQuery update;
    update.prepare("UPDATE production_records SET press_time = :p, duration = :d, status = :s "
                   "WHERE id = :id");
    update.bindValue(":p", pressTime);
    update.bindValue(":d", duration);
    update.bindValue(":s", status);
    update.bindValue(":id", id);

    if (update.exec()) {
        outRecord.id = id;
        outRecord.qrCode = qrCode;
        outRecord.glueTime = glueTime;
        outRecord.pressTime = pressTime;
        outRecord.duration = duration;
        outRecord.status = status;
        return true;
    }
    return false;
}

QList<ProductionRecord> DatabaseManager::getRecentRecords(int limit) {
    QList<ProductionRecord> list;
    QSqlQuery query;
    query.prepare("SELECT id, qr_code, glue_time, press_time, duration, status, scanner_source "
                  "FROM production_records ORDER BY id DESC LIMIT :limit");
    query.bindValue(":limit", limit);
    
    if (query.exec()) {
        while (query.next()) {
            ProductionRecord r;
            r.id = query.value(0).toInt();
            r.qrCode = query.value(1).toString();
            r.glueTime = query.value(2).toDateTime();
            r.pressTime = query.value(3).toDateTime();
            r.duration = query.value(4).toInt();
            r.status = query.value(5).toString();
            r.source = query.value(6).toString();
            list.append(r);
        }
    }
    return list;
}
QList<ProductionRecord> DatabaseManager::getRecordsByDate(const QDate &date) {
    QList<ProductionRecord> list;
    QSqlQuery query;
    QDateTime start(date, QTime(0, 0, 0));
    QDateTime end(date, QTime(23, 59, 59));

    query.prepare("SELECT id, qr_code, glue_time, press_time, duration, status, scanner_source "
                  "FROM production_records WHERE glue_time BETWEEN :start AND :end ORDER BY id DESC");
    query.bindValue(":start", start);
    query.bindValue(":end", end);
    
    if (query.exec()) {
        while (query.next()) {
            ProductionRecord r;
            r.id = query.value(0).toInt();
            r.qrCode = query.value(1).toString();
            r.glueTime = query.value(2).toDateTime();
            r.pressTime = query.value(3).toDateTime();
            r.duration = query.value(4).toInt();
            r.status = query.value(5).toString();
            r.source = query.value(6).toString();
            list.append(r);
        }
    }
    return list;
}
