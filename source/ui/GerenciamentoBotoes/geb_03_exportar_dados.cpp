#include "geb_03_exportar_dados.h"

#include <algorithm>
#include <exception>
#include <stdexcept>

#include <QAction>
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QMetaObject>
#include <QProcess>
#include <QSet>
#include <QStringConverter>
#include <QTableView>
#include <QTableWidget>
#include <QTemporaryDir>
#include <QTextStream>
#include <QVariant>
#include <QXmlStreamWriter>

#include "geb_04_exportar_para_sqlite.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/ui/ui_01_GerenciadorBotoes.h"
#include "source/utils/LogManager.h"

namespace {
struct LinhaExportacao {
    QStringList ordem_colunas;
    QHash<QString, QString> valores;
};

QString get_text(QObject* loc, const QString& key) {
    QString text;
    if (loc != nullptr) {
        QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, text), Q_ARG(QString, key));
    }
    return text;
}

QString get_text_or(QObject* loc, const QString& key, const QString& fallback) {
    const QString text = get_text(loc, key);
    return (text.isEmpty() || text == key) ? fallback : text;
}

QString format_python_style(QString text, const QStringList& args) {
    for (const QString& arg : args) {
        const int pos = text.indexOf(QStringLiteral("{}"));
        if (pos < 0) {
            break;
        }
        text.replace(pos, 2, arg);
    }
    return text;
}

QObject* get_object_property(QObject* obj, const char* key) {
    if (obj == nullptr) {
        return nullptr;
    }

    const QVariant prop = obj->property(key);
    if (!prop.isValid()) {
        return nullptr;
    }

    QObject* value = prop.value<QObject*>();
    if (value != nullptr) {
        return value;
    }

    if (QAction* action = prop.value<QAction*>()) {
        return action;
    }

    return nullptr;
}

QAction* get_action_property(QObject* obj, const char* key) {
    if (obj == nullptr) {
        return nullptr;
    }

    const QVariant prop = obj->property(key);
    if (!prop.isValid()) {
        return nullptr;
    }

    QAction* action = prop.value<QAction*>();
    if (action == nullptr) {
        action = qobject_cast<QAction*>(prop.value<QObject*>());
    }
    return action;
}

bool is_checked(QObject* obj, const char* key) {
    QAction* action = get_action_property(obj, key);
    return action != nullptr && action->isChecked();
}

QObject* find_gerenciador_colunas_obj(QObject* interface_obj) {
    for (QWidget* widget : QApplication::topLevelWidgets()) {
        if (widget == nullptr) {
            continue;
        }

        QObject* gc = get_object_property(widget, "gerenciador_colunas");
        if (gc != nullptr) {
            return gc;
        }

        QObject* menus = get_object_property(widget, "gerenciador_menus_ui");
        if (menus != nullptr) {
            gc = get_object_property(menus, "gerenciador_colunas");
            if (gc != nullptr) {
                return gc;
            }
        }
    }

    if (interface_obj != nullptr) {
        QObject* menus = get_object_property(interface_obj, "gerenciador_menus_ui");
        if (menus != nullptr) {
            QObject* gc = get_object_property(menus, "gerenciador_colunas");
            if (gc != nullptr) {
                return gc;
            }
        }
        return get_object_property(interface_obj, "gerenciador_colunas");
    }

    return nullptr;
}

