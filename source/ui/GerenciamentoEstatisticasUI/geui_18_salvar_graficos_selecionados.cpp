#include "geui_18_salvar_graficos_selecionados.h"

#include <QAbstractButton>
#include <QApplication>
#include <QCheckBox>
#include <QDateTime>
#include <QDir>
#include <QEventLoop>
#include <QFileDialog>
#include <QHash>
#include <QImage>
#include <QMessageBox>
#include <QMetaObject>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QTabWidget>
#include <QTimer>
#include <QStringList>
#include <QVariantHash>
#include <QVariantMap>
#include <QWidget>

#include "geui_14_gerar_todos_graficos.h"
#include "geui_28_traduzir_botoes_detalhes.h"
#include "geui_29_abrir_diretorio.h"
#include "source/ui/ui_06_GerenciadorEstatisticasUI.h"
#include "source/utils/LogManager.h"

namespace {
QString get_text(QObject* loc, const QString& key, const QString& fallback = QString()) {
    QString text = fallback;
    if (loc != nullptr) {
        QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, text), Q_ARG(QString, key), Q_ARG(QString, fallback));
    }
    return text;
}

QString format_text(QString pattern, const QStringList& args) {
    for (const QString& value : args) {
        const int brace_pos = pattern.indexOf(QStringLiteral("{}"));
        if (brace_pos >= 0) {
            pattern.replace(brace_pos, 2, value);
        } else if (pattern.contains(QLatin1Char('%'))) {
            pattern = pattern.arg(value);
        }
    }
    return pattern;
}

QString sanitize_file_name(QString nome) {
    static const QString invalid_chars = QStringLiteral("<>:\"/\\|?*");
    for (const QChar ch : invalid_chars) {
        nome.replace(ch, QLatin1Char('_'));
    }
    nome = nome.trimmed();
    while (!nome.isEmpty() && (nome.endsWith(QLatin1Char('.')) || nome.endsWith(QLatin1Char(' ')))) {
        nome.chop(1);
    }
    return nome.isEmpty() ? QStringLiteral("grafico") : nome;
}

QVariant obter_figura_cache(const QVariantHash& graficos_dados, const QString& titulo) {
    const QVariant entry_variant = graficos_dados.value(titulo);
    const QVariantMap entry_map = entry_variant.toMap();
    if (entry_map.contains(QStringLiteral("fig"))) {
        return entry_map.value(QStringLiteral("fig"));
    }

    if (entry_map.contains(QStringLiteral("kind"))) {
        return entry_map;
    }

    return entry_variant;
}

bool salvar_figura_do_cache(const QVariantHash& graficos_dados,
                            QObject* loc,
                            const QString& titulo,
                            const QString& arquivo_destino) {
    const QVariant fig = obter_figura_cache(graficos_dados, titulo);
    const QVariantMap fig_map = fig.toMap();
    if (!fig.isValid() || fig_map.isEmpty()) {
        return false;
    }

    return source::ui::GerenciamentoEstatisticasUI::renderizar_grafico_para_imagem(
        titulo, fig, arquivo_destino, QSize(1920, 1200), loc);
}
}

namespace source::ui::GerenciamentoEstatisticasUI {

void salvar_graficos_selecionados(source::ui::GerenciadorEstatisticasUI* self) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        QObject* loc = self->loc_obj();
        QWidget* parent = qobject_cast<QWidget*>(iface);
        const QVariantHash mapa = iface->property("checkboxes_graficos").toHash();
        const QVariantHash graficos_dados = iface->property("graficos_dados").toHash();

        QStringList graficos_selecionados;
        for (auto it = mapa.cbegin(); it != mapa.cend(); ++it) {
            const QString titulo = it.key();
            auto* c = it.value().toMap().value("checkbox").value<QCheckBox*>();
            if (c && c->isChecked()) {
                graficos_selecionados.append(titulo);
            }
        }

