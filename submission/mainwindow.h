#pragma once

#include <QMainWindow>
#include <QListWidgetItem>
#include <QSignalBlocker>
#include <QString>
#include <deque>
#include <random>
#include <functional>

#include "model.h" 
#include "algo.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Наборы данных для кнопок "чай" и "торты"
    static const std::deque<QString> tea;
    static const std::deque<QString> cakes;

    inline void SetRandomGen(const std::mt19937& random_gen) {
        random_gen_ = random_gen;
    }

private slots:
    void on_btn_push_back_clicked();
    void on_btn_push_front_clicked();
    void on_btn_pop_back_clicked();
    void on_btn_pop_front_clicked();
    void on_btn_clear_clicked();

    void on_btn_edit_clicked();
    void on_btn_prev_clicked();
    void on_btn_next_clicked();
    void on_btn_begin_clicked();
    void on_btn_end_clicked();
    void on_btn_erase_clicked();
    void on_btn_insert_clicked();
    void on_list_widget_currentRowChanged(int currentRow);

    void on_btn_resize_clicked();
    void on_btn_reverse_clicked();

    // Алгоритмы
    void on_btn_find_clicked();
    void on_btn_count_clicked();
    void on_btn_min_element_clicked();
    void on_btn_max_element_clicked();
    void on_btn_merge_sort_clicked();    // case-sensitive
    void on_btn_merge_sOrT_clicked();    // case-insensitive
    void on_btn_shuffle_clicked();
    void on_btn_unique_clicked();
    void on_btn_lower_bound_clicked();
    void on_btn_upper_bound_clicked();

    // Наборы
    void on_btn_tea_clicked();
    void on_btn_cakes_clicked();

private:
    // Простейшая модель: держим индекс вместо итератора
    struct Model {
        std::deque<QString> items;
        size_t pos = 0;                 // pos == items.size() => end()
        bool atEnd() const { return pos >= items.size(); }
        void clamp() { if (pos > items.size()) pos = items.size(); }
        void toBegin() { pos = 0; }
        void toEnd() { pos = items.size(); }
    };

    enum class SortMode { None, CaseSensitive, CaseInsensitive };

    using Cmp = std::function<bool(const QString&, const QString&)>;

    Cmp currentComp() const;
    void applyModel();
    void applyIterator();
    void updateControls();

private:
    Model deque_model_;
    Ui::MainWindow *ui = nullptr;
    std::mt19937 random_gen_{ std::random_device{}() };
    SortMode sortMode_ = SortMode::None;
};