void carregar_opcoes_exportacao(QObject* interface_obj,
                                bool& apenas_colunas_ativas,
                                bool& apenas_filtros_ativos,
                                bool& apenas_selecao) {
    for (QWidget* widget : QApplication::topLevelWidgets()) {
        if (widget == nullptr) {
            continue;
        }

        QObject* gerenciador_menus_ui = get_object_property(widget, "gerenciador_menus_ui");
        if (gerenciador_menus_ui != nullptr) {
            apenas_colunas_ativas = is_checked(gerenciador_menus_ui, "acao_exportar_colunas_ativas");
            apenas_filtros_ativos = is_checked(gerenciador_menus_ui, "acao_exportar_filtros_ativos");
            apenas_selecao = is_checked(gerenciador_menus_ui, "acao_exportar_selecao");
            return;
        }

        const bool has_any =
            widget->property("acao_exportar_colunas_ativas").isValid() ||
            widget->property("acao_exportar_filtros_ativos").isValid() ||
            widget->property("acao_exportar_selecao").isValid();

        if (!has_any) {
            continue;
        }

        apenas_colunas_ativas = is_checked(widget, "acao_exportar_colunas_ativas");
        apenas_filtros_ativos = is_checked(widget, "acao_exportar_filtros_ativos");
        apenas_selecao = is_checked(widget, "acao_exportar_selecao");
        return;
    }

    QObject* gerenciador_menus_ui = get_object_property(interface_obj, "gerenciador_menus_ui");
    if (gerenciador_menus_ui != nullptr) {
        apenas_colunas_ativas = is_checked(gerenciador_menus_ui, "acao_exportar_colunas_ativas");
        apenas_filtros_ativos = is_checked(gerenciador_menus_ui, "acao_exportar_filtros_ativos");
        apenas_selecao = is_checked(gerenciador_menus_ui, "acao_exportar_selecao");
        return;
    }

    apenas_colunas_ativas = is_checked(interface_obj, "acao_exportar_colunas_ativas");
    apenas_filtros_ativos = is_checked(interface_obj, "acao_exportar_filtros_ativos");
    apenas_selecao = is_checked(interface_obj, "acao_exportar_selecao");
}

QString csv_escape(QString value, const QString& sep) {
    value.replace("\"", "\"\"");
    if (value.contains(sep) || value.contains('\n') || value.contains('\r') || value.contains('"')) {
        value = QString("\"%1\"").arg(value);
    }
    return value;
}

QStringList construir_headers_tabulares(const QList<LinhaExportacao>& dados) {
    QStringList headers;
    QSet<QString> vistos;

    for (const LinhaExportacao& linha : dados) {
        for (const QString& coluna : linha.ordem_colunas) {
            if (!vistos.contains(coluna)) {
                vistos.insert(coluna);
                headers.append(coluna);
            }
        }
    }

    return headers;
}

QList<QMap<QString, QString>> converter_para_sqlite(const QList<LinhaExportacao>& dados) {
    QList<QMap<QString, QString>> convertido;
    convertido.reserve(dados.size());

    for (const LinhaExportacao& linha : dados) {
        QMap<QString, QString> row;
        for (const QString& coluna : linha.ordem_colunas) {
            row.insert(coluna, linha.valores.value(coluna));
        }
        convertido.append(row);
    }

    return convertido;
}

QJsonObject converter_linha_para_json(const LinhaExportacao& linha) {
    QJsonObject obj;
    for (const QString& coluna : linha.ordem_colunas) {
        obj.insert(coluna, linha.valores.value(coluna));
    }
    return obj;
}

void exportar_delimitado(const QList<LinhaExportacao>& dados,
                         const QStringList& headers,
                         const QString& nome_arquivo,
                         const QString& sep) {
    QFile f(nome_arquivo);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        throw std::runtime_error(QString("Falha ao abrir arquivo para escrita: %1").arg(nome_arquivo).toStdString());
    }

    QTextStream out(&f);
    out.setEncoding(QStringConverter::Utf8);

    if (!headers.isEmpty()) {
        QStringList escaped_headers;
        for (const QString& h : headers) {
            escaped_headers << csv_escape(h, sep);
        }
        out << escaped_headers.join(sep) << Qt::endl;

        for (const LinhaExportacao& row : dados) {
            QStringList values;
            for (const auto& h : headers) {
                values << csv_escape(row.valores.value(h), sep);
            }
            out << values.join(sep) << Qt::endl;
        }
    }

    f.close();
}

QString excel_column_name(int column_index_zero_based) {
    int value = column_index_zero_based + 1;
    QString column;
    while (value > 0) {
        const int rem = (value - 1) % 26;
        column.prepend(QChar('A' + rem));
        value = (value - 1) / 26;
    }
    return column;
}

