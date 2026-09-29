#pragma once

#include <QHash>
#include <QString>
#include <QStringList>
#include <tuple>
#include <vector>

#include "source/ui/Manual/common_Manual.h"

namespace source::ui::manual {

// Normaliza o código de idioma para um dos 6 suportados (pt_BR, en_US, es_ES, fr_FR, it_IT, de_DE).
QString normalize_language(const QString& lang = QString());

// Retorna o título traduzido do Manual do Usuário para o idioma informado.
QString get_manual_title(const QString& lang = QString());

// Converte texto alfanumérico para negrito Unicode matemático.
QString to_unicode_bold(const QString& text);

// Obtém a lista de seções completas do manual para o idioma especificado.
std::vector<ManualSection> get_manual_document(const QString& lang = QString());

// Constrói os blocos textuais formatados do manual e a ordem de IDs de seção.
std::tuple<std::vector<ManualBlock>, QStringList> get_manual_blocks(const QString& lang = QString());

// Retorna o texto plano consolidado do manual no idioma especificado.
QString get_manual_text(const QString& lang = QString());

// Retorna o texto do manual acompanhado das posições de caractere de cada seção e da lista ordenada de IDs.
std::tuple<QString, QHash<QString, int>, QStringList> get_manual_text_with_positions(const QString& lang = QString());

}  // namespace source::ui::manual
