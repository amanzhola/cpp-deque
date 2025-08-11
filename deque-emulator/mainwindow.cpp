#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <algorithm>
#include <random>

namespace {
    static constexpr int kMaxDequeSize = 1000;

    template <class ModelT>
    inline void moveCursorToBeginOrEnd(ModelT& model) {
        if (model.items.empty()) {
            model.toEnd();
        } else {
            model.toBegin();
        }
    }
}

const std::deque<QString> MainWindow::tea = {
    "Чай Лунцзин", "Эрл Грей", "Сенча", "Пуэр", "Дарджилинг",
    "Ассам", "Матча", "Ганпаудер", "Оолонг", "Лапсанг Сушонг"
};

const std::deque<QString> MainWindow::cakes = {
    "Красный бархат", "Наполеон", "Медовик", "Тирамису", "Прага",
    "Чизкейк", "Захер", "Эстерхази", "Морковный торт", "Чёрный лес"
};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow) {
    ui->setupUi(this);
    applyModel();
}

MainWindow::~MainWindow() {
    delete ui;
}

// Текущий компаратор по режиму сортировки
MainWindow::Cmp MainWindow::currentComp() const {
    static auto caseSensitiveLess = [](const QString& a, const QString& b) {
        return a < b; // лексикографически, case-sensitive
    };

    static auto caseInsensitiveLess = [](const QString& a, const QString& b) {
        return a.compare(b, Qt::CaseInsensitive) < 0;
    };

    switch (sortMode_) {
        case SortMode::CaseInsensitive: {
            return caseInsensitiveLess;
        }
        case SortMode::CaseSensitive: {
            return caseSensitiveLess;
        }
        case SortMode::None: {
            return caseSensitiveLess; // дефолт
        }
    }
    return caseSensitiveLess;
}

void MainWindow::applyModel() {
    QSignalBlocker blocker(ui->list_widget);
    ui->list_widget->setUpdatesEnabled(false);
    ui->list_widget->clear();

    for (int i = 0; i < static_cast<int>(deque_model_.items.size()); ++i) {
        ui->list_widget->addItem(QString("%1: %2").arg(i).arg(deque_model_.items[i]));
    }
    ui->list_widget->addItem("end");

    ui->txt_size->setText(QString::number(deque_model_.items.size()));

    deque_model_.clamp();

    const int currentRowIndex = deque_model_.atEnd()
        ? static_cast<int>(deque_model_.items.size())
        : static_cast<int>(deque_model_.pos);

    ui->list_widget->setCurrentRow(currentRowIndex);

    ui->list_widget->setUpdatesEnabled(true);
    updateControls();
    applyIterator();
}

void MainWindow::applyIterator() {
    const bool atEnd = deque_model_.atEnd();

    ui->txt_elem_content->setText(atEnd ? "" : deque_model_.items[deque_model_.pos]);

    // prev активна везде, кроме начала (и когда пусто)
    const bool isEmpty  = deque_model_.items.empty();
    const bool atBegin  = !atEnd && (deque_model_.pos == 0);
    ui->btn_prev->setEnabled(!isEmpty && !atBegin);

    // next активна, пока не на end
    ui->btn_next->setEnabled(!atEnd);

    ui->btn_edit->setEnabled(!atEnd);
    ui->btn_erase->setEnabled(!atEnd);

    // синхронизация выделения
    const int currentRowIndex = atEnd
        ? static_cast<int>(deque_model_.items.size())
        : static_cast<int>(deque_model_.pos);

    if (ui->list_widget->currentRow() != currentRowIndex) {
        ui->list_widget->setCurrentRow(currentRowIndex);
    }
}

void MainWindow::updateControls() {
    const bool isEmpty = deque_model_.items.empty();

    ui->btn_pop_front->setEnabled(!isEmpty);
    ui->btn_pop_back->setEnabled(!isEmpty);
    ui->btn_erase->setEnabled(!isEmpty && !deque_model_.atEnd());

    const bool isSorted = (sortMode_ != SortMode::None) && !isEmpty;
    ui->btn_lower_bound->setEnabled(isSorted);
    ui->btn_upper_bound->setEnabled(isSorted);
    ui->btn_unique->setEnabled(isSorted);
}

// === Базовые операции ===

void MainWindow::on_btn_push_front_clicked() {
    const QString text = ui->txt_elem_content->text().trimmed();
    if (text.isEmpty()) {
        return;
    }

    deque_model_.items.push_front(text);
    sortMode_ = SortMode::None;
    moveCursorToBeginOrEnd(deque_model_);
    applyModel();
}

