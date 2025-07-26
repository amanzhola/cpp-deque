#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QString>
#include <QPushButton>
#include <QComboBox>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    InitControllerComboBox();
    ConnectSignals();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::InitControllerComboBox() {
    ui->cmb_controller->clear();
    ui->cmb_controller->addItems({
        "double", "float", "uint8_t", "int", "int64_t", "size_t", "Rational"
    });
    ui->cmb_controller->setCurrentIndex(0);
    OnControllerChanged(0);

    connect(ui->cmb_controller, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::OnControllerChanged);
}

QString MainWindow::FormatFormulaText(const QString& text) {
    QString result = text;

    result.replace(QRegularExpression("\\s*\\+\\s*"), " + ");
    result.replace(QRegularExpression("\\s*-\\s*"), " - ");
    result.replace(QRegularExpression("\\s*×\\s*"), " × ");
    result.replace(QRegularExpression("\\s*÷\\s*"), " ÷ ");
    result.replace(QRegularExpression("\\s*\\^\\s*-\\s*"), " ^ -");
    result.replace(QRegularExpression("\\s*\\^\\s*"), " ^ ");
    result.replace(QRegularExpression("(÷|×)\\s+-\\s*(\\d+)"), "\\1 -\\2");
    result.replace(QRegularExpression("(^|\\s)[-]\\s+(\\d+)"), "\\1-\\2");

    return result.simplified();
}

QString MainWindow::FormatNumberText(const QString& text) {
    bool okInt = false, okUInt = false, okDouble = false;

    const qint64 valInt = text.toLongLong(&okInt);
    if (okInt) {
        return QString::number(valInt);
    }

    const quint64 valUInt = text.toULongLong(&okUInt);
    if (okUInt) {
        return QString::number(valUInt);
    }

    const double valDouble = text.toDouble(&okDouble);
    if (!okDouble) {
        return text;
    }

    if (qAbs(valDouble) >= 1e9 || (qAbs(valDouble) > 0 && qAbs(valDouble) < 1e-4)) {
        QString s = QString::number(valDouble, 'e', 6);
        s.replace(QRegularExpression("\\.?0+(e)"), "e");
        return s;
    }

    QString formatted = QString::number(valDouble, 'f', 10);
    formatted = formatted.remove(QRegularExpression("0+$"));
    if (formatted.endsWith('.')) {
        formatted.chop(1);
    }

    return formatted;
}

// --- Методы отображения ---

void MainWindow::SetInputText(const std::string& text) {
    ResetResultColor();
    ui->l_result->setText(FormatNumberText(QString::fromStdString(text)));
}

void MainWindow::SetErrorText(const std::string& text) {
    SetResultColorRed();
    ui->l_result->setText(QString::fromStdString(text));
}

void MainWindow::SetFormulaText(const std::string& text) {
    ui->l_formula->setText(FormatFormulaText(QString::fromStdString(text)));
}

void MainWindow::SetMemText(const std::string& text) {
    ui->l_memory->setText(QString::fromStdString(text));
}

void MainWindow::SetExtraKey(const std::optional<std::string>& key) {
    if (key.has_value()) {
        ui->tb_extra->setText(QString::fromStdString(key.value()));
        ui->tb_extra->show();
    } else {
        ui->tb_extra->hide();
    }
}

// --- Колбэки ---

void MainWindow::SetDigitKeyCallback(std::function<void(int)> cb) {
    digit_callback_ = std::move(cb);
}

void MainWindow::SetProcessOperationKeyCallback(std::function<void(Operation)> cb) {
    operation_callback_ = std::move(cb);
}

void MainWindow::SetProcessControlKeyCallback(std::function<void(ControlKey)> cb) {
    control_callback_ = std::move(cb);
}

void MainWindow::SetControllerCallback(std::function<void(ControllerType)> cb) {
    controller_callback_ = std::move(cb);
}

// --- Обработчики сигналов ---

void MainWindow::OnDigitClicked() {
    if (!digit_callback_) {
        return;
    }

    const auto* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) {
        return;
    }

    bool ok = false;
    const int digit = btn->text().toInt(&ok);
    if (ok) {
        digit_callback_(digit);
    }
}

