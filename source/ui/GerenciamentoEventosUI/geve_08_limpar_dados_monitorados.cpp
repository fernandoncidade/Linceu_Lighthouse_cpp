#include "geve_08_limpar_dados_monitorados.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMetaObject>
#include <QSet>
#include <QStringList>

#include <exception>

#include "geve_09_limpar_dados_interface.h"
#include "source/ui/ui_04_GerenciadorEventosUI.h"
#include "source/utils/CaminhoPersistenteUtils.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoEventosUI {

void limpar_dados_monitorados(source::ui::GerenciadorEventosUI* self) {
    try {
        if (self == nullptr || self->interface_obj() == nullptr) {
            return;
        }

        auto* interface_obj = self->interface_obj();
        QObject* observador = interface_obj->property("observador").value<QObject*>();
        if (observador != nullptr && observador->property("ativo").toBool()) {
            QObject* gerenciador_botoes = interface_obj->property("gerenciador_botoes").value<QObject*>();
            if (gerenciador_botoes != nullptr) {
                QMetaObject::invokeMethod(gerenciador_botoes, "alternar_analise_diretorio");
            }
        }

        QObject* evento_base = interface_obj->property("evento_base").value<QObject*>();
        if (evento_base != nullptr) {
            QMetaObject::invokeMethod(evento_base, "limpar_registros");
        }

        const QString modulo_dir = QFileInfo(QString::fromUtf8(__FILE__)).absolutePath();
        QStringList db_paths = {
            "monitoramento.db",
            QDir(modulo_dir).filePath("../Observador/monitoramento.db"),
            QFileInfo(QDir(modulo_dir).filePath("../Observador/monitoramento.db")).absoluteFilePath(),
            QDir(QCoreApplication::applicationDirPath()).filePath("monitoramento.db"),
            QDir(QCoreApplication::applicationDirPath()).filePath("source/ui/Observador/monitoramento.db"),
            QDir(QCoreApplication::applicationDirPath()).filePath("source/services/Observador/monitoramento.db"),
            QDir(source::utils::obter_caminho_persistente()).filePath("monitoramento.db")
        };

        bool removed = false;
        QSet<QString> processed_paths;
        for (const auto& path : db_paths) {
            const QString full_path = QFileInfo(path).absoluteFilePath();
            if (processed_paths.contains(full_path)) {
                continue;
            }
            processed_paths.insert(full_path);

            if (QFile::exists(full_path)) {
                if (QFile::remove(full_path)) {
                    removed = true;
                } else {
                    source::utils::LogManager::get_logger().warning(
                        QString("Não foi possível remover o arquivo de banco de dados: %1").arg(full_path));
                }
            }
        }
        if (!removed) {
            source::utils::LogManager::get_logger().warning("Arquivo de banco de dados não encontrado para remoção.");
        }

        limpar_dados_interface(self);
    } catch (const std::exception& e) {
        source::utils::LogManager::get_logger().error(
            QString("Erro ao limpar dados monitorados: %1").arg(QString::fromUtf8(e.what())), true);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao limpar dados monitorados", true);
    }
}

}