        if (graficos_selecionados.isEmpty()) {
            QMessageBox::warning(
                parent,
                get_text(loc, "warning", "Warning"),
                get_text(loc, "no_graphs_selected", "Nenhum gráfico selecionado"));
            return;
        }

        const QString diretorio = QFileDialog::getExistingDirectory(
            parent,
            get_text(loc, "select_save_dir", "Selecionar diretório"),
            QString(),
            QFileDialog::ShowDirsOnly);
        if (diretorio.isEmpty()) {
            return;
        }

        QApplication::setOverrideCursor(Qt::WaitCursor);
        try {
            const QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
            const QString estatisticas_texto = get_text(loc, "statistics", "Statistics").replace(':', "").trimmed();
            const QString selecionados_texto = get_text(loc, "selected", "Selecionados");
            const QString diretorio_graficos = QDir(diretorio).filePath(estatisticas_texto + "_" + selecionados_texto + "_" + timestamp);
            QDir().mkpath(diretorio_graficos);

            QHash<QString, bool> resultados;
            int graficos_salvos = 0;

            for (const QString& titulo : graficos_selecionados) {
                const QString arquivo_destino =
                    QDir(diretorio_graficos).filePath(sanitize_file_name(titulo) + QStringLiteral(".png"));

                try {
                    const bool ok = salvar_figura_do_cache(graficos_dados, loc, titulo, arquivo_destino);
                    if (!ok) {
                        source::utils::LogManager::get_logger().error(
                            QString("Nao foi possivel exportar o grafico selecionado: %1").arg(titulo), false);
                    }
                    resultados.insert(titulo, ok);
                    if (ok) {
                        graficos_salvos += 1;
                    }
                } catch (...) {
                    source::utils::LogManager::get_logger().error(
                        QString("Erro ao salvar %1").arg(titulo), true);
                    resultados.insert(titulo, false);
                }

                // Mantem a interface responsiva durante exportacao em lote.
                QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
            }

            QApplication::restoreOverrideCursor();

            QStringList detalhes;
            for (auto it = resultados.cbegin(); it != resultados.cend(); ++it) {
                const QString status = it.value()
                                           ? get_text(loc, "success", "Sucesso")
                                           : get_text(loc, "failure", "Falha");
                detalhes.append(QString("- %1: %2").arg(it.key(), status));
            }

            QMessageBox msg(parent);
            msg.setWindowTitle(get_text(loc, "graphs_saved", "Gráficos salvos"));
            msg.setIcon(QMessageBox::Information);
            msg.setText(
                format_text(
                    get_text(loc, "selected_graphs_saved_detail", "Gráficos selecionados salvos: {}/{} em {}"),
                    {QString::number(graficos_salvos), QString::number(graficos_selecionados.size()), diretorio_graficos}));
            msg.setDetailedText(detalhes.join('\n'));
            msg.addButton(get_text(loc, "ok", "OK"), QMessageBox::AcceptRole);
            QAbstractButton* btn_open = msg.addButton(get_text(loc, "open_folder", "Abrir pasta"), QMessageBox::ActionRole);

            traduzir_botoes_detalhes(self, &msg);
            QTimer timer;
            QObject::connect(&timer, &QTimer::timeout, [&]() { traduzir_botoes_detalhes(self, &msg); });
            timer.start(10);
            msg.exec();
            timer.stop();

            if (msg.clickedButton() == btn_open) {
                abrir_diretorio(self, diretorio_graficos);
            }
        } catch (...) {
            QApplication::restoreOverrideCursor();
            throw;
        }
    } catch (...) {
        QApplication::restoreOverrideCursor();
        source::utils::LogManager::get_logger().error("Erro ao salvar gráficos selecionados", true);
        if (self && self->interface_obj()) {
            QWidget* parent = qobject_cast<QWidget*>(self->interface_obj());
            QMessageBox::critical(
                parent,
                get_text(self->loc_obj(), "error", "Error"),
                get_text(self->loc_obj(), "save_graphs_error", "Erro ao salvar gráficos"));
        }
    }
}

}