void escrever_texto_arquivo(const QString& caminho, const QString& conteudo) {
    QFile arquivo(caminho);
    if (!arquivo.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        throw std::runtime_error(
            QString("Falha ao escrever arquivo temporário XLSX: %1").arg(caminho).toStdString());
    }

    QTextStream stream(&arquivo);
    stream.setEncoding(QStringConverter::Utf8);
    stream << conteudo;
    arquivo.close();
}

void escrever_sheet_xml(const QString& caminho,
                        const QList<LinhaExportacao>& dados,
                        const QStringList& headers) {
    QFile arquivo(caminho);
    if (!arquivo.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        throw std::runtime_error(
            QString("Falha ao escrever planilha XLSX temporária: %1").arg(caminho).toStdString());
    }

    QXmlStreamWriter xml(&arquivo);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement("worksheet");
    xml.writeDefaultNamespace("http://schemas.openxmlformats.org/spreadsheetml/2006/main");
    xml.writeStartElement("sheetData");

    int row_number = 1;
    if (!headers.isEmpty()) {
        xml.writeStartElement("row");
        xml.writeAttribute("r", QString::number(row_number));
        for (int col = 0; col < headers.size(); ++col) {
            xml.writeStartElement("c");
            xml.writeAttribute("r", excel_column_name(col) + QString::number(row_number));
            xml.writeAttribute("t", "inlineStr");
            xml.writeStartElement("is");
            xml.writeStartElement("t");
            xml.writeAttribute("xml:space", "preserve");
            xml.writeCharacters(headers.at(col));
            xml.writeEndElement();  // t
            xml.writeEndElement();  // is
            xml.writeEndElement();  // c
        }
        xml.writeEndElement();  // row
        ++row_number;
    }

    for (const LinhaExportacao& linha : dados) {
        xml.writeStartElement("row");
        xml.writeAttribute("r", QString::number(row_number));

        for (int col = 0; col < headers.size(); ++col) {
            const QString& header = headers.at(col);
            xml.writeStartElement("c");
            xml.writeAttribute("r", excel_column_name(col) + QString::number(row_number));
            xml.writeAttribute("t", "inlineStr");
            xml.writeStartElement("is");
            xml.writeStartElement("t");
            xml.writeAttribute("xml:space", "preserve");
            xml.writeCharacters(linha.valores.value(header));
            xml.writeEndElement();  // t
            xml.writeEndElement();  // is
            xml.writeEndElement();  // c
        }

        xml.writeEndElement();  // row
        ++row_number;
    }

    xml.writeEndElement();  // sheetData
    xml.writeEndElement();  // worksheet
    xml.writeEndDocument();
    arquivo.close();
}

QString ps_quote(const QString& value) {
    QString escaped = value;
    escaped.replace("'", "''");
    return QString("'%1'").arg(escaped);
}