void MainWindow::OnOperationClicked() {
    if (!operation_callback_) {
        return;
    }

    const auto* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) {
        return;
    }

    if (btn == ui->pb_add) {
        operation_callback_(Operation::ADDITION);
    } else if (btn == ui->pb_subtract) {
        operation_callback_(Operation::SUBTRACTION);
    } else if (btn == ui->pb_multiplicate) {
        operation_callback_(Operation::MULTIPLICATION);
    } else if (btn == ui->pb_divide) {
        operation_callback_(Operation::DIVISION);
    } else if (btn == ui->pb_power) {
        operation_callback_(Operation::POWER);
    }
}

void MainWindow::OnControlKeyClicked() {
    if (!control_callback_) {
        return;
    }

    QObject* src = sender();

    if (src == ui->pb_equal) {
        control_callback_(ControlKey::EQUALS);
    } else if (src == ui->pb_reset) {
        control_callback_(ControlKey::CLEAR);
    } else if (src == ui->pb_ms) {
        control_callback_(ControlKey::MEM_SAVE);
    } else if (src == ui->pb_mr) {
        control_callback_(ControlKey::MEM_LOAD);
    } else if (src == ui->pb_mc) {
        control_callback_(ControlKey::MEM_CLEAR);
    } else if (src == ui->pb_negate) {
        control_callback_(ControlKey::PLUS_MINUS);
    } else if (src == ui->pb_backspace) {
        control_callback_(ControlKey::BACKSPACE);
    }
}

void MainWindow::OnExtraKeyClicked() {
    if (!control_callback_) {
        return;
    }

    control_callback_(ControlKey::EXTRA_KEY);
}

void MainWindow::OnControllerChanged(int index) {
    if (!controller_callback_) {
        return;
    }

    const ControllerType type = static_cast<ControllerType>(index);
    controller_callback_(type);

    const bool show_extra =
        type == ControllerType::DOUBLE ||
        type == ControllerType::FLOAT ||
        type == ControllerType::RATIONAL;

    ui->tb_extra->setVisible(show_extra);
}

// --- Вспомогательные методы ---

void MainWindow::ConnectSignals() {
    const QList<QPushButton*> digit_buttons = {
        ui->pb_zero, ui->pb_one, ui->pb_two, ui->pb_three, ui->pb_four,
        ui->pb_five, ui->pb_six, ui->pb_seven, ui->pb_eight, ui->pb_nine
    };

    for (QPushButton* btn : digit_buttons) {
        connect(btn, &QPushButton::clicked, this, &MainWindow::OnDigitClicked);
    }

    connect(ui->pb_add, &QPushButton::clicked, this, &MainWindow::OnOperationClicked);
    connect(ui->pb_subtract, &QPushButton::clicked, this, &MainWindow::OnOperationClicked);
    connect(ui->pb_multiplicate, &QPushButton::clicked, this, &MainWindow::OnOperationClicked);
    connect(ui->pb_divide, &QPushButton::clicked, this, &MainWindow::OnOperationClicked);
    connect(ui->pb_power, &QPushButton::clicked, this, &MainWindow::OnOperationClicked);

    connect(ui->pb_equal, &QPushButton::clicked, this, &MainWindow::OnControlKeyClicked);
    connect(ui->pb_reset, &QPushButton::clicked, this, &MainWindow::OnControlKeyClicked);
    connect(ui->pb_ms, &QPushButton::clicked, this, &MainWindow::OnControlKeyClicked);
    connect(ui->pb_mr, &QPushButton::clicked, this, &MainWindow::OnControlKeyClicked);
    connect(ui->pb_mc, &QPushButton::clicked, this, &MainWindow::OnControlKeyClicked);
    connect(ui->pb_negate, &QPushButton::clicked, this, &MainWindow::OnControlKeyClicked);
    connect(ui->pb_backspace, &QPushButton::clicked, this, &MainWindow::OnControlKeyClicked);

    connect(ui->tb_extra, &QPushButton::clicked, this, &MainWindow::OnExtraKeyClicked);
}

void MainWindow::SetResultColorRed() {
    ui->l_result->setStyleSheet("color: red;");
}

void MainWindow::ResetResultColor() {
    ui->l_result->setStyleSheet("");
}