#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QGroupBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileInfo>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("ЛР 14 — Вариант 29");
    resize(1000, 700);

    // ========== ГРУППА ВЫБОРА ФАЙЛОВ ==========
    QGroupBox *fileGroup = new QGroupBox("Выбор файлов");

    QLabel *labelInput = new QLabel("Входной файл:");
    lineEditInputPath = new QLineEdit;
    lineEditInputPath->setReadOnly(true);
    lineEditInputPath->setPlaceholderText("Выберите входной текстовый файл...");
    btnSelectInput = new QPushButton("Выбрать...");

    QLabel *labelOutput = new QLabel("Выходной файл:");
    lineEditOutputPath = new QLineEdit;
    lineEditOutputPath->setReadOnly(true);
    lineEditOutputPath->setPlaceholderText("Выберите выходной текстовый файл...");
    btnSelectOutput = new QPushButton("Выбрать...");

    QGridLayout *fileLayout = new QGridLayout;
    fileLayout->addWidget(labelInput, 0, 0);
    fileLayout->addWidget(lineEditInputPath, 0, 1);
    fileLayout->addWidget(btnSelectInput, 0, 2);
    fileLayout->addWidget(labelOutput, 1, 0);
    fileLayout->addWidget(lineEditOutputPath, 1, 1);
    fileLayout->addWidget(btnSelectOutput, 1, 2);
    fileGroup->setLayout(fileLayout);

    // ========== ГРУППА ПРОСМОТРА ФАЙЛОВ ==========
    QGroupBox *viewGroup = new QGroupBox("Содержимое файлов");

    QLabel *labelInputContent = new QLabel("Входной файл:");
    labelInputContent->setStyleSheet("font-weight: bold;");
    listWidgetInput = new QListWidget;
    listWidgetInput->setAlternatingRowColors(true);

    QLabel *labelOutputContent = new QLabel("Выходной файл:");
    labelOutputContent->setStyleSheet("font-weight: bold;");
    listWidgetOutput = new QListWidget;
    listWidgetOutput->setAlternatingRowColors(true);

    QHBoxLayout *viewLayout = new QHBoxLayout;
    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(labelInputContent);
    leftLayout->addWidget(listWidgetInput);
    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(labelOutputContent);
    rightLayout->addWidget(listWidgetOutput);
    viewLayout->addLayout(leftLayout);
    viewLayout->addLayout(rightLayout);
    viewGroup->setLayout(viewLayout);

    // ========== ИНФОРМАЦИЯ О МАКСИМАЛЬНЫХ ВСТАВКАХ ==========
    labelMaxInsertions = new QLabel("Максимальное количество вставок в строке: 0");
    labelMaxInsertions->setAlignment(Qt::AlignCenter);
    labelMaxInsertions->setStyleSheet("QLabel { font-size: 14px; font-weight: bold; color: #2c3e50; padding: 10px; background-color: #ecf0f1; border-radius: 5px; }");

    // ========== КНОПКИ УПРАВЛЕНИЯ ==========
    btnProcess = new QPushButton("Обработать файлы");
    btnProcess->setEnabled(false);
    btnClear = new QPushButton("Очистить");
    btnExit = new QPushButton("Выход");

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(btnProcess);
    btnLayout->addWidget(btnClear);
    btnLayout->addWidget(btnExit);
    btnLayout->addStretch();

    // ========== СТАТУСНАЯ СТРОКА ==========
    labelStatus = new QLabel("Выберите входной и выходной файлы");
    labelStatus->setAlignment(Qt::AlignCenter);
    labelStatus->setMinimumHeight(40);
    labelStatus->setStyleSheet("QLabel { padding: 10px; border-radius: 5px; background-color: #ecf0f1; color: #34495e; }");

    // ========== ОСНОВНОЙ МАКЕТ ==========
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(fileGroup);
    mainLayout->addWidget(viewGroup);
    mainLayout->addWidget(labelMaxInsertions);
    mainLayout->addLayout(btnLayout);
    mainLayout->addWidget(labelStatus);

    QWidget *central = new QWidget;
    central->setLayout(mainLayout);
    setCentralWidget(central);

    // ========== ПОДКЛЮЧЕНИЕ СИГНАЛОВ ==========
    connect(btnSelectInput, &QPushButton::clicked, this, &MainWindow::onSelectInputFile);
    connect(btnSelectOutput, &QPushButton::clicked, this, &MainWindow::onSelectOutputFile);
    connect(btnProcess, &QPushButton::clicked, this, &MainWindow::onProcess);
    connect(btnClear, &QPushButton::clicked, this, &MainWindow::onClear);
    connect(btnExit, &QPushButton::clicked, this, &MainWindow::onExit);
}

MainWindow::~MainWindow() {}

// ========== ОБРАБОТКА ОДНОГО СЛОВА ==========
QString MainWindow::processWord(const QString& word, int& insertCount) {
    int len = word.length();
    if (len % 2 == 0 && len > 0) {
        int mid = len / 2;
        insertCount++;
        return word.left(mid) + "К" + word.mid(mid);
    }
    return word;
}

