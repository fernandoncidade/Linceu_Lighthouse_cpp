#pragma once

#include <deque>
#include <memory>

#include <QAbstractTableModel>
#include <QBrush>
#include <QColor>
#include <QHash>
#include <QPair>
#include <QSet>
#include <QVariantMap>

class QCollator;
class QTableView;

namespace source::services::GerenciamentoTabela {

class FixedLanguageLocalizer;

class EventTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    static constexpr int EventDataRole = Qt::UserRole + 200;

    explicit EventTableModel(QObject* interface_obj,
                             const QString& idioma_exibicao = QString(),
                             QObject* parent = nullptr);
    ~EventTableModel() override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    void prepend_event(const QVariantMap& evento);
    void append_events(const QVariantList& eventos);
    void replace_events(const QVariantList& eventos);
    QVariantList snapshot_events() const;
    QString column_key(int model_column) const;
    int column_index_for_key(const QString& key) const;
    void invalidate_color_cache();
    void notify_coloring_changed(const QSet<QString>& colunas_afetadas = {});
    void bind_view(QTableView* view);
    void refresh_headers();
    QString idioma_exibicao() const;

private:
    void flush_pending_prepend();
    void agendar_flush_pendente(int delay_ms = -1);
    bool modelo_ativo() const;
    bool ordenacao_dinamica_ativa() const;
    int comparar_registros(const QVariantMap& lhs,
                           const QVariantMap& rhs,
                           const QString& key,
                           const QCollator& collator) const;
    QString resolver_chave_operacao(const QString& tipo_valor) const;
    void reconstruir_cache_rotulos_operacao() const;
    void reconstruir_cache_brushes() const;
    int indice_coluna_por_chave(const QString& chave) const;
    void sincronizar_colunas_do_gerenciador();
    QString valor_exibicao_traduzido(const QString& key, const QVariantMap& row_data) const;

    QObject* interface_;
    QString idioma_exibicao_;
    QList<QPair<QString, QVariantMap>> colunas_disponiveis_;
    std::deque<QVariantMap> data_;
    std::deque<QVariantMap> pending_prepend_;
    bool flush_prepend_agendado_;
    QHash<QString, QColor> cores_padrao_operacao_;
    QTableView* tabela_view_;
    std::unique_ptr<FixedLanguageLocalizer> localizer_;
    mutable QHash<QString, QString> cache_tipo_para_operacao_;
    mutable QHash<QString, QString> cache_rotulo_para_operacao_;
    mutable QHash<QString, QBrush> cache_bg_por_operacao_;
    mutable QHash<QString, QBrush> cache_fg_tipo_por_operacao_;
    mutable QHash<QString, QBrush> cache_fg_coluna_por_operacao_;
    mutable QBrush cache_brush_texto_padrao_;
    mutable bool cache_rotulo_inicializado_;
    mutable bool cache_brushes_inicializado_;
    int coluna_ordenacao_ativa_;
    Qt::SortOrder ordem_ordenacao_ativa_;
};

}  // namespace source::services::GerenciamentoTabela
