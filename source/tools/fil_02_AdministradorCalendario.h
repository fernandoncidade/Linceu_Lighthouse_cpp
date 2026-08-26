#pragma once

class QDate;
class QDateTimeEdit;
class QCalendarWidget;

namespace source::tools {

class Filtros;

class AdministradorCalendario {
public:
    explicit AdministradorCalendario(source::tools::Filtros* parent);

    void mostrar_calendario(QDateTimeEdit* campo_data);
    void selecionar_data(const QDate& data, QDateTimeEdit* campo_data, QCalendarWidget* calendario);

private:
    source::tools::Filtros* parent_;
};

}  // namespace source::tools