void MainWindow::on_btn_push_back_clicked() {
    const QString text = ui->txt_elem_content->text().trimmed();
    if (text.isEmpty()) {
        return;
    }

    deque_model_.items.push_back(text);
    sortMode_ = SortMode::None;
    moveCursorToBeginOrEnd(deque_model_);
    applyModel();
}

void MainWindow::on_btn_pop_front_clicked() {
    if (deque_model_.items.empty()) {
        return;
    }

    deque_model_.items.pop_front();
    sortMode_ = SortMode::None;
    moveCursorToBeginOrEnd(deque_model_);
    applyModel();
}

void MainWindow::on_btn_pop_back_clicked() {
    if (deque_model_.items.empty()) {
        return;
    }

    deque_model_.items.pop_back();
    sortMode_ = SortMode::None;
    moveCursorToBeginOrEnd(deque_model_);
    applyModel();
}

void MainWindow::on_btn_clear_clicked() {
    deque_model_.items.clear();
    sortMode_ = SortMode::None;
    deque_model_.toEnd();
    applyModel();
}

void MainWindow::on_btn_insert_clicked() {
    const QString text = ui->txt_elem_content->text().trimmed();
    if (text.isEmpty()) {
        return;
    }

    const size_t insertIndex = deque_model_.atEnd()
        ? deque_model_.items.size()
        : deque_model_.pos;

    auto it = deque_model_.items.begin();
    std::advance(it, static_cast<long>(insertIndex));
    deque_model_.items.insert(it, text);

    sortMode_ = SortMode::None;
    moveCursorToBeginOrEnd(deque_model_);
    applyModel();
}

void MainWindow::on_btn_erase_clicked() {
    if (deque_model_.atEnd()) {
        return;
    }

    auto it = deque_model_.items.begin();
    std::advance(it, static_cast<long>(deque_model_.pos));
    deque_model_.items.erase(it);

    sortMode_ = SortMode::None;
    moveCursorToBeginOrEnd(deque_model_);
    applyModel();
}

void MainWindow::on_btn_edit_clicked() {
    if (deque_model_.atEnd()) {
        return;
    }

    const QString text = ui->txt_elem_content->text().trimmed();
    if (text.isEmpty()) {
        return;
    }

    // правим текущее значение, позицию НЕ трогаем
    deque_model_.items[deque_model_.pos] = text;
    sortMode_ = SortMode::None;
    applyModel();
}

// === Навигация ===

void MainWindow::on_btn_prev_clicked() {
    if (deque_model_.items.empty()) {
        return;
    }

    if (!deque_model_.atEnd() && deque_model_.pos > 0) {
        --deque_model_.pos;
    } else if (deque_model_.atEnd()) {
        // с end шагаем на последний элемент
        deque_model_.pos = deque_model_.items.size() - 1;
    }

    applyIterator();
}

void MainWindow::on_btn_next_clicked() {
    if (!deque_model_.atEnd() && deque_model_.pos + 1 <= deque_model_.items.size()) {
        ++deque_model_.pos; // может стать end()
        applyIterator();
    }
}

void MainWindow::on_btn_begin_clicked() {
    deque_model_.toBegin();
    applyIterator();
}

void MainWindow::on_btn_end_clicked() {
    if (deque_model_.items.empty()) {
        deque_model_.toEnd();
    } else {
        // шагнуть к фиктивному end
        deque_model_.pos = deque_model_.items.size();
    }
    applyIterator();
}

void MainWindow::on_list_widget_currentRowChanged(int currentRow) {
    currentRow = std::clamp(currentRow, 0, static_cast<int>(deque_model_.items.size()));
    deque_model_.pos = static_cast<size_t>(currentRow);
    applyIterator();
}

// === Изменение размера и перестановки ===

void MainWindow::on_btn_resize_clicked() {
    bool ok = false;
    const int newSizeInt = ui->txt_size->text().toInt(&ok);
    if (!ok || newSizeInt < 0 || newSizeInt > kMaxDequeSize) {
        return;
    }

    const size_t newSize = static_cast<size_t>(newSizeInt);
    deque_model_.items.resize(newSize);
    sortMode_ = SortMode::None;

    // после resize курсор на начало (или end, если пусто)
    if (newSize == 0) {
        deque_model_.toEnd();
    } else {
        deque_model_.toBegin();
    }

    applyModel();
}

void MainWindow::on_btn_reverse_clicked() {
    if (deque_model_.items.empty()) {
        return;
    }

    // сохраняем индекс
    const size_t savedIndex = deque_model_.atEnd()
        ? deque_model_.items.size()
        : deque_model_.pos;

    std::reverse(deque_model_.items.begin(), deque_model_.items.end());
    sortMode_ = SortMode::None;

    deque_model_.pos = std::min(savedIndex, deque_model_.items.size());
    applyModel();
}

