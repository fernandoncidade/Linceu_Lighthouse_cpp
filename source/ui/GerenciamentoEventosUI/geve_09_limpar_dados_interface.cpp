#include "geve_09_limpar_dados_interface.h"

#include <QMetaObject>
#include <QTableView>
#include <QTableWidget>
#include <QVariant>
#include <QVariantMap>

#include <exception>

#include "source/services/GerenciamentoEventoMovido/gevmov_10_PendenciasMovimentacao.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/tools/fil_01_Filtros.h"
#include "source/ui/ui_04_GerenciadorEventosUI.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoEventosUI {

void limpar_dados_interface(source::ui::GerenciadorEventosUI* self) {
    try {
        if (self == nullptr || self->interface_obj() == nullptr) {
            return;
        }

        auto* interface_obj = self->interface_obj();
        QObject* tabela_obj = interface_obj->property("tabela_dados").value<QObject*>();
        if (auto* tabela = qobject_cast<QTableWidget*>(tabela_obj)) {
            tabela->clearContents();
            tabela->setRowCount(0);
            tabela->viewport()->update();
        } else if (auto* view = qobject_cast<QTableView*>(tabela_obj)) {
            if (!QMetaObject::invokeMethod(interface_obj, "limpar_tabelas_idioma")) {
                QMetaObject::invokeMethod(view, "clearSelection");
            }
            if (view->viewport() != nullptr) {
                view->viewport()->update();
            }
        }

        QObject* gerenciador_colunas_obj = interface_obj->property("gerenciador_colunas").value<QObject*>();
        if (auto* gerenciador_colunas = qobject_cast<source::services::GerenciadorColunas*>(gerenciador_colunas_obj)) {
            gerenciador_colunas->cache_metadados.clear();
        } else if (gerenciador_colunas_obj != nullptr) {
            QMetaObject::invokeMethod(gerenciador_colunas_obj, "limpar_cache_metadados");
        }

        QObject* painel_filtros_obj = interface_obj->property("painel_filtros").value<QObject*>();
        if (auto* painel_filtros = qobject_cast<source::tools::Filtros*>(painel_filtros_obj)) {
            painel_filtros->limpar_filtros();
        } else if (painel_filtros_obj != nullptr) {
            QObject* administrador_filtros = painel_filtros_obj->property("administrador_filtros").value<QObject*>();
            if (administrador_filtros != nullptr) {
                QMetaObject::invokeMethod(administrador_filtros, "limpar_filtros");
            } else {
                QMetaObject::invokeMethod(painel_filtros_obj, "limpar_filtros");
            }
        }

        interface_obj->setProperty("ultimo_salvamento", QVariant());
        interface_obj->setProperty("diretorio_atual", QVariant());

        source::services::GerenciamentoEventoMovido::limpar_pendencias_movimentacao(interface_obj);

        QMetaObject::invokeMethod(interface_obj, "atualizar_status");
    } catch (const std::exception& e) {
        source::utils::LogManager::get_logger().error(
            QString("Erro ao limpar interface: %1").arg(QString::fromUtf8(e.what())), true);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao limpar interface", true);
    }
}

}
