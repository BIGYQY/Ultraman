#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include "database_manager.h" // 引入记录结构

class HomePage : public QWidget
{
    Q_OBJECT
public:
    explicit HomePage(QWidget *parent = nullptr);
    void retranslateUi(int langIdx); // 深度汉化接口

    // 后端调用：将最新结果刷到表格首行
    void updateTableWithNewRecord(const ProductionRecord &record);
    void loadHistoryData(const QDate &date); // 按日期加载历史数据

private:
    void setupUI();
    QTableWidget *table;
    QLabel *lblYield;
    int currentLangIdx; // 记录当前语言环境
};

#endif // HOMEPAGE_H
