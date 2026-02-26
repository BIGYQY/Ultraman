#include "settingpage.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QInputDialog>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QMap>

SettingPage::SettingPage(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

int SettingPage::getLanguageIndex() const
{
    return comboLang->currentIndex();
}

int SettingPage::getScannerPort() const
{
    return spinPort->value();
}

int SettingPage::getTimeout() const
{
    return spinTimeout->value();
}

QString SettingPage::getScannerAlias(const QString &ip)
{
    for (int i = 0; i < listScanners->count(); ++i) {
        QString txt = listScanners->item(i)->text();
        if (txt.contains(ip)) return txt;
    }
    return ip;
}
void SettingPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 5, 10, 5); // 极致压缩，对齐左上角
    mainLayout->setSpacing(8);

    // 标准样式定义：极致高密度 (对标样板图)
    QString labelStyle = "font-size: 15px; font-weight: normal; color: #FFFFFF;";
    QString inputStyle = "background-color: transparent; border: 1.2px solid #FFFFFF; border-radius: 4px; padding: 1px 5px; color: #FFFFFF; font-size: 15px;";
    QString comboStyle = "QComboBox { background-color: transparent; border: 1.2px solid #FFFFFF; border-radius: 4px; padding: 1px 5px; color: #FFFFFF; font-size: 15px; } "
                         "QComboBox::drop-down { border: none; width: 18px; } "
                         "QComboBox::down-arrow { image: url(:/assets/filter_white.png); width: 8px; height: 8px; }";

    // 1. 监听地址区域
    lblTitleListen = new QLabel("监听地址:");
    lblTitleListen->setStyleSheet(labelStyle);
    mainLayout->addWidget(lblTitleListen);

    QHBoxLayout *rowIp = new QHBoxLayout();
    rowIp->setContentsMargins(10, 0, 0, 0); 
    lblIp = new QLabel("IP:");
    lblIp->setFixedWidth(40);
    lblIp->setStyleSheet(labelStyle);
    editIp = new QLineEdit(getLocalIP());
    editIp->setStyleSheet(inputStyle);
    editIp->setFixedWidth(150); // 进一步瘦身
    rowIp->addWidget(lblIp);
    rowIp->addWidget(editIp);
    rowIp->addStretch();
    mainLayout->addLayout(rowIp);

    QHBoxLayout *rowPort = new QHBoxLayout();
    rowPort->setContentsMargins(10, 0, 0, 0);
    lblPort = new QLabel("端口:");
    lblPort->setFixedWidth(40);
    lblPort->setStyleSheet(labelStyle);
    spinPort = new QSpinBox();
    spinPort->setRange(1024, 65535);
    spinPort->setValue(1234);
    spinPort->setStyleSheet(inputStyle);
    spinPort->setFixedWidth(150);
    rowPort->addWidget(lblPort);
    rowPort->addWidget(spinPort);
    rowPort->addStretch();
    mainLayout->addLayout(rowPort);

    // 2. 硬件与超时
    QHBoxLayout *rowCom = new QHBoxLayout();
    lblCom = new QLabel("报警器COM:");
    lblCom->setStyleSheet(labelStyle);
    comboCom = new QComboBox();
    comboCom->addItem("COM1");
    comboCom->addItem("COM2");
    comboCom->setStyleSheet(comboStyle);
    comboCom->setFixedWidth(150);
    rowCom->addWidget(lblCom);
    rowCom->setSpacing(8);
    rowCom->addWidget(comboCom);
    rowCom->addStretch();
    mainLayout->addLayout(rowCom);

    QHBoxLayout *rowTimeout = new QHBoxLayout();
    lblTimeout = new QLabel("超时报警时间:");
    lblTimeout->setStyleSheet(labelStyle);
    spinTimeout = new QSpinBox();
    spinTimeout->setRange(1, 3600);
    spinTimeout->setValue(128);
    spinTimeout->setSuffix("s"); 
    spinTimeout->setStyleSheet(inputStyle);
    spinTimeout->setFixedWidth(80);
    rowTimeout->addWidget(lblTimeout);
    rowTimeout->setSpacing(8);
    rowTimeout->addWidget(spinTimeout);
    rowTimeout->addStretch();
    mainLayout->addLayout(rowTimeout);

    // 3. 扫码枪列表
    lblScanners = new QLabel("扫码枪列表:");
    lblScanners->setStyleSheet(labelStyle);
    mainLayout->addWidget(lblScanners);

    listScanners = new QListWidget();
    listScanners->addItem("扫码枪A：192.168.101.277");
    listScanners->addItem("扫码枪B：192.168.101.277");
    listScanners->addItem("扫码枪C：192.168.101.277");
    listScanners->setStyleSheet("QListWidget { background-color: transparent; border: 1.2px solid #FFFFFF; border-radius: 8px; color: #FFFFFF; font-size: 14px; padding: 5px; }");
    listScanners->setFixedWidth(300);
    listScanners->setFixedHeight(120);
    
    QHBoxLayout *rowList = new QHBoxLayout();
    rowList->setContentsMargins(10, 0, 0, 0);
    rowList->addWidget(listScanners);
    rowList->addStretch();
    mainLayout->addLayout(rowList);

    // 语言切换 (放在角落或底部)
    QHBoxLayout *rowLang = new QHBoxLayout();
    lblLang = new QLabel("界面语言:");
    lblLang->setStyleSheet(labelStyle);
    comboLang = new QComboBox();
    comboLang->addItem("简体中文");
    comboLang->addItem("English");
    comboLang->setStyleSheet(comboStyle);
    rowLang->addStretch();
    rowLang->addWidget(lblLang);
    rowLang->addWidget(comboLang);
    mainLayout->addLayout(rowLang);

    mainLayout->addStretch();

    // 初始文案
    retranslateUi();

    // 连接信号
    connect(comboLang, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingPage::onLanguageChanged);
            
    connect(spinPort, QOverload<int>::of(&QSpinBox::valueChanged), [=](int p){
        retranslateUi();
        emit portChanged(p);
    });

    connect(listScanners, &QListWidget::itemDoubleClicked, [=](QListWidgetItem *item){
        bool ok;
        QString oldText = item->text();
        QString alias = QInputDialog::getText(this, "重命名扫码枪", "设置别名:", QLineEdit::Normal, oldText, &ok);
        if (ok && !alias.isEmpty()) {
            item->setText(alias);
        }
    });
}

void SettingPage::onLanguageChanged(int index)
{
    retranslateUi();
    emit languageChanged(index); // 发射信号通知 MainWindow
}

void SettingPage::retranslateUi()
{
    bool isChinese = (comboLang->currentIndex() == 0);

    if (isChinese) {
        lblLang->setText("界面语言:");
        lblTitleListen->setText("监听地址:");
        lblIp->setText("IP:");
        lblPort->setText("端口:");
        lblCom->setText("报警器COM:");
        lblTimeout->setText("超时报警时间:");
        lblScanners->setText("扫码枪列表:");
        spinTimeout->setSuffix("s");
    } else {
        lblLang->setText("Language:");
        lblTitleListen->setText("Listen Address:");
        lblIp->setText("IP:");
        lblPort->setText("Port:");
        lblCom->setText("Alarm COM:");
        lblTimeout->setText("Timeout Alarm:");
        lblScanners->setText("Scanner List:");
        spinTimeout->setSuffix("s");
    }
}

QString SettingPage::getLocalIP()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    for (const QHostAddress& address : list) {
        if (!address.isLoopback() && address.protocol() == QAbstractSocket::IPv4Protocol) {
            return address.toString();
        }
    }
    return "127.0.0.1";
}
