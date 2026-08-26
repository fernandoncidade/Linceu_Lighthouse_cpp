#include "gevmov_08_adicionar_item_tabela.h"

#include <QColor>
#include <QDir>
#include <QMetaObject>
#include <QMetaMethod>
#include <QTableView>
#include <QTableWidget>
#include <QTableWidgetItem>

#include "source/tools/fil_01_Filtros.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoEventoMovido {

QString normalizar_caminho_tabela(const QString& caminho) {
    return QDir::toNativeSeparators(QDir::cleanPath(caminho)).replace('/', '\\');
}

void adicionar_item_tabela(QObject* interfaceMonitor, const QVariantMap& evento, bool atualizar_interface) {
    try {
        if (!interfaceMonitor || evento.isEmpty()) return;

        auto* loc = interfaceMonitor->property("loc").value<source::ui::LocalizadorQt*>();
        QObject* painel_filtros = interfaceMonitor->property("painel_filtros").value<QObject*>();
        QObject* tabelaObj = interfaceMonitor->property("tabela_dados").value<QObject*>();

        QString tipo = evento.value("tipo_operacao").toString();
        const QString tipo_traduzido = loc ? loc->traduzir_tipo_operacao(tipo) : tipo;

        if (painel_filtros) {
            bool permitido = true;
            if (auto* filtros = qobject_cast<source::tools::Filtros*>(painel_filtros)) {
                permitido = filtros->verificar_filtro_operacao(tipo_traduzido);
            } else {
                const int method_index = painel_filtros->metaObject()->indexOfMethod("verificar_filtro_operacao(QString)");
                if (method_index >= 0) {
                    QMetaObject::invokeMethod(painel_filtros, "verificar_filtro_operacao", Qt::DirectConnection,
                                              Q_RETURN_ARG(bool, permitido), Q_ARG(QString, tipo_traduzido));
                }
            }
            if (!permitido) return;
        }

        if (auto* view = qobject_cast<QTableView*>(tabelaObj)) {
            QMetaObject::invokeMethod(interfaceMonitor, "inserir_evento_streaming", Qt::QueuedConnection, Q_ARG(QVariantMap, evento));
            if (atualizar_interface) {
                QMetaObject::invokeMethod(interfaceMonitor, "atualizar_status", Qt::QueuedConnection);
            }
            return;
        }

        auto* tabela = qobject_cast<QTableWidget*>(tabelaObj);
        if (!tabela) return;

        const QHash<QString, QColor> cores = {
            {loc ? loc->get_text("op_renamed") : QStringLiteral("op_renamed"), QColor(0, 255, 0)},
            {loc ? loc->get_text("op_added") : QStringLiteral("op_added"), QColor(0, 0, 255)},
            {loc ? loc->get_text("op_deleted") : QStringLiteral("op_deleted"), QColor(255, 0, 0)},
            {loc ? loc->get_text("op_modified") : QStringLiteral("op_modified"), QColor(255, 98, 0)},
            {loc ? loc->get_text("op_moved") : QStringLiteral("op_moved"), QColor(255, 0, 255)},
            {loc ? loc->get_text("op_scanned") : QStringLiteral("op_scanned"), QColor(128, 128, 128)}};

        const QStringList ordem = {"tipo_operacao", "nome", "dir_anterior", "dir_atual", "timestamp"};
        tabela->insertRow(0);
        for (int col = 0; col < ordem.size() && col < tabela->columnCount(); ++col) {
            const QString key = ordem.at(col);
            QString valor = evento.value(key).toString();

            if (key == "tipo_operacao") valor = loc ? loc->traduzir_tipo_operacao(valor) : valor;
            if ((key == "dir_anterior" || key == "dir_atual") && !valor.isEmpty() && valor != ".") {
                valor = normalizar_caminho_tabela(valor);
            }

            auto* item = new QTableWidgetItem(valor);
            if (key == "tipo_operacao") {
                item->setBackground(cores.value(valor, QColor(255, 255, 255)));
            }
            tabela->setItem(0, col, item);
        }

        if (atualizar_interface) {
            QMetaObject::invokeMethod(interfaceMonitor, "atualizar_status", Qt::QueuedConnection);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao adicionar item na tabela", true);
    }
}

}  // namespace source::services::GerenciamentoEventoMovido