void MainWindow::on_btn_shuffle_clicked() {
    if (deque_model_.items.empty()) {
        return;
    }

    // сохраняем индекс
    const size_t savedIndex = deque_model_.atEnd()
        ? deque_model_.items.size()
        : deque_model_.pos;

    std::shuffle(deque_model_.items.begin(), deque_model_.items.end(), random_gen_);
    sortMode_ = SortMode::None;

    deque_model_.pos = std::min(savedIndex, deque_model_.items.size());
    applyModel();
}

// === Поиск/подсчёт/мин-макс ===

void MainWindow::on_btn_find_clicked() {
    const QString key = ui->txt_elem_content->text().trimmed();
    if (key.isEmpty()) {
        return;
    }

    auto it = std::find(deque_model_.items.begin(), deque_model_.items.end(), key);
    if (it == deque_model_.items.end()) {
        // не найдено — курсор на end
        deque_model_.toEnd();
    } else {
        deque_model_.pos = static_cast<size_t>(std::distance(deque_model_.items.begin(), it));
    }

    applyModel();
}

void MainWindow::on_btn_count_clicked() {
    const QString key = ui->le_count->text().trimmed();
    if (key.isEmpty()) {
        ui->lbl_count->setText("0");
        return;
    }

    const int countValue = static_cast<int>(
        std::count(deque_model_.items.begin(), deque_model_.items.end(), key)
    );

    ui->lbl_count->setText(QString::number(countValue));
    // курсор не трогаем
}

void MainWindow::on_btn_min_element_clicked() {
    if (deque_model_.items.empty()) {
        return;
    }

    auto it = std::min_element(
        deque_model_.items.begin(),
        deque_model_.items.end(),
        [](const QString& a, const QString& b) { return a < b; }
    );

    deque_model_.pos = static_cast<size_t>(std::distance(deque_model_.items.begin(), it));
    applyModel();
}

void MainWindow::on_btn_max_element_clicked() {
    if (deque_model_.items.empty()) {
        return;
    }

    auto it = std::max_element(
        deque_model_.items.begin(),
        deque_model_.items.end(),
        [](const QString& a, const QString& b) { return a < b; }
    );

    deque_model_.pos = static_cast<size_t>(std::distance(deque_model_.items.begin(), it));
    applyModel();
}

// === Сортировка и производные операции ===

void MainWindow::on_btn_merge_sort_clicked() {
    sortMode_ = SortMode::CaseSensitive;
    deque_model_.items = MergeSort(deque_model_.items, currentComp());
    deque_model_.toBegin(); // на сортировку курсор в 0
    applyModel();
}

void MainWindow::on_btn_merge_sOrT_clicked() {
    sortMode_ = SortMode::CaseInsensitive;
    deque_model_.items = MergeSort(deque_model_.items, currentComp());
    deque_model_.toBegin(); // на сортировку курсор в 0
    applyModel();
}

void MainWindow::on_btn_unique_clicked() {
    if (deque_model_.items.empty() || sortMode_ == SortMode::None) {
        return;
    }

    auto less = currentComp();
    auto equalByLess = [less](const QString& a, const QString& b) {
        return !less(a, b) && !less(b, a);
    };

    auto newEnd = std::unique(deque_model_.items.begin(), deque_model_.items.end(), equalByLess);
    deque_model_.items.erase(newEnd, deque_model_.items.end());
    deque_model_.toBegin(); // unique начало
    applyModel();
}

void MainWindow::on_btn_lower_bound_clicked() {
    const QString key = ui->txt_elem_content->text().trimmed();
    if (key.isEmpty() || sortMode_ == SortMode::None) {
        return;
    }

    auto it = std::lower_bound(deque_model_.items.begin(), deque_model_.items.end(), key, currentComp());
    deque_model_.pos = static_cast<size_t>(std::distance(deque_model_.items.begin(), it)); // может быть end
    applyModel();
}

void MainWindow::on_btn_upper_bound_clicked() {
    const QString key = ui->txt_elem_content->text().trimmed();
    if (key.isEmpty() || sortMode_ == SortMode::None) {
        return;
    }

    auto it = std::upper_bound(deque_model_.items.begin(), deque_model_.items.end(), key, currentComp());
    deque_model_.pos = static_cast<size_t>(std::distance(deque_model_.items.begin(), it)); // может быть end
    applyModel();
}

// === Наборы ===

void MainWindow::on_btn_tea_clicked() {
    deque_model_.items = tea;
    deque_model_.toBegin();
    sortMode_ = SortMode::None;
    applyModel();
}

void MainWindow::on_btn_cakes_clicked() {
    deque_model_.items = cakes;
    deque_model_.toBegin();
    sortMode_ = SortMode::None;
    applyModel();
}