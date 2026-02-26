#ifndef SETTINGPAGE_H
#define SETTINGPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QListWidget>
#include <QLabel>

class SettingPage : public QWidget
{
    Q_OBJECT
public:
    explicit SettingPage(QWidget *parent = nullptr);
    int getLanguageIndex() const;
    int getScannerPort() const;
    int getTimeout() const; // 新增：安全读取超时
    QString getScannerAlias(const QString &ip); // 新增：安全查询别名
    QString getLocalIP();

signals:
    void languageChanged(int index);
    void portChanged(int port); // 新增：端口变更信号

private slots:
    void onLanguageChanged(int index);

private:
    void setupUI();
    void retranslateUi();

    // 控件指针，用于动态翻译
    QLabel *lblLang;
    QLabel *lblIp;
    QLabel *lblPort;    
    QLabel *lblTitleListen; // 新增：监听地址标题
    QLabel *lblCom;
    QLabel *lblTimeout;
    QLabel *lblScanners;

    QLineEdit *editIp;
    QSpinBox *spinPort;  // 新增：端口输入框
    QComboBox *comboCom;
    QComboBox *comboLang;
    QSpinBox *spinTimeout;
    QListWidget *listScanners;

    // 声明 MainWindow 为友元或保持封装，通过上述 Getter 访问
    friend class MainWindow; 
};

#endif // SETTINGPAGE_H
