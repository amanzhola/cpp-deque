#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cmath>
#include <limits>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
    current_operation_(Operation::NO_OPERATION),
    active_number_(0.0), memory_(0.0), has_memory_(false)
{
    ui->setupUi(this);

    QList<QPushButton*> digits = {ui->pb_zero, ui->pb_one, ui->pb_two, ui->pb_three, ui->pb_four,
                                   ui->pb_five, ui->pb_six, ui->pb_seven, ui->pb_eight, ui->pb_nine};

    for (int i = 0; i < digits.size(); ++i) {
        connect(digits[i], &QPushButton::clicked, this, &MainWindow::DigitClicked);
    }

    connect(ui->pb_comma, &QPushButton::clicked, this, &MainWindow::OnCommaClicked);
    connect(ui->pb_negate, &QPushButton::clicked, this, &MainWindow::OnNegateClicked);
    connect(ui->pb_backspace, &QPushButton::clicked, this, &MainWindow::OnBackspaceClicked);
    connect(ui->pb_add, &QPushButton::clicked, this, &MainWindow::OnOperationClicked);
    connect(ui->pb_subtract, &QPushButton::clicked, this, &MainWindow::OnOperationClicked);
    connect(ui->pb_multiplicate, &QPushButton::clicked, this, &MainWindow::OnOperationClicked);
    connect(ui->pb_divide, &QPushButton::clicked, this, &MainWindow::OnOperationClicked);
    connect(ui->pb_power, &QPushButton::clicked, this, &MainWindow::OnOperationClicked);
    connect(ui->pb_equal, &QPushButton::clicked, this, &MainWindow::OnEqualClicked);
    connect(ui->pb_reset, &QPushButton::clicked, this, &MainWindow::OnResetClicked);

    connect(ui->pb_ms, &QPushButton::clicked, this, &MainWindow::OnMemorySave);
    connect(ui->pb_mc, &QPushButton::clicked, this, &MainWindow::OnMemoryClear);
    connect(ui->pb_mr, &QPushButton::clicked, this, &MainWindow::OnMemoryRecall);

    SetText("0");
    ui->l_formula->setText("");
    ui->l_memory->setText("");
}

MainWindow::~MainWindow() {
    delete ui;
}

QString MainWindow::NormalizeNumber(const QString &text) {
    if (text.isEmpty() || text == "-") return "0";

    if (text == "nan" || text == "inf" || text == "-inf") return text;

    QString res = text;
    if (res.startsWith('.')) res = "0" + res;
    if (res.startsWith("-.")) res = "-0" + res.mid(1);

    QRegularExpression re("[1-9]");

    if (res.startsWith("0") && !res.startsWith("0.") && res.size() > 1) {
        QRegularExpressionMatch match = re.match(res);
        if (match.hasMatch()) {
            int idx = match.capturedStart();
            res.remove(0, idx);
        } else {
            res = "0";
        }
    }

    if (res.startsWith("-0") && !res.startsWith("-0.") && res.size() > 2) {
        QRegularExpressionMatch match = re.match(res.mid(1));
        if (match.hasMatch()) {
            int idx = match.capturedStart();
            res = "-" + res.mid(idx + 1);
        } else {
            res = "0";
        }
    }

    return res;
}

void MainWindow::SetText(const QString &text) {
    if (text == "nan" || text == "inf" || text == "-inf") {
        input_number_ = text;

        if (text == "nan") active_number_ = std::numeric_limits<double>::quiet_NaN();
        else if (text == "inf") active_number_ = std::numeric_limits<double>::infinity();
        else if (text == "-inf") active_number_ = -std::numeric_limits<double>::infinity();

        ui->l_result->setText(text);
        return;
    }

    input_number_ = NormalizeNumber(text);
    active_number_ = input_number_.toDouble();
    ui->l_result->setText(input_number_);
}

void MainWindow::AddText(const QString &suffix) {
    SetText(input_number_ + suffix);
}

void MainWindow::DigitClicked() {
    QPushButton *button = qobject_cast<QPushButton*>(sender());

    if (clear_formula_on_next_input_) {
        ui->l_formula->setText("");
        clear_formula_on_next_input_ = false;
    }

    if (input_number_ == "nan" || input_number_ == "inf" || input_number_ == "-inf") {
        SetText(button->text());
        return;
    }

    AddText(button->text());
}

