#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QStringList>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSelectInputFile();
    void onSelectOutputFile();
    void onProcess();
    void onClear();
    void onExit();

private:
    void updateStatus(const QString& message, bool isError = false);
    void setProcessButtonState();
    int processLine(const QString& line, QString& resultLine);
    QString processWord(const QString& word, int& insertCount);

    QString inputFilePath;
    QString outputFilePath;

    QLineEdit *lineEditInputPath;
    QLineEdit *lineEditOutputPath;
    QListWidget *listWidgetInput;
    QListWidget *listWidgetOutput;
    QLabel *labelMaxInsertions;
    QLabel *labelStatus;
    QPushButton *btnProcess;
    QPushButton *btnClear;
    QPushButton *btnExit;
    QPushButton *btnSelectInput;
    QPushButton *btnSelectOutput;
};

#endif