// ========== ОБРАБОТКА ОДНОЙ СТРОКИ ==========
int MainWindow::processLine(const QString& line, QString& resultLine) {
    QStringList words = line.split(' ', Qt::SkipEmptyParts);
    QStringList resultWords;
    int lineInsertCount = 0;

    for (const QString& word : words) {
        int wordInsertCount = 0;
        QString newWord = processWord(word, wordInsertCount);
        resultWords.append(newWord);
        lineInsertCount += wordInsertCount;
    }

    resultLine = resultWords.join(" ");
    return lineInsertCount;
}

// ========== ВЫБОР ВХОДНОГО ФАЙЛА ==========
void MainWindow::onSelectInputFile() {
    QString fileName = QFileDialog::getOpenFileName(this,
        "Выберите входной текстовый файл",
        QDir::homePath(),
        "Текстовые файлы (*.txt);;Все файлы (*.*)");

    if (!fileName.isEmpty()) {
        inputFilePath = fileName;
        lineEditInputPath->setText(inputFilePath);
        updateStatus("Входной файл выбран: " + QFileInfo(fileName).fileName());
    } else {
        inputFilePath = "";
        lineEditInputPath->clear();
        updateStatus("Выбор входного файла отменён");
    }
    setProcessButtonState();
}

// ========== ВЫБОР ВЫХОДНОГО ФАЙЛА ==========
void MainWindow::onSelectOutputFile() {
    QString fileName = QFileDialog::getSaveFileName(this,
        "Выберите выходной текстовый файл",
        QDir::homePath() + "/output.txt",
        "Текстовые файлы (*.txt);;Все файлы (*.*)");

    if (!fileName.isEmpty()) {
        if (!fileName.endsWith(".txt", Qt::CaseInsensitive)) {
            fileName += ".txt";
        }
        outputFilePath = fileName;
        lineEditOutputPath->setText(outputFilePath);
        updateStatus("Выходной файл выбран: " + QFileInfo(fileName).fileName());
    } else {
        outputFilePath = "";
        lineEditOutputPath->clear();
        updateStatus("Выбор выходного файла отменён");
    }
    setProcessButtonState();
}

// ========== ОСНОВНАЯ ОБРАБОТКА ==========
void MainWindow::onProcess() {
    listWidgetInput->clear();
    listWidgetOutput->clear();

    if (!QFile::exists(inputFilePath)) {
        updateStatus("Ошибка: входной файл не существует!", true);
        return;
    }

    try {
        QFile inputFile(inputFilePath);
        if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            updateStatus("Ошибка: не удалось открыть входной файл!", true);
            return;
        }

        QFile outputFile(outputFilePath);
        if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            updateStatus("Ошибка: не удалось открыть выходной файл!", true);
            inputFile.close();
            return;
        }

        QTextStream in(&inputFile);
        QTextStream out(&outputFile);

        in.setEncoding(QStringConverter::Utf8);
        out.setEncoding(QStringConverter::Utf8);

        int maxInsertions = 0;
        QString lineWithMaxInsertions;
        int lineNumber = 0;

        while (!in.atEnd()) {
            QString originalLine = in.readLine();
            listWidgetInput->addItem(originalLine);

            QString processedLine;
            int insertCount = processLine(originalLine, processedLine);

            out << processedLine << Qt::endl;
            listWidgetOutput->addItem(processedLine);

            if (insertCount > maxInsertions) {
                maxInsertions = insertCount;
                lineWithMaxInsertions = originalLine;
                lineNumber = listWidgetInput->count();
            }
        }

        inputFile.close();
        outputFile.close();

        labelMaxInsertions->setText(QString("Максимальное количество вставок в строке: %1 (строка %2)")
            .arg(maxInsertions).arg(lineNumber));

        updateStatus(QString("Обработка завершена. Всего строк: %1, максимум вставок: %2")
            .arg(listWidgetInput->count()).arg(maxInsertions));

    } catch (const std::exception& e) {
        updateStatus(QString("Ошибка: %1").arg(e.what()), true);
    } catch (...) {
        updateStatus("Неизвестная ошибка!", true);
    }
}

// ========== ОЧИСТКА ==========
void MainWindow::onClear() {
    inputFilePath = "";
    outputFilePath = "";
    lineEditInputPath->clear();
    lineEditOutputPath->clear();
    listWidgetInput->clear();
    listWidgetOutput->clear();
    labelMaxInsertions->setText("Максимальное количество вставок в строке: 0");
    btnProcess->setEnabled(false);
    updateStatus("Поля очищены. Выберите входной и выходной файлы");
}

// ========== ВЫХОД ==========
void MainWindow::onExit() {
    close();
}

// ========== ОБНОВЛЕНИЕ СТАТУСА ==========
void MainWindow::updateStatus(const QString& message, bool isError) {
    labelStatus->setText(message);
    if (isError) {
        labelStatus->setStyleSheet("QLabel { background-color: #f8d7da; color: #721c24; padding: 10px; border-radius: 5px; }");
    } else {
        labelStatus->setStyleSheet("QLabel { background-color: #d4edda; color: #155724; padding: 10px; border-radius: 5px; }");
    }
}

// ========== УПРАВЛЕНИЕ КНОПКОЙ ОБРАБОТКИ ==========
void MainWindow::setProcessButtonState() {
    bool enabled = !inputFilePath.isEmpty() && !outputFilePath.isEmpty();
    btnProcess->setEnabled(enabled);
    if (enabled) {
        btnProcess->setToolTip("Начать обработку файлов");
    } else {
        btnProcess->setToolTip("Сначала выберите входной и выходной файлы");
    }
}
