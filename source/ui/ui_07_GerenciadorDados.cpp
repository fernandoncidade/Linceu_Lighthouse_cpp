#include "ui_07_GerenciadorDados.h"

#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QMetaObject>
#include <QVariantMap>

#include "source/ui/ui_01_GerenciadorBotoes.h"
#include "source/ui/GerenciamentoBotoes/geb_03_exportar_dados.h"
#include "source/utils/LogManager.h"

namespace {
QString get_text(QObject* loc, const QString& key) {
    QString text;
    if (loc != nullptr) {
        QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, text), Q_ARG(QString, key));
    }
    return text;
}

QString format_with_one_arg(QString pattern, const QString& arg) {
    const int brace_pos = pattern.indexOf(QStringLiteral("{}"));
    if (brace_pos >= 0) {
        pattern.replace(brace_pos, 2, arg);
        return pattern;
    }
    if (pattern.contains(QStringLiteral("%1"))) {
        return pattern.arg(arg);
    }
    return pattern;
}

QString formato_com_ponto(QString formato) {
    formato = formato.trimmed().toLower();
    if (formato.isEmpty()) {
        return formato;
    }
    if (!formato.startsWith(QStringLiteral("."))) {
        formato.prepend(QStringLiteral("."));
    }
    return formato;
}

QString mensagem_sucesso_salvamento(QObject* loc,
                                    const QString& destino,
                                    const QString& formato,
                                    bool incluir_formato) {
    if (incluir_formato) {
        const QString extensao = formato_com_ponto(formato);
        if (!extensao.isEmpty()) {
            return QStringLiteral("Arquivo (%1) salvo com sucesso!").arg(extensao);
        }
    }

    return format_with_one_arg(get_text(loc, "file_saved"), destino);
}

bool executar_exportacao(QObject* interface_obj, QObject* tabela_dados, const QString& formato,
                         const QString& destino) {
    if (!interface_obj || !tabela_dados) return false;
    QObject* loc = interface_obj->property("loc").value<QObject*>();
    source::ui::GerenciadorBotoes botoes_temp(interface_obj, loc);
    return source::ui::GerenciamentoBotoes::exportar_dados(
        &botoes_temp, static_cast<void*>(tabela_dados), formato, destino);
}

bool exportar_e_mostrar_dialogos(QObject* interface_obj,
                                 const QString& formato,
                                 const QString& destino,
                                 bool sucesso_com_formato) {
    auto* parent = qobject_cast<QWidget*>(interface_obj);
    auto* loc = interface_obj ? interface_obj->property("loc").value<QObject*>() : nullptr;

    try {
        QObject* tabela_dados = interface_obj ? interface_obj->property("tabela_dados").value<QObject*>() : nullptr;
        const bool resultado = executar_exportacao(interface_obj, tabela_dados, formato, destino);

        if (resultado) {
            QMessageBox::information(parent,
                                     get_text(loc, "success"),
                                     mensagem_sucesso_salvamento(loc, destino, formato, sucesso_com_formato));
        } else {
            source::utils::LogManager::get_logger().warning("Exportação retornou status de falha");
            QMessageBox::warning(parent, get_text(loc, "warning"), get_text(loc, "save_incomplete"));
        }

        return resultado;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao exportar dados", true);
        QMessageBox::critical(parent,
                              get_text(loc, "error"),
                              format_with_one_arg(get_text(loc, "save_error"),
                                                  QStringLiteral("erro desconhecido")));
        return false;
    }
}
}

namespace source::ui {

GerenciadorDados::GerenciadorDados(QObject* interface_principal) : interface_(interface_principal) {}

void GerenciadorDados::salvar_dados() {
    const QVariant ultimo_var = interface_->property("ultimo_salvamento");
    const QVariantMap ultimo = ultimo_var.toMap();

    if (ultimo_var.isValid() && !ultimo_var.isNull() && !ultimo.isEmpty() &&
        ultimo.contains("format") && ultimo.contains("path")) {
        const QString formato = ultimo.value("format").toString();
        const QString destino = ultimo.value("path").toString();
        exportar_e_mostrar_dialogos(interface_, formato, destino, true);
    } else {
        abrir_salvar_como();
    }
}

void GerenciadorDados::abrir_salvar_como() {
    auto* parent = qobject_cast<QWidget*>(interface_);
    QFileDialog dialog(parent);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    const QStringList filters = {
        "Excel Files (*.xlsx)", "CSV Files (*.csv)", "Text Files (*.txt)",
        "JSON Files (*.json)", "XML Files (*.xml)", "Database Files (*.db)"};
    dialog.setNameFilters(filters);

    if (!dialog.exec()) {
        return;
    }

    QString selected_file = dialog.selectedFiles().value(0);
    const QString file_format = dialog.selectedNameFilter();

    const QHash<QString, QString> format_map = {
        {"Excel Files (*.xlsx)", "xlsx"}, {"CSV Files (*.csv)", "csv"}, {"Text Files (*.txt)", "txt"},
        {"JSON Files (*.json)", "json"}, {"XML Files (*.xml)", "xml"}, {"Database Files (*.db)", "db"}};

    const QString selected_format = format_map.value(file_format);
    const QString expected_ext = QString(".") + selected_format;
    if (!selected_file.toLower().endsWith(expected_ext)) {
        selected_file += expected_ext;
    }

    const QString dest_dir = QFileInfo(selected_file).absolutePath();
    if (!QDir(dest_dir).exists() && !QDir().mkpath(dest_dir)) {
        source::utils::LogManager::get_logger().error("Erro ao criar diretório", true);
        const auto* loc = interface_->property("loc").value<QObject*>();
        QMessageBox::warning(parent, get_text(const_cast<QObject*>(loc), "error"), get_text(const_cast<QObject*>(loc), "dir_create_error"));
        return;
    }

    QVariantMap ultimo_salvamento;
    ultimo_salvamento.insert("path", selected_file);
    ultimo_salvamento.insert("format", selected_format);
    ultimo_salvamento.insert("timestamp", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    interface_->setProperty("ultimo_salvamento", ultimo_salvamento);

    exportar_e_mostrar_dialogos(interface_, selected_format, selected_file, false);
}

}  // namespace source::ui
