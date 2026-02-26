#include "homepage.h"
#include "row_delegate.h"
#include <QHeaderView>
#include <QVBoxLayout>
#include <QResizeEvent>

HomePage::HomePage(QWidget *parent) : QWidget(parent), currentLangIdx(0)
{
    setupUI();
}

void HomePage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); // 彻底贴边
    mainLayout->setSpacing(0);

    // 表格初始化
    table = new QTableWidget(0, 7); // 增加一列：来源
    table->setObjectName("productionTable");
    table->setItemDelegate(new RowDelegate(table));
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->verticalHeader()->setVisible(false); // 必须明确隐藏！
    table->verticalHeader()->setDefaultSectionSize(40); 
    
    // 关键修正：移除所有自适应模式，手动分配固定列宽（适配 1200 宽度）
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed); 
    table->horizontalHeader()->setFixedHeight(50);
    table->horizontalHeader()->setStretchLastSection(false);
    
    // 精确分配 1135px 宽度 (1200宽 - 65侧边栏)
    table->setColumnWidth(0, 95);   // 序号
    table->setColumnWidth(1, 195);  // 二维码
    table->setColumnWidth(2, 195);  // 来源
    table->setColumnWidth(3, 195);  // 点胶时间
    table->setColumnWidth(4, 195);  // 热压时间
    table->setColumnWidth(5, 150);  // 耗时
    table->setColumnWidth(6, 110);  // 状态 (稍微补宽，对齐边缘)
    
    table->setShowGrid(true);
    table->setFrameShape(QFrame::NoFrame); // 彻底移除边框阴影
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 强制垂直铺满
    
    mainLayout->addWidget(table);

    // 初始汉化
    retranslateUi(0); // 默认中文

    // 纠正模拟数据：从 1 开始，严谨对标
    auto addRowData = [&](int id, const QString &sn, const QString &src, const QString &t1, const QString &t2, const QString &dur, const QString &st) {
        int row = table->rowCount();
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(QString::number(id)));
        table->setItem(row, 1, new QTableWidgetItem(sn));
        table->setItem(row, 2, new QTableWidgetItem(src));
        table->setItem(row, 3, new QTableWidgetItem(t1));
        table->setItem(row, 4, new QTableWidgetItem(t2));
        table->setItem(row, 5, new QTableWidgetItem(dur));
        table->setItem(row, 6, new QTableWidgetItem(st));
    };

    addRowData(1, "#1982430950", "Scanner A", "15:39:15", "15:39:30", "15s", "OK");
    addRowData(2, "#1982430959", "Scanner B", "15:40:22", "15:42:10", "108s", "NG");
    addRowData(3, "#1982430960", "Scanner A", "15:45:10", "15:45:25", "15s", "OK");
    addRowData(4, "#1982430961", "Scanner A", "15:46:00", "15:46:20", "20s", "OK");
    addRowData(5, "#1982430962", "Scanner C", "15:48:30", "15:51:40", "190s", "NG");
    addRowData(6, "#1982430963", "Scanner B", "15:52:15", "15:52:35", "20s", "OK");
    addRowData(7, "#1982430965", "Scanner A", "15:55:00", "15:55:30", "30s", "OK");
}

void HomePage::retranslateUi(int langIdx)
{
    currentLangIdx = langIdx;
    bool isChn = (langIdx == 0);
    QStringList headers;
    if (isChn) {
        headers << "序号" << "二维码" << "来源" << "点胶时间" << "热压时间" << "耗时" << "状态";
    } else {
        headers << "ID" << "QR Code" << "Source" << "Glue Time" << "Press Time" << "Duration" << "Status";
    }
    table->setHorizontalHeaderLabels(headers);
}

void HomePage::updateTableWithNewRecord(const ProductionRecord &record)
{
    // 检查是否已经存在该 ID 的行（如果是从 PENDING 变成 OK/NG）
    int targetRow = -1;
    for (int i = 0; i < table->rowCount(); ++i) {
        if (table->item(i, 0) && table->item(i, 0)->text().toInt() == record.id) {
            targetRow = i;
            break;
        }
    }

    if (targetRow == -1) {
        table->insertRow(0);
        targetRow = 0;
    }

    table->setItem(targetRow, 0, new QTableWidgetItem(QString::number(record.id)));
    table->setItem(targetRow, 1, new QTableWidgetItem(record.qrCode));
    table->setItem(targetRow, 2, new QTableWidgetItem(record.source));
    
    QString timeFormat = (currentLangIdx == 0) ? "HH:mm:ss" : "HH:mm:ss";
    
    table->setItem(targetRow, 3, new QTableWidgetItem(record.glueTime.toString(timeFormat)));
    table->setItem(targetRow, 4, new QTableWidgetItem(record.status == "PENDING" ? "-" : record.pressTime.toString(timeFormat)));
    table->setItem(targetRow, 5, new QTableWidgetItem(record.status == "PENDING" ? "-" : QString("%1s").arg(record.duration)));
    table->setItem(targetRow, 6, new QTableWidgetItem(record.status));

    // 视觉反馈：PENDING 显示为灰色背景
    QColor bgColor = Qt::transparent;
    if (record.status == "PENDING") {
        bgColor = QColor(100, 100, 100, 128); // 半透明灰色
    }

    for (int col = 0; col < table->columnCount(); ++col) {
        if (table->item(targetRow, col)) {
            table->item(targetRow, col)->setBackground(bgColor);
        }
    }
    
    if (table->rowCount() > 500) {
        table->removeRow(500);
    }
}

void HomePage::loadHistoryData(const QDate &date)
{
    table->setRowCount(0); // 清空实时数据
    QList<ProductionRecord> records = DatabaseManager::instance().getRecordsByDate(date);
    
    // 倒序插入，保持 ID 最大的在上面
    for (const auto &record : records) {
        updateTableWithNewRecord(record);
    }
}
