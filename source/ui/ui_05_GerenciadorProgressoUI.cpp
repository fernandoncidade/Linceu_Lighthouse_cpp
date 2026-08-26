#include "ui_05_GerenciadorProgressoUI.h"

#include <QMetaObject>
#include <QProgressBar>
#include <QTimer>

#include "source/utils/LogManager.h"

namespace source::ui {
namespace {

QString get_text(QObject* loc, const QString& key) {
    QString text;
    if (loc != nullptr) {
        QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, text), Q_ARG(QString, key));
    }
    return text;
}

void set_label_text(QObject* interface_obj, const QString& text) {
    if (interface_obj == nullptr) {
        return;
    }
    QObject* rotulo_resultado = interface_obj->property("rotulo_resultado").value<QObject*>();
    if (rotulo_resultado != nullptr) {
        QMetaObject::invokeMethod(rotulo_resultado, "setText", Q_ARG(QString, text));
    }
}

}  // namespace

GerenciadorProgressoUI::GerenciadorProgressoUI(QObject* interface_principal)
    : interface_(interface_principal), loc_(nullptr), has_scan_start_time_(false) {
    if (interface_ != nullptr) {
        loc_ = interface_->property("loc").value<QObject*>();
    }
}

void GerenciadorProgressoUI::criar_barra_progresso() {
    try {
        auto* barra = interface_->property("barra_progresso").value<QProgressBar*>();
        if (barra != nullptr) {
            barra->setValue(0);
            barra->setFormat("%p%");
            barra->setTextVisible(true);
            barra->show();
        }
        set_label_text(interface_, get_text(loc_, "preparing_scan"));
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao criar barra de progresso", true);
    }
}

void GerenciadorProgressoUI::atualizar_progresso_scan(int progresso, int contador, int total) {
    try {
        const QString scanning_text = get_text(loc_, "scanning");
        const QString remaining_text = get_text(loc_, "remaining");
        const QString complete_text = get_text(loc_, "scan_complete");
        const QString items_text = get_text(loc_, "items");

        QString msg = QString("%1 %2% (%3/%4)").arg(scanning_text).arg(progresso).arg(contador).arg(total);
        if (total > 1000 && contador > 0 && progresso < 100) {
            if (!has_scan_start_time_) {
                scan_start_time_ = QDateTime::currentDateTime();
                has_scan_start_time_ = true;
            }
            const qint64 elapsed = scan_start_time_.secsTo(QDateTime::currentDateTime());
            if (progresso > 0) {
                const double estimatedTotal = static_cast<double>(elapsed) * (100.0 / static_cast<double>(progresso));
                const int remaining = static_cast<int>(estimatedTotal - static_cast<double>(elapsed));
                const int min = remaining / 60;
                const int sec = remaining % 60;
                if (min > 0) {
                    msg += QString(" - %1: %2m %3s").arg(remaining_text).arg(min).arg(sec);
                } else {
                    msg += QString(" - %1: %2s").arg(remaining_text).arg(sec);
                }
            }
        }

        auto* barra = interface_->property("barra_progresso").value<QProgressBar*>();
        if (barra != nullptr) {
            if (!barra->isVisible()) {
                barra->show();
            }
            barra->setValue(progresso);
            if (progresso >= 100) {
                QTimer::singleShot(1000, barra, &QProgressBar::hide);
                has_scan_start_time_ = false;
            }
        }

        if (progresso >= 100) {
            msg = QString("%1 (%2 %3)").arg(complete_text).arg(total).arg(items_text);
        }

        set_label_text(interface_, msg);
        interface_->setProperty("status_text", msg);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar progresso", true);
    }
}

void GerenciadorProgressoUI::atualizar_progresso_traducao(int progresso, int, int) {
    try {
        set_label_text(interface_, get_text(loc_, "translating_table"));

        auto* barra = interface_->property("barra_progresso").value<QProgressBar*>();
        if (barra != nullptr) {
            if (!barra->isVisible()) {
                barra->show();
            }
            barra->setValue(progresso);
            if (progresso >= 100) {
                QTimer::singleShot(1000, barra, &QProgressBar::hide);
                has_scan_start_time_ = false;
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar progresso de tradução", true);
    }
}

}  // namespace source::ui