void MainWindow::OnCommaClicked() {
    if (input_number_ == "nan" || input_number_ == "inf" || input_number_ == "-inf") return;

    if (!input_number_.contains('.')) {
        AddText(".");
    }
}

void MainWindow::OnNegateClicked() {
    if (input_number_ == "nan" || input_number_ == "inf" || input_number_ == "-inf") {
        return;
    }

    if (input_number_.startsWith('-')) {
        SetText(input_number_.mid(1));
    } else {
        SetText("-" + input_number_);
    }
}

void MainWindow::OnBackspaceClicked() {
    if (input_number_ == "nan" || input_number_ == "inf" || input_number_ == "-inf") return;

    if (!input_number_.isEmpty()) {
        QString temp = input_number_;
        temp.chop(1);
        SetText(temp);
    }
    clear_formula_on_next_input_ = true;
}

QString MainWindow::OpToString(Operation op) {
    switch(op) {
    case Operation::ADDITION: return "+";
    case Operation::SUBTRACTION: return "−";
    case Operation::MULTIPLICATION: return "×";
    case Operation::DIVISION: return "÷";
    case Operation::POWER: return "^";
    default: return "";
    }
}

void MainWindow::SetOperation(Operation op) {
    if (current_operation_ == Operation::NO_OPERATION) {
        calculator_.Set(active_number_);
    }
    current_operation_ = op;
    ui->l_formula->setText(QString("%1 %2").arg(QString::number(calculator_.GetNumber()))
                               .arg(OpToString(op)));
    input_number_.clear();

    clear_formula_on_next_input_ = false;
}

void MainWindow::OnOperationClicked() {
    QPushButton *button = qobject_cast<QPushButton*>(sender());

    if (button == ui->pb_add) SetOperation(Operation::ADDITION);
    else if (button == ui->pb_subtract) SetOperation(Operation::SUBTRACTION);
    else if (button == ui->pb_multiplicate) SetOperation(Operation::MULTIPLICATION);
    else if (button == ui->pb_divide) SetOperation(Operation::DIVISION);
    else if (button == ui->pb_power) SetOperation(Operation::POWER);
}

void MainWindow::OnEqualClicked() {
    if (current_operation_ == Operation::NO_OPERATION) return;

    QString formula = QString("%1 %2 %3 =").arg(QString::number(calculator_.GetNumber()))
                          .arg(OpToString(current_operation_))
                          .arg(QString::number(active_number_));
    ui->l_formula->setText(formula);

    switch(current_operation_) {
    case Operation::ADDITION: calculator_.Add(active_number_); break;
    case Operation::SUBTRACTION: calculator_.Sub(active_number_); break;
    case Operation::MULTIPLICATION: calculator_.Mul(active_number_); break;
    case Operation::DIVISION: calculator_.Div(active_number_); break;
    case Operation::POWER: calculator_.Pow(active_number_); break;
    default: break;
    }

    active_number_ = calculator_.GetNumber();

    if (std::isnan(active_number_)) {
        ui->l_result->setText("nan");
        input_number_ = "nan";
    } else if (std::isinf(active_number_)) {
        ui->l_result->setText(active_number_ > 0 ? "inf" : "-inf");
        input_number_ = active_number_ > 0 ? "inf" : "-inf";
    } else {
        ui->l_result->setText(QString::number(active_number_));
        input_number_.clear();
    }

    current_operation_ = Operation::NO_OPERATION;
}

void MainWindow::OnResetClicked() {
    current_operation_ = Operation::NO_OPERATION;
    ui->l_formula->clear();
    SetText("0");
}

void MainWindow::OnMemorySave() {
    memory_ = active_number_;
    has_memory_ = true;
    ui->l_memory->setText("M");
}

void MainWindow::OnMemoryClear() {
    has_memory_ = false;
    ui->l_memory->clear();
}

void MainWindow::OnMemoryRecall() {
    if (!has_memory_) return;

    active_number_ = memory_;
    input_number_ = QString::number(active_number_);
    ui->l_result->setText(input_number_);
}
