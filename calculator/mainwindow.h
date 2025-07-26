#pragma once

#include <QMainWindow>
#include <functional>
#include <optional>
#include <string>
#include "enums.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    // Методы для изменения UI
    void SetInputText(const std::string& text);
    void SetErrorText(const std::string& text);
    void SetFormulaText(const std::string& text);
    void SetMemText(const std::string& text);
    void SetExtraKey(const std::optional<std::string>& key);
    
    // Методы для установки колбэков
    void SetDigitKeyCallback(std::function<void(int)> cb);
    void SetProcessOperationKeyCallback(std::function<void(Operation)> cb);
    void SetProcessControlKeyCallback(std::function<void(ControlKey)> cb);
    void SetControllerCallback(std::function<void(ControllerType)> cb);
    
private slots:
    void OnDigitClicked();
    void OnOperationClicked();
    void OnControlKeyClicked();
    void OnExtraKeyClicked();
    void OnControllerChanged(int index);
    
private:
    Ui::MainWindow *ui;
    
    std::function<void(int)> digit_callback_;
    std::function<void(Operation)> operation_callback_;
    std::function<void(ControlKey)> control_callback_;
    std::function<void(ControllerType)> controller_callback_;
    
    void ConnectSignals();
    void SetResultColorRed();
    void ResetResultColor();
    
    // Новые методы для форматирования
    QString FormatNumberText(const QString& text);
    QString FormatFormulaText(const QString& text);
};