void exportar_xlsx(const QList<LinhaExportacao>& dados,
                   const QStringList& headers,
                   const QString& nome_arquivo) {
    QTemporaryDir dir;
    if (!dir.isValid()) {
        throw std::runtime_error("Falha ao criar diretório temporário para XLSX");
    }

    const QString root = dir.path();
    QDir qdir(root);
    qdir.mkpath("_rels");
    qdir.mkpath("xl/_rels");
    qdir.mkpath("xl/worksheets");

    escrever_texto_arquivo(
        root + "/[Content_Types].xml",
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
        "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">\n"
        "  <Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>\n"
        "  <Default Extension=\"xml\" ContentType=\"application/xml\"/>\n"
        "  <Override PartName=\"/xl/workbook.xml\" "
        "ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml\"/>\n"
        "  <Override PartName=\"/xl/worksheets/sheet1.xml\" "
        "ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml\"/>\n"
        "</Types>\n");

    escrever_texto_arquivo(
        root + "/_rels/.rels",
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
        "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">\n"
        "  <Relationship Id=\"rId1\" "
        "Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" "
        "Target=\"xl/workbook.xml\"/>\n"
        "</Relationships>\n");

    escrever_texto_arquivo(
        root + "/xl/workbook.xml",
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
        "<workbook xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" "
        "xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\">\n"
        "  <sheets>\n"
        "    <sheet name=\"Sheet1\" sheetId=\"1\" r:id=\"rId1\"/>\n"
        "  </sheets>\n"
        "</workbook>\n");

    escrever_texto_arquivo(
        root + "/xl/_rels/workbook.xml.rels",
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
        "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">\n"
        "  <Relationship Id=\"rId1\" "
        "Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet\" "
        "Target=\"worksheets/sheet1.xml\"/>\n"
        "</Relationships>\n");

    escrever_sheet_xml(root + "/xl/worksheets/sheet1.xml", dados, headers);

    QFile::remove(nome_arquivo);

#ifdef Q_OS_WIN
    const QString zip_temporario = root + ".zip";
    QFile::remove(zip_temporario);

    const QString script = QString("$ErrorActionPreference = 'Stop'; "
                                   "Compress-Archive -Path %1 -DestinationPath %2 -Force")
                               .arg(ps_quote(root + "/*"), ps_quote(zip_temporario));
    QProcess process;
    process.start("powershell", {"-NoProfile", "-Command", script});
    if (!process.waitForFinished(120000)) {
        process.kill();
        QFile::remove(zip_temporario);
        throw std::runtime_error("Falha ao finalizar processo de compactação XLSX");
    }

    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        const QString erro = QString::fromUtf8(process.readAllStandardError()).trimmed();
        QFile::remove(zip_temporario);
        throw std::runtime_error(
            QString("Falha ao gerar XLSX: %1").arg(erro.isEmpty() ? "erro desconhecido" : erro).toStdString());
    }

    if (!QFile::copy(zip_temporario, nome_arquivo)) {
        QFile::remove(zip_temporario);
        throw std::runtime_error(
            QString("Falha ao mover XLSX gerado para: %1").arg(nome_arquivo).toStdString());
    }

    QFile::remove(zip_temporario);
#else
    Q_UNUSED(nome_arquivo)
    throw std::runtime_error("Exportação XLSX indisponível nesta plataforma");
#endif
}
}  // namespace

