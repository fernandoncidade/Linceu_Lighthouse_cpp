#include "gtab_46_ExecutarRetraducaoAgendada.h"

#include <algorithm>
#include <functional>
#include <QElapsedTimer>
#include <QLabel>
#include <QMetaObject>
#include <QPointer>
#include <QProgressBar>
#include <QTableView>
#include <QTableWidget>
#include <QTimer>
#include <memory>
#include <vector>

#include "source/services/GerenciamentoTabela/gtab_47_EventTableModel.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/services/ob_01_Observador.h"
#include "source/security/secore_04_traduzir_strings_ui.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {
namespace {
constexpr int kRawSecurityValueRole = Qt::UserRole + 100;
constexpr int kRawMetadataValueRole = Qt::UserRole + 101;
constexpr int kMinLinhasPorIteracao = 8;
constexpr int kMaxLinhasPorIteracao = 64;
constexpr int kOrcamentoTempoMs = 6;

source::services::Observador* obter_observador_cpp(QObject* interface_obj) {
    if (interface_obj == nullptr) {
        return nullptr;
    }
    const qulonglong ptr = interface_obj->property("_observador_cpp_ptr").toULongLong();
    if (ptr == 0) {
        return nullptr;
    }
    return reinterpret_cast<source::services::Observador*>(ptr);
}

bool monitoramento_ativo(QObject* interface_obj) {
    const auto* obs_cpp = obter_observador_cpp(interface_obj);
    const bool ativo_cpp = (obs_cpp != nullptr && obs_cpp->ativo.load());
    QObject* obs_qt = interface_obj != nullptr ? interface_obj->property("observador").value<QObject*>() : nullptr;
    const bool ativo_qt = (obs_qt != nullptr && obs_qt->property("ativo").toBool());
    return ativo_cpp || ativo_qt;
}

bool monitoramento_pausado(QObject* interface_obj) {
    const auto* obs_cpp = obter_observador_cpp(interface_obj);
    const bool pausado_cpp = (obs_cpp != nullptr && obs_cpp->pausado.load());
    QObject* obs_qt = interface_obj != nullptr ? interface_obj->property("observador").value<QObject*>() : nullptr;
    const bool pausado_qt = (obs_qt != nullptr && obs_qt->property("pausado").toBool());
    return pausado_cpp || pausado_qt;
}

void set_rotulo_resultado(QObject* interface_obj, const QString& text) {
    if (interface_obj == nullptr) {
        return;
    }
    if (QObject* rotulo_obj = interface_obj->property("rotulo_resultado").value<QObject*>()) {
        if (auto* rotulo = qobject_cast<QLabel*>(rotulo_obj)) {
            rotulo->setText(text);
        } else {
            QMetaObject::invokeMethod(rotulo_obj, "setText", Q_ARG(QString, text));
        }
    }
}

void atualizar_feedback_progresso_traducao(QObject* interface_obj,
                                           source::ui::LocalizadorQt* loc_qt,
                                           int concluido,
                                           int total) {
    if (interface_obj == nullptr || loc_qt == nullptr) {
        return;
    }

    const int total_valido = std::max(1, total);
    int concluido_clamped = concluido;
    if (concluido_clamped < 0) concluido_clamped = 0;
    if (concluido_clamped > total_valido) concluido_clamped = total_valido;
    const int progresso = (concluido_clamped * 100) / total_valido;

    if (QObject* barra_obj = interface_obj->property("barra_progresso").value<QObject*>()) {
        if (auto* barra = qobject_cast<QProgressBar*>(barra_obj)) {
            if (!barra->isVisible()) {
                barra->show();
            }
            barra->setValue(progresso);
        } else {
            QMetaObject::invokeMethod(barra_obj, "show");
            QMetaObject::invokeMethod(barra_obj, "setValue", Q_ARG(int, progresso));
        }
    }

    const QString prefix = loc_qt->get_text("translating_table");
    const QString texto = QStringLiteral("%1 %2% (%3/%4)")
                              .arg(prefix)
                              .arg(progresso)
                              .arg(concluido_clamped)
                              .arg(total_valido);
    set_rotulo_resultado(interface_obj, texto);
}

void limpar_estado_scan_stale(QObject* interface_obj) {
    if (interface_obj == nullptr) {
        return;
    }
    if (!interface_obj->property("_scan_em_execucao").toBool()) {
        return;
    }

    // Estado de scan ativo com barra oculta indica estado antigo preso (ex.: fluxo de tradução).
    QObject* barra_obj = interface_obj->property("barra_progresso").value<QObject*>();
    const auto* barra = qobject_cast<QProgressBar*>(barra_obj);
    if (barra == nullptr || !barra->isVisible()) {
        interface_obj->setProperty("_scan_em_execucao", false);
        interface_obj->setProperty("_scan_contador", 0);
        interface_obj->setProperty("_scan_total", 0);
    }
}

struct RetraducaoEstado {
    QPointer<source::services::GerenciadorTabela> self;
    QPointer<QObject> interface_obj;
    QPointer<QTableWidget> tabela;
    QPointer<source::services::GerenciadorColunas> gc;
    QPointer<source::ui::LocalizadorQt> loc;
    int total_linhas = 0;
    int linha_atual = 0;
    int idx_tipo = -1;
    bool exibir_feedback_traducao = false;
    bool reaplicar_cores_no_final = false;
    int unidades_por_linha = 1;
    std::vector<std::pair<QString, int>> colunas_metadados;
};

void atualizar_progresso_global(const std::shared_ptr<RetraducaoEstado>& estado, int incremento) {
    if (estado == nullptr || estado->interface_obj.isNull() || estado->loc.isNull()) {
        return;
    }

    QObject* iface = estado->interface_obj.data();
    const int total_global = std::max(1, iface->property("_traducao_total_unidades").toInt());
    int concluido = iface->property("_traducao_concluidas_unidades").toInt();
    concluido += incremento;
    if (concluido < 0) concluido = 0;
    if (concluido > total_global) concluido = total_global;
    iface->setProperty("_traducao_concluidas_unidades", concluido);

    if (estado->exibir_feedback_traducao) {
        atualizar_feedback_progresso_traducao(iface, estado->loc.data(), concluido, total_global);
    }
}

void restaurar_rotulo_monitoramento_apos_traducao(QObject* interface_obj,
                                                  source::ui::LocalizadorQt* loc_qt) {
    if (interface_obj == nullptr || loc_qt == nullptr) {
        return;
    }

    QPointer<QObject> interface_guard(interface_obj);
    QPointer<source::ui::LocalizadorQt> loc_guard(loc_qt);
    QTimer::singleShot(3000, interface_obj, [interface_guard, loc_guard]() {
        if (interface_guard.isNull() || loc_guard.isNull()) {
            return;
        }
        if (interface_guard->property("_scan_em_execucao").toBool()) {
            return;
        }

        const QString resultado_chave = interface_guard->property("_resultado_chave").toString().trimmed();
        if (!resultado_chave.isEmpty()) {
            set_rotulo_resultado(interface_guard.data(), loc_guard->get_text(resultado_chave));
            return;
        }

        const bool ativo = monitoramento_ativo(interface_guard.data());
        const bool pausado = monitoramento_pausado(interface_guard.data());
        if (ativo && !pausado) {
            set_rotulo_resultado(interface_guard.data(), loc_guard->get_text("monitoring_started"));
        } else if (ativo && pausado) {
            set_rotulo_resultado(interface_guard.data(), loc_guard->get_text("monitoring_paused"));
        } else {
            set_rotulo_resultado(interface_guard.data(), loc_guard->get_text("monitoring_stopped"));
        }
    });
}

void finalizar_retraducao(const std::shared_ptr<RetraducaoEstado>& estado, bool sucesso) {
    if (estado == nullptr) {
        return;
    }

    auto* self = estado->self.data();
    if (self == nullptr) {
        return;
    }

    if (!estado->tabela.isNull()) {
        auto* tabela = estado->tabela.data();
        tabela->viewport()->setUpdatesEnabled(true);
        tabela->blockSignals(false);
        tabela->viewport()->update();
    }

    if (sucesso && estado->reaplicar_cores_no_final) {
        QPointer<source::services::GerenciadorTabela> self_guard(self);
        QTimer::singleShot(0, self, [self_guard]() {
            if (self_guard.isNull()) {
                return;
            }
            self_guard->_invalidar_cache_cores();
            self_guard->atualizar_cores_colunas(true);
        });
    }

    if (!estado->interface_obj.isNull()) {
        QObject* iface = estado->interface_obj.data();
        const int total_global = std::max(1, iface->property("_traducao_total_unidades").toInt());
        const bool scan_em_execucao = iface->property("_scan_em_execucao").toBool();
        iface->setProperty("_traducao_concluidas_unidades", total_global);

        if (estado->exibir_feedback_traducao && !estado->loc.isNull()) {
            atualizar_feedback_progresso_traducao(iface, estado->loc.data(), total_global, total_global);
            set_rotulo_resultado(iface, estado->loc->get_text("translation_complete"));
            if (!scan_em_execucao) {
                if (QObject* barra = iface->property("barra_progresso").value<QObject*>()) {
                    QMetaObject::invokeMethod(barra, "hide");
                }
            }
            restaurar_rotulo_monitoramento_apos_traducao(iface, estado->loc.data());
        } else if (!scan_em_execucao) {
            if (QObject* barra = iface->property("barra_progresso").value<QObject*>()) {
                QMetaObject::invokeMethod(barra, "hide");
            }
        }

        iface->setProperty("_traducao_em_execucao", false);
        iface->setProperty("_trocando_idioma", false);
        iface->setProperty("_aguardando_conclusao_traducao", false);
        iface->setProperty("_traducao_ui_contabilizada", false);

        limpar_estado_scan_stale(iface);
        if (iface->metaObject()->indexOfMethod("atualizar_status()") >= 0) {
            QMetaObject::invokeMethod(iface, "atualizar_status", Qt::QueuedConnection);
        }
    }

    self->idioma_ultima_retraducao =
        self->loc ? self->loc->property("idioma_atual").toString() : self->idioma_ultima_retraducao;
    self->retraducao_realizada_para_idioma = true;
    self->retraducao_em_andamento = false;
    const bool pendente = self->retraducao_agendada;
    if (pendente && !self->timer_retraducao.isActive()) {
        self->timer_retraducao.start(50);
    } else if (!pendente) {
        self->retraducao_agendada = false;
    }

    if (!sucesso) {
        source::utils::LogManager::get_logger().error("Falha durante pipeline de retradução", true);
    }
}

void processar_lote_retraducao(const std::shared_ptr<RetraducaoEstado>& estado,
                               const std::shared_ptr<std::function<void()>>& continuidade) {
    if (estado == nullptr || continuidade == nullptr ||
        estado->self.isNull() || estado->tabela.isNull() || estado->loc.isNull()) {
        finalizar_retraducao(estado, false);
        return;
    }

    try {
        auto* tabela = estado->tabela.data();
        auto* loc_qt = estado->loc.data();

        int linhas_processadas = 0;
        int row = estado->linha_atual;
        QElapsedTimer timer_lote;
        timer_lote.start();

        while (row < estado->total_linhas) {
            if (estado->idx_tipo >= 0 && estado->idx_tipo < tabela->columnCount()) {
                if (auto* item_tipo = tabela->item(row, estado->idx_tipo)) {
                    const QString atual = item_tipo->text();
                    const QString traduzido =
                        source::ui::GerenciamentoLocalizadorQt::traduzir_tipo_operacao(loc_qt, atual);
                    if (atual != traduzido) {
                        item_tipo->setText(traduzido);
                    }
                }
            }

            for (const auto& col : estado->colunas_metadados) {
                const QString& key = col.first;
                const int indice_coluna = col.second;
                if (indice_coluna < 0 || indice_coluna >= tabela->columnCount()) {
                    continue;
                }
                if (auto* item = tabela->item(row, indice_coluna)) {
                    const bool eh_campo_seguranca = source::security::is_ui_translatable_field(key);
                    const QString atual = item->text();
                    const QString valor_bruto_seguranca =
                        eh_campo_seguranca ? item->data(kRawSecurityValueRole).toString() : QString();
                    const QString valor_bruto_metadado =
                        !eh_campo_seguranca ? item->data(kRawMetadataValueRole).toString() : QString();
                    const QString valor_origem =
                        !valor_bruto_seguranca.isEmpty()
                            ? valor_bruto_seguranca
                            : (!valor_bruto_metadado.isEmpty() ? valor_bruto_metadado : item->text());
                    if (!valor_origem.isEmpty()) {
                        const QString traduzido = eh_campo_seguranca
                                                      ? source::security::traduzir_valor_ui(loc_qt, key, valor_origem)
                                                      : source::ui::GerenciamentoLocalizadorQt::traduzir_metadados(
                                                            loc_qt, valor_origem, key);
                        if (atual != traduzido) {
                            item->setText(traduzido);
                        }
                    }
                }
            }

            ++row;
            ++linhas_processadas;

            const bool atingiu_limite_linhas = (linhas_processadas >= kMaxLinhasPorIteracao);
            const bool atingiu_orcamento_tempo =
                (linhas_processadas >= kMinLinhasPorIteracao && timer_lote.elapsed() >= kOrcamentoTempoMs);
            if (atingiu_limite_linhas || atingiu_orcamento_tempo) {
                break;
            }
        }

        estado->linha_atual = row;
        atualizar_progresso_global(estado, linhas_processadas * estado->unidades_por_linha);

        if (estado->linha_atual < estado->total_linhas) {
            QTimer::singleShot(0, tabela, [continuidade]() { (*continuidade)(); });
            return;
        }

        finalizar_retraducao(estado, true);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao processar lote de retradução", true);
        finalizar_retraducao(estado, false);
    }
}

}  // namespace

