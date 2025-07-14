#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "calculator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void DigitClicked();
    void OnCommaClicked();
    void OnNegateClicked();
    void OnBackspaceClicked();
    void OnOperationClicked();
    void OnEqualClicked();
    void OnResetClicked();
    void OnMemorySave();
    void OnMemoryClear();
    void OnMemoryRecall();

private:
    enum class Operation {
        NO_OPERATION,
        ADDITION,
        SUBTRACTION,
        MULTIPLICATION,
        DIVISION,
        POWER
    };

    Ui::MainWindow *ui;

    Calculator calculator_;
    Operation current_operation_;
    QString input_number_;
    double active_number_;
    double memory_;
    bool has_memory_;
    bool clear_formula_on_next_input_ = false;

    void SetText(const QString &text);
    void AddText(const QString &suffix);
    QString NormalizeNumber(const QString &text);
    QString OpToString(Operation op);
    void SetOperation(Operation op);
};

#endif