namespace source::ui::GerenciamentoBotoes {

bool exportar_dados(source::ui::GerenciadorBotoes* self, void* tabela_dados, const QString& formato,
                    const QString& nome_arquivo_in) {
    try {
        if (self == nullptr) {
            return false;
        }

        QObject* interface_obj = self->interface_obj();
        QObject* loc = self->loc_obj();

        // Resolve table: may be QTableWidget or QTableView with model
        auto* table_obj = static_cast<QObject*>(tabela_dados);
        auto* table_widget = qobject_cast<QTableWidget*>(table_obj);
        auto* table_view = table_widget ? nullptr : qobject_cast<QTableView*>(table_obj);
        QAbstractItemModel* model = table_view ? table_view->model() : nullptr;

        if (!table_widget && (!table_view || !model)) {
            QMessageBox::warning(nullptr, get_text(loc, "warning"), get_text(loc, "no_data"));
            return false;
        }

        // Lambda accessors that work with both widget types
        const int row_count = table_widget ? table_widget->rowCount() : model->rowCount();
        const int col_count = table_widget ? table_widget->columnCount() : model->columnCount();

        if (row_count == 0) {
            QMessageBox::warning(nullptr, get_text(loc, "warning"), get_text(loc, "no_data"));
            return false;
        }

        auto header_text = [&](int col) -> QString {
            if (table_widget) {
                auto* item = table_widget->horizontalHeaderItem(col);
                return item ? item->text() : QString();
            }
            return model->headerData(col, Qt::Horizontal).toString();
        };

        auto cell_text = [&](int row, int col) -> QString {
            if (table_widget) {
                auto* item = table_widget->item(row, col);
                return item ? item->text() : QString();
            }
            return model->data(model->index(row, col)).toString();
        };

        auto is_row_hidden = [&](int row) -> bool {
            if (table_widget) return table_widget->isRowHidden(row);
            if (table_view) return table_view->isRowHidden(row);
            return false;
        };

        auto is_column_hidden = [&](int col) -> bool {
            if (table_widget) return table_widget->isColumnHidden(col);
            if (table_view) return table_view->isColumnHidden(col);
            return false;
        };

        auto selection_model = [&]() -> QItemSelectionModel* {
            if (table_widget) return table_widget->selectionModel();
            if (table_view) return table_view->selectionModel();
            return nullptr;
        };

        bool apenas_colunas_ativas = false;
        bool apenas_filtros_ativos = false;
        bool apenas_selecao = false;
        carregar_opcoes_exportacao(interface_obj, apenas_colunas_ativas, apenas_filtros_ativos, apenas_selecao);

        QList<int> colunas_para_exportar;
        QSet<int> colunas_selecionadas;
        if (auto* sel_model = selection_model(); sel_model != nullptr) {
            const QModelIndexList selected_columns = sel_model->selectedColumns();
            if (!selected_columns.isEmpty()) {
                for (const QModelIndex& idx : selected_columns) {
                    if (idx.column() >= 0) {
                        colunas_selecionadas.insert(idx.column());
                    }
                }
            } else {
                const QModelIndexList selected_indexes = sel_model->selectedIndexes();
                for (const QModelIndex& idx : selected_indexes) {
                    if (idx.column() >= 0) {
                        colunas_selecionadas.insert(idx.column());
                    }
                }
            }
        }

        source::services::GerenciadorColunas* gerenciador_colunas = nullptr;
        QObject* gc_obj = find_gerenciador_colunas_obj(interface_obj);
        if (gc_obj != nullptr) {
            gerenciador_colunas = qobject_cast<source::services::GerenciadorColunas*>(gc_obj);
        }

        if (apenas_colunas_ativas) {
            if (!colunas_selecionadas.isEmpty()) {
                QList<int> cols_sorted = colunas_selecionadas.values();
                std::sort(cols_sorted.begin(), cols_sorted.end());
                for (const int col : cols_sorted) {
                    if (col >= 0 && col < col_count) {
                        colunas_para_exportar.append(col);
                    }
                }
            } else if (gerenciador_colunas != nullptr) {
                QSet<QString> headers_visiveis;
                for (auto it = gerenciador_colunas->COLUNAS_DISPONIVEIS.constBegin();
                     it != gerenciador_colunas->COLUNAS_DISPONIVEIS.constEnd(); ++it) {
                    const QVariantMap coluna = it.value();
                    if (coluna.value("visivel").toBool()) {
                        headers_visiveis.insert(coluna.value("nome").toString());
                    }
                }

                for (int col = 0; col < col_count; ++col) {
                    const QString hdr = header_text(col);
                    if (!hdr.isEmpty() && headers_visiveis.contains(hdr)) {
                        colunas_para_exportar.append(col);
                    }
                }
            } else {
                for (int col = 0; col < col_count; ++col) {
                    if (!is_column_hidden(col)) {
                        colunas_para_exportar.append(col);
                    }
                }
            }
        } else {
            for (int col = 0; col < col_count; ++col) {
                colunas_para_exportar.append(col);
            }
        }

        QList<LinhaExportacao> dados;
        if (apenas_selecao) {
            auto* sel_model = selection_model();
            const QModelIndexList selecao =
                (sel_model != nullptr) ? sel_model->selectedIndexes()
                                       : QModelIndexList();
            if (selecao.isEmpty()) {
                QString mensagem = get_text(loc, "no_selection");
                if (mensagem.isEmpty() || mensagem == "no_selection") {
                    mensagem = QStringLiteral("Nenhuma célula selecionada para exportação.");
                }

                QMessageBox::warning(nullptr, get_text(loc, "warning"), mensagem);
                return false;
            }

            QMap<int, QSet<int>> selecao_mapa;
            for (const auto& indice : selecao) {
                const int row = indice.row();
                const int col = indice.column();
                if (apenas_colunas_ativas && !colunas_para_exportar.contains(col)) {
                    continue;
                }
                selecao_mapa[row].insert(col);
            }

            for (auto it = selecao_mapa.constBegin(); it != selecao_mapa.constEnd(); ++it) {
                const int row = it.key();
                if (apenas_filtros_ativos && is_row_hidden(row)) {
                    continue;
                }

                const QSet<int>& cols = it.value();
                LinhaExportacao linha;

                for (int col = 0; col < col_count; ++col) {
                    const QString hdr = header_text(col);
                    if (!hdr.isEmpty() && cols.contains(col)) {
                        linha.ordem_colunas.append(hdr);
                        linha.valores.insert(hdr, cell_text(row, col));
                    }
                }

                if (!linha.ordem_colunas.isEmpty()) {
                    dados.append(linha);
                }
            }
        } else {
            QList<int> linhas_para_exportar;
            if (apenas_filtros_ativos) {
                for (int row = 0; row < row_count; ++row) {
                    if (!is_row_hidden(row)) {
                        linhas_para_exportar.append(row);
                    }
                }
            } else {
                for (int row = 0; row < row_count; ++row) {
                    linhas_para_exportar.append(row);
                }
            }

            for (const int row : linhas_para_exportar) {
                LinhaExportacao linha;
                for (const int col : colunas_para_exportar) {
                    const QString hdr = header_text(col);
                    if (hdr.isEmpty()) {
                        continue;
                    }

                    linha.ordem_colunas.append(hdr);
                    linha.valores.insert(hdr, cell_text(row, col));
                }
                dados.append(linha);
            }
        }

        const QStringList headers_tabulares = construir_headers_tabulares(dados);

        try {
            QString nome_arquivo = nome_arquivo_in;
            if (nome_arquivo.isEmpty()) {
                if (formato == "xlsx" || formato == "csv" || formato == "txt") {
                    nome_arquivo = QFileDialog::getSaveFileName(
                        nullptr, QString("Exportar para %1").arg(formato.toUpper()), QString(),
                        QString("%1 Files (*.%2)").arg(formato.toUpper(), formato));
                } else if (formato == "json") {
                    nome_arquivo = QFileDialog::getSaveFileName(nullptr, "Exportar para JSON", QString(),
                                                                "JSON Files (*.json)");
                } else if (formato == "xml") {
                    nome_arquivo = QFileDialog::getSaveFileName(nullptr, "Exportar para XML", QString(),
                                                                "XML Files (*.xml)");
                } else if (formato == "db") {
                    nome_arquivo = QFileDialog::getSaveFileName(nullptr, "Exportar para SQLite", QString(),
                                                                "DB Files (*.db)");
                } else if (formato == "evidence") {
                    nome_arquivo = QFileDialog::getExistingDirectory(
                        nullptr,
                        get_text_or(loc,
                                    "security_export_evidence_package",
                                    "Exportar pacote de evidência"));
                }
            }

            if (nome_arquivo.isEmpty()) {
                return false;
            }

            try {
                if (formato == "xlsx") {
                    exportar_xlsx(dados, headers_tabulares, nome_arquivo);
                } else if (formato == "csv") {
                    exportar_delimitado(dados, headers_tabulares, nome_arquivo, ",");
                } else if (formato == "txt") {
                    exportar_delimitado(dados, headers_tabulares, nome_arquivo, "\t");
                } else if (formato == "json") {
                    QJsonArray arr;
                    for (const LinhaExportacao& row : dados) {
                        arr.append(converter_linha_para_json(row));
                    }

                    QFile f(nome_arquivo);
                    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                        throw std::runtime_error(
                            QString("Falha ao abrir arquivo JSON: %1").arg(nome_arquivo).toStdString());
                    }
                    f.write(QJsonDocument(arr).toJson(QJsonDocument::Indented));
                    f.close();
                } else if (formato == "xml") {
                    QFile f(nome_arquivo);
                    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                        throw std::runtime_error(
                            QString("Falha ao abrir arquivo XML: %1").arg(nome_arquivo).toStdString());
                    }

                    QXmlStreamWriter xml(&f);
                    xml.setAutoFormatting(true);
                    xml.writeStartDocument();
                    xml.writeStartElement("eventos");

                    for (const LinhaExportacao& row : dados) {
                        xml.writeStartElement("evento");
                        for (const QString& coluna : row.ordem_colunas) {
                            const QString key = QString(coluna).toLower().replace(" ", "_");
                            xml.writeTextElement(key, row.valores.value(coluna));
                        }
                        xml.writeEndElement();
                    }

                    xml.writeEndElement();
                    xml.writeEndDocument();
                    f.close();
                } else if (formato == "db") {
                    const QList<QMap<QString, QString>> dados_sqlite = converter_para_sqlite(dados);
                    exportar_para_sqlite(self, dados_sqlite, nome_arquivo, headers_tabulares);
                } else if (formato == "evidence") {
                    QString pacote_gerado;
                    if (interface_obj != nullptr) {
                        QMetaObject::invokeMethod(interface_obj,
                                                  "exportar_pacote_evidencia",
                                                  Q_RETURN_ARG(QString, pacote_gerado),
                                                  Q_ARG(QString, nome_arquivo));
                    }
                    if (pacote_gerado.isEmpty()) {
                        throw std::runtime_error("Falha ao gerar pacote de evidência");
                    }
                } else {
                    return false;
                }
                return true;
            } catch (const std::exception& e) {
                const QString detalhes = QString::fromUtf8(e.what());
                source::utils::LogManager::get_logger().error(
                    QString("Erro ao exportar arquivo (%1): %2").arg(formato, detalhes), true);

                const QString mensagem = format_python_style(
                    get_text_or(loc, "export_error_details",
                                "Erro ao exportar para {}: {}\\n\\nDetalhes: {}"),
                    {formato, detalhes, detalhes});
                QMessageBox::critical(nullptr,
                                      get_text_or(loc, "export_error", "Erro de Exportação"),
                                      mensagem);
                return false;
            } catch (...) {
                source::utils::LogManager::get_logger().error(
                    QString("Erro ao exportar arquivo (%1)").arg(formato), true);

                const QString mensagem = format_python_style(
                    get_text_or(loc, "export_error_details",
                                "Erro ao exportar para {}: {}\\n\\nDetalhes: {}"),
                    {formato, "Erro desconhecido", "Erro desconhecido"});
                QMessageBox::critical(nullptr,
                                      get_text_or(loc, "export_error", "Erro de Exportação"),
                                      mensagem);
                return false;
            }
        } catch (const std::exception& e) {
            const QString detalhes = QString::fromUtf8(e.what());
            source::utils::LogManager::get_logger().error(
                QString("Erro na preparação para exportação: %1").arg(detalhes), true);

            const QString mensagem = format_python_style(
                get_text_or(loc, "export_preparation_error",
                            "Erro ao preparar exportação: {}\\n\\nDetalhes: {}"),
                {detalhes, detalhes});
            QMessageBox::critical(nullptr, get_text_or(loc, "error", "Erro"), mensagem);
            return false;
        } catch (...) {
            source::utils::LogManager::get_logger().error("Erro na preparação para exportação", true);

            const QString mensagem = format_python_style(
                get_text_or(loc, "export_preparation_error",
                            "Erro ao preparar exportação: {}\\n\\nDetalhes: {}"),
                {"Erro desconhecido", "Erro desconhecido"});
            QMessageBox::critical(nullptr, get_text_or(loc, "error", "Erro"), mensagem);
            return false;
        }
    } catch (const std::exception& e) {
        source::utils::LogManager::get_logger().error(
            QString("Erro ao exportar dados: %1").arg(QString::fromUtf8(e.what())), true);
        return false;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao exportar dados", true);
        return false;
    }
}

}  // namespace source::ui::GerenciamentoBotoes
