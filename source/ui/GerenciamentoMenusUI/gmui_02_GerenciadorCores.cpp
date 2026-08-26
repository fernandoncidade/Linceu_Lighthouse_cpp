#include "gmui_02_GerenciadorCores.h"

#include <QColor>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QIcon>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaObject>
#include <QPointer>
#include <QTimer>
#include <QWidget>

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/ApplicationPathUtils.h"
#include "source/utils/CaminhoPersistenteUtils.h"
#include "source/utils/IconUtils.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoMenusUI {

QString get_palette_icon_path(const QString& tipo) {
    try {
        return source::utils::get_icon_path(tipo == QStringLiteral("paleta") ? QStringLiteral("blocos-de-cores.ico") : QStringLiteral("paleta-de-cores.ico"));
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao obter caminho do ícone da paleta", true);
        return QString();
    }
}

GerenciadorCores::GerenciadorCores(QObject* interface_principal, QObject* parent)
    : QObject(parent), interface_(interface_principal), loc_(nullptr), thread_cores_(nullptr) {
    cores_operacoes_ = {
        {"op_renamed", "#00ff00"}, {"op_added", "#0000ff"}, {"op_deleted", "#ff0000"},
        {"op_modified", "#ff6200"}, {"op_moved", "#ff00ff"}, {"op_scanned", "#808080"}
    };
    if (interface_) loc_ = interface_->property("loc").value<QObject*>();
    carregar_cores();
}

void GerenciadorCores::carregar_cores() {
    try {
        const QString dir = source::utils::obter_caminho_persistente();
        const QString path = QDir(dir).filePath("cores_config.json");
        QFile file(path);
        if (!file.exists() || !file.open(QIODevice::ReadOnly)) return;
        const QJsonObject root = QJsonDocument::fromJson(file.readAll()).object();
        const QJsonObject cores = root.value("cores_operacoes").toObject();
        for (auto it = cores.begin(); it != cores.end(); ++it) {
            if (cores_operacoes_.contains(it.key())) {
                cores_operacoes_[it.key()] = it.value().toString();
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao carregar configurações de cores", true);
    }
}

bool GerenciadorCores::salvar_cores() {
    try {
        const QString dir = source::utils::obter_caminho_persistente();
        QDir().mkpath(dir);
        QFile file(QDir(dir).filePath("cores_config.json"));
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) return false;
        QJsonObject root;
        QJsonObject cores;
        for (auto it = cores_operacoes_.cbegin(); it != cores_operacoes_.cend(); ++it) cores.insert(it.key(), it.value());
        root.insert("cores_operacoes", cores);
        file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
        return true;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao salvar configurações de cores", true);
        return false;
    }
}

QString GerenciadorCores::obter_diretorio_config() const {
    try {
        const auto base_path = source::utils::get_app_base_path();
        if (base_path.has_value()) {
            return QDir(base_path.value()).filePath("config");
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao obter diretório de configuração", true);
    }

    return QFileInfo(QDir(QString::fromUtf8(__FILE__)).filePath("../../config")).absoluteFilePath();
}

QColor GerenciadorCores::obter_cor_qcolor(const QString& tipo_operacao) const { return QColor(obter_cor_hex(tipo_operacao)); }

QString GerenciadorCores::obter_cor_hex(const QString& tipo_operacao) const { return cores_operacoes_.value(tipo_operacao, QStringLiteral("#333333")); }

bool GerenciadorCores::definir_cor(const QString& tipo_operacao, const QString& cor_hex) {
    if (!cores_operacoes_.contains(tipo_operacao)) return false;
    cores_operacoes_[tipo_operacao] = cor_hex;
    return true;
}

void GerenciadorCores::atualizar_cores_no_sistema() {
    try {
        if (interface_ == nullptr) {
            return;
        }

        // Agendar no loop da UI evita travamentos por espera/bloqueio durante monitoramento ativo.
        QPointer<GerenciadorCores> self_guard(this);
        QTimer::singleShot(0, interface_, [self_guard]() {
            if (self_guard.isNull()) {
                return;
            }
            self_guard->atualizar_cores_no_sistema_threadsafe();
        });
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar cores no sistema", true);
    }
}

void GerenciadorCores::atualizar_cores_no_sistema_threadsafe() {
    try {
        if (interface_ == nullptr) {
            return;
        }

        QObject* gerenciador_tabela_obj = interface_->property("gerenciador_tabela").value<QObject*>();
        if (auto* gerenciador_tabela = qobject_cast<source::services::GerenciadorTabela*>(gerenciador_tabela_obj)) {
            // Troca de cor não precisa recarregar dados do banco: apenas invalidar cache e repintar.
            gerenciador_tabela->_invalidar_cache_cores();
            gerenciador_tabela->atualizar_cores_colunas(true);
        }

        QObject* gerenciador_estatisticas_ui = interface_->property("gerenciador_estatisticas_ui").value<QObject*>();
        if (gerenciador_estatisticas_ui != nullptr) {
            QObject* gerador_atual = gerenciador_estatisticas_ui->property("gerador_atual").value<QObject*>();
            if (gerador_atual != nullptr) {
                QMetaObject::invokeMethod(gerador_atual, "atualizar_textos_traduzidos");
            }
        }

        QMetaObject::invokeMethod(interface_, "update", Qt::QueuedConnection);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar cores no sistema (threadsafe)", true);
    }
}

void GerenciadorCores::aplicar_icone_paleta(QWidget* dialog, const QString& tipo) {
    if (!dialog) return;
    dialog->setWindowIcon(QIcon(get_palette_icon_path(tipo)));
}

ThreadAtualizarCores::ThreadAtualizarCores(GerenciadorCores* gerenciador_cores, QObject* interface_obj)
    : gerenciador_cores_(gerenciador_cores), interface_(interface_obj) {}

void ThreadAtualizarCores::run() {
    try {
        if (gerenciador_cores_ != nullptr) {
            QPointer<GerenciadorCores> self_guard(gerenciador_cores_);
            QTimer::singleShot(0, gerenciador_cores_, [self_guard]() {
                if (self_guard.isNull()) {
                    return;
                }
                self_guard->atualizar_cores_no_sistema_threadsafe();
            });
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro na thread de atualização de cores", true);
    }
    emit sinal_concluido();
}

}