void executar_retraducao_agendada(source::services::GerenciadorTabela* self) {
    try {
        if (!self || !self->retraducao_agendada) return;

        const QString idioma_atual = self->loc ? self->loc->property("idioma_atual").toString() : QString();
        if (self->retraducao_realizada_para_idioma && idioma_atual == self->idioma_ultima_retraducao) {
            return;
        }

        self->retraducao_agendada = false;
        self->retraducao_em_andamento = true;

        auto finalizar = [&]() {
            self->idioma_ultima_retraducao = self->loc ? self->loc->property("idioma_atual").toString() : self->idioma_ultima_retraducao;
            self->retraducao_realizada_para_idioma = true;
            self->retraducao_em_andamento = false;
        };

        if (!self->interface) {
            finalizar();
            return;
        }

        QObject* tabela_obj = self->interface->property("tabela_dados").value<QObject*>();
        auto* tabela_view = qobject_cast<QTableView*>(tabela_obj);
        auto* tabela = qobject_cast<QTableWidget*>(tabela_obj);
        auto* gc = qobject_cast<source::services::GerenciadorColunas*>(self->interface->property("gerenciador_colunas").value<QObject*>());
        auto* loc_qt = qobject_cast<source::ui::LocalizadorQt*>(self->loc);
        if (tabela_view != nullptr && loc_qt != nullptr) {
            QMetaObject::invokeMethod(self->interface,
                                      "alternar_tabela_idioma",
                                      Qt::DirectConnection,
                                      Q_ARG(QString, idioma_atual));

            if (auto* model = qobject_cast<source::services::GerenciamentoTabela::EventTableModel*>(tabela_view->model())) {
                model->refresh_headers();
            }

            const int total_global =
                std::max(1, self->interface->property("_traducao_total_unidades").toInt());
            const bool scan_em_execucao = self->interface->property("_scan_em_execucao").toBool();
            self->interface->setProperty("_traducao_concluidas_unidades", total_global);

            atualizar_feedback_progresso_traducao(self->interface, loc_qt, total_global, total_global);
            set_rotulo_resultado(self->interface, loc_qt->get_text("translation_complete"));
            if (!scan_em_execucao) {
                if (QObject* barra = self->interface->property("barra_progresso").value<QObject*>()) {
                    QMetaObject::invokeMethod(barra, "hide");
                }
            }
            restaurar_rotulo_monitoramento_apos_traducao(self->interface, loc_qt);

            self->interface->setProperty("_traducao_em_execucao", false);
            self->interface->setProperty("_trocando_idioma", false);
            self->interface->setProperty("_aguardando_conclusao_traducao", false);
            self->interface->setProperty("_traducao_ui_contabilizada", false);

            limpar_estado_scan_stale(self->interface);
            if (self->interface->metaObject()->indexOfMethod("atualizar_status()") >= 0) {
                QMetaObject::invokeMethod(self->interface, "atualizar_status", Qt::QueuedConnection);
            }

            finalizar();
            return;
        }

        if (!tabela || !gc || !loc_qt) {
            self->interface->setProperty("_traducao_em_execucao", false);
            self->interface->setProperty("_trocando_idioma", false);
            self->interface->setProperty("_aguardando_conclusao_traducao", false);
            finalizar();
            return;
        }

        auto estado = std::make_shared<RetraducaoEstado>();
        estado->self = self;
        estado->interface_obj = self->interface;
        estado->tabela = tabela;
        estado->gc = gc;
        estado->loc = loc_qt;
        estado->total_linhas = tabela->rowCount();
        estado->linha_atual = 0;
        estado->reaplicar_cores_no_final = self->cores_visiveis;

        limpar_estado_scan_stale(self->interface);
        estado->exibir_feedback_traducao = !self->interface->property("_scan_em_execucao").toBool();

        tabela->blockSignals(true);
        tabela->viewport()->setUpdatesEnabled(false);

        try {
            if (estado->exibir_feedback_traducao) {
                set_rotulo_resultado(self->interface, loc_qt->get_text("translating_table"));
            }

            const QVariantMap header_indices = self->_obter_indices_colunas(tabela);
            const QSet<QString> colunas_traduziveis = {
                QStringLiteral("tipo_operacao"),
                QStringLiteral("tipo"),
                QStringLiteral("mime"),
                QStringLiteral("atributos"),
                QStringLiteral("autor"),
                QStringLiteral("protegido"),
                QStringLiteral("dimensoes"),
                QStringLiteral("baseline_status"),
                QStringLiteral("alerta_resumo"),
                QStringLiteral("session_label"),
                QStringLiteral("diff_resumo"),
                QStringLiteral("risk_level"),
                QStringLiteral("quarantine_status"),
                QStringLiteral("actor_process")
            };

            QHash<QString, QString> mapa_colunas;
            for (auto it = gc->COLUNAS_DISPONIVEIS.constBegin(); it != gc->COLUNAS_DISPONIVEIS.constEnd(); ++it) {
                const QString nome_coluna = it.value().value("nome").toString().replace('\n', ' ').trimmed();
                mapa_colunas.insert(nome_coluna, it.key());
            }

            for (auto hi = header_indices.constBegin(); hi != header_indices.constEnd(); ++hi) {
                const QString key = mapa_colunas.value(hi.key());
                const int indice_coluna = hi.value().toInt();
                if (key == QStringLiteral("tipo_operacao")) {
                    estado->idx_tipo = indice_coluna;
                    continue;
                }
                if (colunas_traduziveis.contains(key)) {
                    estado->colunas_metadados.push_back({key, indice_coluna});
                }
            }

            estado->unidades_por_linha = 1 + static_cast<int>(estado->colunas_metadados.size());
            if (estado->unidades_por_linha <= 0) {
                estado->unidades_por_linha = 1;
            }

            const int total_tabela = estado->total_linhas * estado->unidades_por_linha;
            self->interface->setProperty("_traducao_unidades_tabela_total", total_tabela);
            const int total_ui = std::max(1, self->interface->property("_traducao_unidades_ui_total").toInt());
            const int total_global_anterior =
                std::max(1, self->interface->property("_traducao_total_unidades").toInt());
            const int total_global = std::max(total_ui + total_tabela, total_ui);
            self->interface->setProperty("_traducao_total_unidades", total_global);

            int concluido_anterior = self->interface->property("_traducao_concluidas_unidades").toInt();
            if (concluido_anterior < 0) concluido_anterior = 0;
            if (concluido_anterior > total_global_anterior) concluido_anterior = total_global_anterior;

            // Reescala para manter o mesmo percentual ao trocar de total estimado para total real.
            int concluido = static_cast<int>(
                (static_cast<qint64>(concluido_anterior) * static_cast<qint64>(total_global)) /
                static_cast<qint64>(total_global_anterior));
            if (concluido < 0) concluido = 0;
            if (concluido > total_global) concluido = total_global;
            self->interface->setProperty("_traducao_concluidas_unidades", concluido);

            if (estado->exibir_feedback_traducao) {
                atualizar_feedback_progresso_traducao(self->interface, loc_qt, concluido, total_global);
            }

            if (estado->total_linhas <= 0) {
                finalizar_retraducao(estado, true);
                return;
            }

            auto continuidade = std::make_shared<std::function<void()>>();
            *continuidade = [estado, continuidade]() {
                processar_lote_retraducao(estado, continuidade);
            };
            QTimer::singleShot(0, tabela, [continuidade]() { (*continuidade)(); });

            return;
        } catch (...) {
            source::utils::LogManager::get_logger().error("Erro ao preparar retradução assíncrona", true);
            finalizar_retraducao(estado, false);
            return;
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em executar_retraducao_agendada", true);
        if (self) {
            if (self->interface) {
                if (auto* tabela = qobject_cast<QTableWidget*>(self->interface->property("tabela_dados").value<QObject*>())) {
                    tabela->viewport()->setUpdatesEnabled(true);
                    tabela->blockSignals(false);
                    tabela->viewport()->update();
                }
                self->interface->setProperty("_traducao_em_execucao", false);
                self->interface->setProperty("_trocando_idioma", false);
                self->interface->setProperty("_aguardando_conclusao_traducao", false);
            }
            self->idioma_ultima_retraducao =
                self->loc ? self->loc->property("idioma_atual").toString() : self->idioma_ultima_retraducao;
            self->retraducao_realizada_para_idioma = true;
            self->retraducao_em_andamento = false;
        }
    }

}
}  // namespace source::services::GerenciamentoTabela
