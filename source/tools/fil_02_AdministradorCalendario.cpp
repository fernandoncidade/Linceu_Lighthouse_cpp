#include "fil_02_AdministradorCalendario.h"

#include <QCalendarWidget>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QLocale>

#include "fil_01_Filtros.h"
#include "fil_03_AdministradorFiltros.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::tools {

AdministradorCalendario::AdministradorCalendario(source::tools::Filtros* parent)
    : parent_(parent) {}

void AdministradorCalendario::mostrar_calendario(QDateTimeEdit* campo_data) {
    try {
        if (!parent_ || !campo_data) {
            return;
        }

        auto* calendario = new QCalendarWidget(parent_);
        calendario->setWindowFlags(Qt::Popup);

        QString idioma;
        if (parent_->localizador() != nullptr) {
            idioma = parent_->localizador()->property("idioma_atual").toString();
        }

        const QLocale locale_qt = idioma.isEmpty() ? QLocale() : QLocale(idioma);
        calendario->setLocale(locale_qt);
        calendario->setFirstDayOfWeek(locale_qt.firstDayOfWeek());
        calendario->setSelectedDate(campo_data->date());

        QObject::connect(calendario, &QCalendarWidget::clicked, parent_,
                         [this, campo_data, calendario](const QDate& data) {
                             selecionar_data(data, campo_data, calendario);
                         });

        const QPoint pos = campo_data->mapToGlobal(campo_data->rect().bottomLeft());
        calendario->move(pos);
        calendario->show();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao exibir calendário", true);
    }
}

void AdministradorCalendario::selecionar_data(const QDate& data, QDateTimeEdit* campo_data, QCalendarWidget* calendario) {
    try {
        if (!parent_ || !campo_data) {
            if (calendario) {
                calendario->close();
            }
            return;
        }

        const QDateTime dt(data, campo_data->dateTime().time());
        campo_data->setDateTime(dt);

        if (calendario) {
            calendario->close();
        }

        if (parent_->administrador_filtros_obj() != nullptr) {
            parent_->administrador_filtros_obj()->aplicar_filtros();
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao selecionar data no calendário", true);
        if (calendario) {
            calendario->close();
        }
    }
}

}  // namespace source::tools
