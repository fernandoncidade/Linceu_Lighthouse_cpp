#include "ui_01_GerenciadorBotoes.h"

#include <QString>

#include <QDir>

#include "source/ui/GerenciamentoBotoes/geb_01_selecionar_diretorio.h"
#include "source/ui/GerenciamentoBotoes/geb_02_alternar_analise_diretorio.h"
#include "source/ui/GerenciamentoBotoes/geb_03_exportar_dados.h"
#include "source/ui/GerenciamentoBotoes/geb_04_exportar_para_sqlite.h"
#include "source/ui/GerenciamentoBotoes/geb_05_limpar_dados.h"
#include "source/ui/GerenciamentoBotoes/geb_06_pausar_monitoramento_ou_escaneamento.h"
#include "source/utils/CaminhoPersistenteUtils.h"
#include "source/utils/LogManager.h"

namespace source::ui {
namespace {

void inicializar_propriedades_db(QObject* interface_obj) {
    if (interface_obj == nullptr) {
        return;
    }

    const QString db_path =
        QDir(source::utils::obter_caminho_persistente()).filePath(QStringLiteral("monitoramento.db"));
    interface_obj->setProperty("_monitoramento_db_path", db_path);
    if (!interface_obj->property("_monitoramento_db_tem_dados").isValid()) {
        interface_obj->setProperty("_monitoramento_db_tem_dados", false);
    }
    if (!interface_obj->property("_permitir_carga_db").isValid()) {
        interface_obj->setProperty("_permitir_carga_db", false);
    }
}

}  // namespace

GerenciadorBotoes::GerenciadorBotoes(QObject* interface_obj, QObject* loc_obj)
    : interface_(interface_obj), loc_(loc_obj) {
    try {
        inicializar_propriedades_db(interface_);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar GerenciadorBotoes", true);
    }
}

QObject* GerenciadorBotoes::interface_obj() const {
    return interface_;
}

QObject* GerenciadorBotoes::loc_obj() const {
    return loc_;
}

void GerenciadorBotoes::selecionar_diretorio() {
    GerenciamentoBotoes::selecionar_diretorio(this);
}

void GerenciadorBotoes::alternar_analise_diretorio() {
    GerenciamentoBotoes::alternar_analise_diretorio(this);
}

bool GerenciadorBotoes::exportar_dados(void* tabela_dados, const QString& formato, const QString& destino) {
    return GerenciamentoBotoes::exportar_dados(this, tabela_dados, formato, destino);
}

bool GerenciadorBotoes::exportar_para_sqlite(void* tabela_dados, const QString& destino) {
    return GerenciamentoBotoes::exportar_para_sqlite(this, tabela_dados, destino);
}

void GerenciadorBotoes::limpar_dados() {
    GerenciamentoBotoes::limpar_dados(this);
}

void GerenciadorBotoes::pausar_monitoramento_ou_escaneamento() {
    GerenciamentoBotoes::pausar_monitoramento_ou_escaneamento(this);
}

}  // namespace source::ui
