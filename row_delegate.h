#ifndef ROW_DELEGATE_H
#define ROW_DELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>

/* 
 * RowDelegate: 像素级色块渲染器
 * 100% 照猫画虎逻辑：
 * - 只有第 0 列 (序号) 会根据 OK/NG 状态进行全方块染色。
 * - OK 状态：#00FF00 背景 + 黑色文字。
 * - NG 状态：#FF0000 背景 + 白色文字。
 * - 其他列：维持纯黑背景 + 白色文字。
 */
class RowDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        // 获取当前行状态 (修正：状态在第 6 列)
        QString status = index.model()->index(index.row(), 6).data().toString();

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setFont(opt.font); 

        // --- 核心逻辑 ---
        if (index.column() == 0) {
            // 第一列染色
            if (status == "OK") {
                painter->fillRect(opt.rect, QColor("#00FF00")); 
                painter->setPen(Qt::black); 
            } else if (status == "NG") {
                painter->fillRect(opt.rect, QColor("#FF0000")); 
                painter->setPen(Qt::white); 
            } else if (status == "PENDING") {
                painter->fillRect(opt.rect, QColor(100, 100, 100));
                painter->setPen(Qt::white);
            } else {
                painter->fillRect(opt.rect, QColor("#000000"));
                painter->setPen(Qt::white);
            }
        } else {
            // 其他列背景
            if (status == "PENDING") {
                painter->fillRect(opt.rect, QColor(50, 50, 50)); 
            } else {
                painter->fillRect(opt.rect, QColor("#000000"));
            }
            painter->setPen(Qt::white);
        }

        painter->drawText(opt.rect, Qt::AlignCenter, opt.text);
        painter->restore();
    }
};

#endif // ROW_DELEGATE_H
