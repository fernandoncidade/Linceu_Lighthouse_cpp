#pragma once

#include <QHash>
#include <QMetaObject>
#include <QString>
#include <QVector>

class QObject;

namespace source::tools {

class Filtros;

class AdministradorFiltros {
public:
    explicit AdministradorFiltros(source::tools::Filtros* parent);
    ~AdministradorFiltros();

    void desconectar_sinais();
    void aplicar_filtros();
    void limpar_filtros();
    void sincronizar_menu_principal_com_filtros();
    QString atualizar_contagem();
    QString atualizar_contagem_eventos_monitorados();
    bool verificar_filtro_operacao(const QString& operacao) const;
    void atualizar_contagem_apos_idioma(const QString& idioma);
    void notificar_alteracao_idioma();
    void salvar_estado_checkboxes();

    static QHash<QString, bool> filtros_estado;

    source::tools::Filtros* parent_obj() const;
    void set_parent_obj(source::tools::Filtros* parent);
    QVector<QMetaObject::Connection>& conexoes_sinais_ref();
    QHash<QString, int>& contadores_ref();
    QHash<QString, int>& contadores_originais_ref();

private:
    source::tools::Filtros* parent_;
    QVector<QMetaObject::Connection> conexoes_sinais_;
    QHash<QString, int> contadores_;
    QHash<QString, int> contadores_originais_;
};

}  // namespace source::tools
