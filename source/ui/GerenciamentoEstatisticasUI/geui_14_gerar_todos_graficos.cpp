#include "geui_14_gerar_todos_graficos.h"

#include <QApplication>
#include <QColor>
#include <QCursor>
#include <QDateTime>
#include <QDialog>
#include <QEventLoop>
#include <QFont>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLinearGradient>
#include <QMessageBox>
#include <QMetaObject>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPainterPathStroker>
#include <QPaintEvent>
#include <QPen>
#include <QProgressDialog>
#include <QScrollArea>
#include <QSignalBlocker>
#include <QSet>
#include <QTabWidget>
#include <QTableWidget>
#include <QTimer>
#include <QToolTip>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QVariantHash>
#include <QVariantMap>
#include <QWidget>

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>

#include "geui_31_worker_grafico.h"
#include "source/data/Charts/ch_02_ChartWidgetFactory.h"
#include "source/data/Charts/ch_08_RasterChartView.h"
#include "source/data/st_01_GeradorEstatisticas.h"
#include "source/ui/ui_06_GerenciadorEstatisticasUI.h"
#include "source/utils/LogManager.h"

namespace {
constexpr auto kStatsGraphDataByLanguageProperty = "_estatisticas_graficos_dados_por_idioma";
constexpr auto kStatsActiveLanguageProperty = "_estatisticas_idioma_ativo";
constexpr auto kStatsGenerationIdProperty = "_estatisticas_generation_id";
constexpr auto kStatsDbPathProperty = "_estatisticas_db_path";
constexpr auto kStatsPendingChartFigureProperty = "_estatisticas_pending_chart_figure";
constexpr auto kStatsPendingChartTitleProperty = "_estatisticas_pending_chart_title";
constexpr auto kStatsPendingChartLoadedProperty = "_estatisticas_pending_chart_loaded";
constexpr auto kStatsTabMaterializerConnectedProperty = "_estatisticas_tab_materializer_connected";
constexpr auto kStatsPreparationFinishedProperty = "_estatisticas_preparacao_concluida";

QString get_text(QObject* loc, const QString& key, const QString& fallback = QString()) {
    QString text = fallback;
    if (loc != nullptr) {
        QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, text), Q_ARG(QString, key), Q_ARG(QString, fallback));
    }
    return text;
}

QObject*& localization_context_obj() {
    static thread_local QObject* loc = nullptr;
    return loc;
}

void set_localization_context(QObject* loc) {
    if (loc != nullptr) {
        localization_context_obj() = loc;
    }
}

QString obter_idioma_estatisticas(QObject* iface, QObject* loc = nullptr) {
    QString idioma;
    if (iface != nullptr) {
        idioma = iface->property(kStatsActiveLanguageProperty).toString();
    }
    if (idioma.isEmpty() && loc != nullptr) {
        idioma = loc->property("idioma_atual").toString();
    }
    return idioma.isEmpty() ? QStringLiteral("pt_BR") : idioma;
}

void sincronizar_graficos_idioma_ativo(QObject* iface, QObject* loc = nullptr) {
    if (iface == nullptr) return;

    const QString idioma = obter_idioma_estatisticas(iface, loc);
    QVariantHash graficos_por_idioma = iface->property(kStatsGraphDataByLanguageProperty).toHash();
    graficos_por_idioma.insert(idioma, iface->property("graficos_dados").toHash());
    iface->setProperty(kStatsGraphDataByLanguageProperty, graficos_por_idioma);
}

QString ui_text(const QString& key, const QString& fallback = QString()) {
    return get_text(localization_context_obj(), key, fallback);
}

QString ui_no_data_text() {
    return ui_text("no_data_display", QStringLiteral("Sem dados para exibir."));
}

qulonglong generation_id_atual(QObject* iface) {
    if (iface == nullptr) {
        return 0;
    }
    return iface->property(kStatsGenerationIdProperty).toULongLong();
}

qulonglong avancar_generation_id(QObject* iface) {
    if (iface == nullptr) {
        return 0;
    }

    qulonglong next = generation_id_atual(iface) + 1;
    if (next == 0) {
        next = 1;
    }
    iface->setProperty(kStatsGenerationIdProperty, QVariant::fromValue<qulonglong>(next));
    return next;
}

bool generation_id_valido(QObject* iface, qulonglong generation_id) {
    return iface != nullptr && generation_id != 0 && generation_id_atual(iface) == generation_id;
}

void liberar_worker_estatisticas(QObject* iface) {
    if (iface == nullptr) {
        return;
    }

    auto* worker = iface->property("worker").value<source::ui::GerenciamentoEstatisticasUI::GraficoWorker*>();
    if (worker == nullptr) {
        return;
    }

    QObject::disconnect(worker, nullptr, nullptr, nullptr);
    worker->parar();

    if (worker->isRunning()) {
        if (!worker->wait(3000)) {
            source::utils::LogManager::get_logger().warning(
                QStringLiteral("Worker de estatísticas ainda estava ativo; aguardando término assíncrono."));
            worker->setParent(nullptr);
            QObject::connect(worker, &QThread::finished, worker, &QObject::deleteLater, Qt::UniqueConnection);
            iface->setProperty("worker", QVariant());
            return;
        }
    }

    worker->deleteLater();
    iface->setProperty("worker", QVariant());
}

bool& export_render_mode_flag() {
    static thread_local bool export_mode = false;
    return export_mode;
}

bool is_export_render_mode() {
    return export_render_mode_flag();
}

int estimate_variant_complexity(const QVariant& value, int depth = 0) {
    if (!value.isValid() || depth > 5) return 0;

    const int type_id =
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        value.typeId();
#else
        value.type();
#endif

    switch (type_id) {
        case QMetaType::QVariantList: {
            const QVariantList list = value.toList();
            int score = list.size();
            const int sample_count = qMin(list.size(), 24);
            for (int i = 0; i < sample_count; ++i) {
                score += estimate_variant_complexity(list.at(i), depth + 1);
            }
            if (list.size() > sample_count && sample_count > 0) {
                score += ((score / sample_count) * (list.size() - sample_count));
            }
            return score;
        }
        case QMetaType::QVariantMap: {
            const QVariantMap map = value.toMap();
            int score = map.size() * 2;
            for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
                score += estimate_variant_complexity(it.value(), depth + 1);
            }
            return score;
        }
        case QMetaType::QVariantHash: {
            const QVariantHash hash = value.toHash();
            int score = hash.size() * 2;
            for (auto it = hash.constBegin(); it != hash.constEnd(); ++it) {
                score += estimate_variant_complexity(it.value(), depth + 1);
            }
            return score;
        }
        default:
            return 1;
    }
}

int estimate_figure_complexity(const QVariantMap& fig_map) {
    int score = 0;
    for (auto it = fig_map.constBegin(); it != fig_map.constEnd(); ++it) {
        score += 1 + estimate_variant_complexity(it.value());
    }
    return score;
}

class ScopedExportRenderMode final {
public:
    explicit ScopedExportRenderMode(bool enabled)
        : previous_(export_render_mode_flag()) {
        export_render_mode_flag() = enabled;
    }

    ~ScopedExportRenderMode() {
        export_render_mode_flag() = previous_;
    }

private:
    bool previous_ = false;
};

QVector<QPointF> construir_curva_monotona(const QVector<QPointF>& pontos, int amostras_por_segmento = 26) {
    const int n = pontos.size();
    if (n <= 2) return pontos;

    const int samples = qMax(2, amostras_por_segmento);
    QVector<double> x(n), y(n);
    for (int i = 0; i < n; ++i) {
        x[i] = pontos.at(i).x();
        y[i] = pontos.at(i).y();
    }

    QVector<double> h(n - 1), d(n - 1);
    for (int i = 0; i < n - 1; ++i) {
        h[i] = x[i + 1] - x[i];
        if (h[i] <= std::numeric_limits<double>::epsilon()) h[i] = 1.0;
        d[i] = (y[i + 1] - y[i]) / h[i];
    }

    QVector<double> m(n);
    m[0] = d[0];
    m[n - 1] = d[n - 2];
    for (int i = 1; i < n - 1; ++i) {
        m[i] = 0.5 * (d[i - 1] + d[i]);
    }

    for (int i = 0; i < n - 1; ++i) {
        if (std::abs(d[i]) <= std::numeric_limits<double>::epsilon()) {
            m[i] = 0.0;
            m[i + 1] = 0.0;
            continue;
        }
        const double a = m[i] / d[i];
        const double b = m[i + 1] / d[i];
        const double soma = (a * a) + (b * b);
        if (soma > 9.0) {
            const double t = 3.0 / std::sqrt(soma);
            m[i] = t * a * d[i];
            m[i + 1] = t * b * d[i];
        }
    }

    QVector<QPointF> curva;
    curva.reserve((n - 1) * (samples - 1) + 1);

    for (int i = 0; i < n - 1; ++i) {
        const double xi = x[i];
        const double hi = h[i];
        const double yi = y[i];
        const double yi1 = y[i + 1];
        const double mi = m[i];
        const double mi1 = m[i + 1];

        for (int s = 0; s < samples; ++s) {
            if (i > 0 && s == 0) continue;
            const double t = static_cast<double>(s) / static_cast<double>(samples - 1);
            const double t2 = t * t;
            const double t3 = t2 * t;

            const double h00 = (2.0 * t3) - (3.0 * t2) + 1.0;
            const double h10 = t3 - (2.0 * t2) + t;
            const double h01 = (-2.0 * t3) + (3.0 * t2);
            const double h11 = t3 - t2;

            const double xv = xi + (t * hi);
            const double yv = (h00 * yi) + (h10 * hi * mi) + (h01 * yi1) + (h11 * hi * mi1);
            curva.append(QPointF(xv, yv));
        }
    }

    return curva;
}

struct TreemapItem {
    QString label;
    double area;
    double value;
    QColor color;
};

struct TreemapRect {
    QRectF rect;
    TreemapItem item;
};

double somar_areas_treemap(const QList<TreemapItem>& itens) {
    double total = 0.0;
    for (const TreemapItem& item : itens) total += qMax(0.0, item.area);
    return total;
}

void calcular_layout_treemap_recursivo(const QList<TreemapItem>& itens, const QRectF& area_total,
                                       QList<TreemapRect>& resultado) {
    if (itens.isEmpty() || area_total.width() <= 0.0 || area_total.height() <= 0.0) return;
    if (itens.size() == 1) {
        resultado.append({area_total, itens.first()});
        return;
    }

    const double total = somar_areas_treemap(itens);
    if (total <= 0.0) return;

    double acumulado = 0.0;
    int split_idx = 0;
    for (; split_idx < itens.size(); ++split_idx) {
        acumulado += qMax(0.0, itens.at(split_idx).area);
        if (acumulado >= total / 2.0) break;
    }
    split_idx = qBound(0, split_idx, itens.size() - 2);

    const QList<TreemapItem> grupo_a = itens.mid(0, split_idx + 1);
    const QList<TreemapItem> grupo_b = itens.mid(split_idx + 1);
    const double total_a = somar_areas_treemap(grupo_a);
    const double proporcao_a = total_a / total;

    if (area_total.width() >= area_total.height()) {
        const double largura_a = area_total.width() * proporcao_a;
        const QRectF rect_a(area_total.left(), area_total.top(), largura_a, area_total.height());
        const QRectF rect_b(area_total.left() + largura_a, area_total.top(), area_total.width() - largura_a, area_total.height());
        calcular_layout_treemap_recursivo(grupo_a, rect_a, resultado);
        calcular_layout_treemap_recursivo(grupo_b, rect_b, resultado);
    } else {
        const double altura_a = area_total.height() * proporcao_a;
        const QRectF rect_a(area_total.left(), area_total.top(), area_total.width(), altura_a);
        const QRectF rect_b(area_total.left(), area_total.top() + altura_a, area_total.width(), area_total.height() - altura_a);
        calcular_layout_treemap_recursivo(grupo_a, rect_a, resultado);
        calcular_layout_treemap_recursivo(grupo_b, rect_b, resultado);
    }
}

QList<TreemapRect> calcular_layout_treemap(const QList<TreemapItem>& itens, const QRectF& area_total) {
    QList<TreemapRect> resultado;
    calcular_layout_treemap_recursivo(itens, area_total, resultado);
    return resultado;
}

QColor interpolar_cor(const QColor& a, const QColor& b, double t) {
    const double u = qBound(0.0, t, 1.0);
    return QColor(
        static_cast<int>(a.red() + (b.red() - a.red()) * u),
        static_cast<int>(a.green() + (b.green() - a.green()) * u),
        static_cast<int>(a.blue() + (b.blue() - a.blue()) * u));
}

QColor cor_viridis(double t) {
    struct Stop {
        double p;
        QColor c;
    };

    const QList<Stop> stops = {
        {0.00, QColor(QStringLiteral("#440154"))},
        {0.25, QColor(QStringLiteral("#3b528b"))},
        {0.50, QColor(QStringLiteral("#21918c"))},
        {0.75, QColor(QStringLiteral("#5ec962"))},
        {1.00, QColor(QStringLiteral("#fde725"))}};

    const double clamped = qBound(0.0, t, 1.0);
    for (int i = 0; i < stops.size() - 1; ++i) {
        if (clamped >= stops.at(i).p && clamped <= stops.at(i + 1).p) {
            const double trecho = (clamped - stops.at(i).p) / (stops.at(i + 1).p - stops.at(i).p);
            return interpolar_cor(stops.at(i).c, stops.at(i + 1).c, trecho);
        }
    }
    return stops.last().c;
}

class TreemapWidget final : public QWidget {
public:
    TreemapWidget(const QString& titulo, const QList<TreemapItem>& itens, QWidget* parent = nullptr)
        : QWidget(parent), titulo_(titulo), itens_(itens) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setMinimumHeight(560);
        setMouseTracking(true);
        if (is_export_render_mode()) {
            anim_progress_ = 1.0;
        } else {
            anim_timer_.setInterval(16);
            connect(&anim_timer_, &QTimer::timeout, this, [this]() {
                anim_progress_ = qMin(1.0, anim_progress_ + 0.07);
                update();
                if (anim_progress_ >= 1.0) anim_timer_.stop();
            });
            anim_timer_.start();
        }
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::TextAntialiasing, true);
        QLinearGradient bg(0, 0, 0, height());
        bg.setColorAt(0.0, QColor(QStringLiteral("#fcfcfd")));
        bg.setColorAt(1.0, QColor(QStringLiteral("#f6f7fb")));
        painter.fillRect(rect(), bg);

        QFont fonte_titulo = painter.font();
        fonte_titulo.setBold(true);
        fonte_titulo.setPointSize(21);
        painter.setFont(fonte_titulo);
        painter.setPen(QColor(QStringLiteral("#111111")));
        painter.drawText(QRectF(0, 8, width(), 36), Qt::AlignCenter, titulo_);

        QRectF area(12.0, 56.0, width() - 24.0, height() - 68.0);
        if (area.width() <= 1.0 || area.height() <= 1.0 || itens_.isEmpty()) return;

        const double total_bruto = somar_areas_treemap(itens_);
        if (total_bruto <= 0.0) return;

        QList<TreemapItem> itens_escalados;
        itens_escalados.reserve(itens_.size());
        const double fator = (area.width() * area.height()) / total_bruto;
        for (TreemapItem item : itens_) {
            item.area = qMax(0.0, item.area * fator);
            itens_escalados.append(item);
        }
        std::sort(itens_escalados.begin(), itens_escalados.end(),
                  [](const TreemapItem& a, const TreemapItem& b) { return a.area > b.area; });

        blocos_renderizados_.clear();
        const QList<TreemapRect> blocos = calcular_layout_treemap(itens_escalados, area);
        const double escala_anim = 0.80 + (0.20 * anim_progress_);
        for (int i = 0; i < blocos.size(); ++i) {
            const TreemapRect& bloco = blocos.at(i);
            QRectF r = bloco.rect.adjusted(2.0, 2.0, -2.0, -2.0);
            if (r.width() <= 1.0 || r.height() <= 1.0) continue;

            QPointF c = r.center();
            QRectF anim_rect(
                c.x() - (r.width() * escala_anim) / 2.0,
                c.y() - (r.height() * escala_anim) / 2.0,
                r.width() * escala_anim,
                r.height() * escala_anim);

            const bool hovered = (i == hovered_index_);
            const QRectF draw_rect = hovered ? anim_rect.adjusted(-2.0, -2.0, 2.0, 2.0) : anim_rect;
            QPainterPath path;
            path.addRoundedRect(draw_rect, 8.0, 8.0);

            QColor cor_base = bloco.item.color;
            QLinearGradient grad(draw_rect.topLeft(), draw_rect.bottomRight());
            grad.setColorAt(0.0, cor_base.lighter(112));
            grad.setColorAt(1.0, cor_base.darker(108));
            painter.fillPath(path, grad);

            QPen border(hovered ? QColor(QStringLiteral("#1f2937")) : QColor(255, 255, 255, 140));
            border.setWidthF(hovered ? 1.8 : 1.0);
            painter.setPen(border);
            painter.drawPath(path);

            TreemapRect hit_rect{draw_rect, bloco.item};
            blocos_renderizados_.append(hit_rect);

            if (draw_rect.width() < 64.0 || draw_rect.height() < 30.0) continue;

            QFont fonte_item = painter.font();
            fonte_item.setBold(hovered);
            int pt = static_cast<int>(qMin(draw_rect.height() * 0.22, draw_rect.width() / qMax(2.0, bloco.item.label.size() * 0.42)));
            pt = qBound(9, pt, 24);
            fonte_item.setPointSize(pt);
            painter.setFont(fonte_item);
            const int luminance = (cor_base.red() * 299 + cor_base.green() * 587 + cor_base.blue() * 114) / 1000;
            painter.setPen(luminance < 128 ? QColor(Qt::white) : QColor(QStringLiteral("#111111")));
            painter.drawText(draw_rect.adjusted(6, 3, -6, -3), Qt::AlignCenter | Qt::TextWordWrap, bloco.item.label);
        }
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        int novo_hover = -1;
        for (int i = 0; i < blocos_renderizados_.size(); ++i) {
            if (blocos_renderizados_.at(i).rect.contains(event->pos())) {
                novo_hover = i;
                break;
            }
        }

        if (hovered_index_ != novo_hover) {
            hovered_index_ = novo_hover;
            if (hovered_index_ >= 0) {
                const TreemapRect& bloco = blocos_renderizados_.at(hovered_index_);
                const double total = qMax(1.0, somar_areas_treemap(itens_));
                const double pct = (bloco.item.value / total) * 100.0;
                const QString tip = QStringLiteral("%1\n%2: %3\n%4: %5%")
                                        .arg(bloco.item.label)
                                        .arg(ui_text("files", QStringLiteral("Arquivos")))
                                        .arg(QString::number(bloco.item.value, 'f', 0))
                                        .arg(ui_text("participation", QStringLiteral("Participação")))
                                        .arg(QString::number(pct, 'f', 1));
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                QToolTip::showText(event->globalPosition().toPoint(), tip, this);
#else
                QToolTip::showText(event->globalPos(), tip, this);
#endif
            } else {
                QToolTip::hideText();
            }
            update();
        }
        QWidget::mouseMoveEvent(event);
    }

    void leaveEvent(QEvent* event) override {
        hovered_index_ = -1;
        QToolTip::hideText();
        update();
        QWidget::leaveEvent(event);
    }

private:
    QString titulo_;
    QList<TreemapItem> itens_;
    QList<TreemapRect> blocos_renderizados_;
    QTimer anim_timer_;
    double anim_progress_ = 0.0;
    int hovered_index_ = -1;
};

struct TemporalHeatCell {
    QRectF rect;
    int row = -1;
    int col = -1;
    double value = 0.0;
};

struct TemporalLegendBand {
    QRectF rect;
    int index = -1;
    double min_value = 0.0;
    double max_value = 0.0;
};

class TemporalHeatmapWidget final : public QWidget {
public:
    TemporalHeatmapWidget(const QString& title, const QString& xlabel, const QString& ylabel, const QStringList& day_labels,
                          const QVector<QVector<double>>& matrix, QWidget* parent = nullptr)
        : QWidget(parent), title_(title), xlabel_(xlabel), ylabel_(ylabel), day_labels_(day_labels), matrix_(matrix) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setMinimumHeight(620);
        setMouseTracking(true);

        rows_ = matrix_.size();
        cols_ = 0;
        max_value_ = 0.0;
        for (const QVector<double>& row : matrix_) {
            cols_ = qMax(cols_, row.size());
            for (double v : row) max_value_ = qMax(max_value_, v);
        }
        if (rows_ <= 0 || cols_ <= 0) {
            rows_ = 0;
            cols_ = 0;
        } else {
            for (QVector<double>& row : matrix_) {
                if (row.size() < cols_) row.resize(cols_);
            }
        }

        if (is_export_render_mode()) {
            anim_progress_ = 1.0;
        } else {
            anim_timer_.setInterval(16);
            connect(&anim_timer_, &QTimer::timeout, this, [this]() {
                anim_progress_ = qMin(1.0, anim_progress_ + 0.06);
                update();
                if (anim_progress_ >= 1.0) anim_timer_.stop();
            });
            anim_timer_.start();
        }
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        QLinearGradient bg(0, 0, 0, height());
        bg.setColorAt(0.0, QColor(QStringLiteral("#f5f5f6")));
        bg.setColorAt(1.0, QColor(QStringLiteral("#ececee")));
        painter.fillRect(rect(), bg);

        QFont f_title = painter.font();
        f_title.setBold(true);
        f_title.setPointSize(20);
        painter.setFont(f_title);
        painter.setPen(QColor(QStringLiteral("#111111")));
        painter.drawText(QRectF(0.0, 8.0, width(), 36.0), Qt::AlignCenter, title_);

        if (rows_ <= 0 || cols_ <= 0) {
            QFont f_empty = painter.font();
            f_empty.setPointSize(13);
            painter.setFont(f_empty);
            painter.setPen(QColor(QStringLiteral("#555555")));
            painter.drawText(QRectF(0.0, 54.0, width(), height() - 54.0), Qt::AlignCenter,
                             ui_no_data_text());
            return;
        }

        const double left_margin = 118.0;
        const double right_margin = 154.0;
        const double top_margin = 56.0;
        const double bottom_margin = 74.0;
        QRectF plot_rect(left_margin, top_margin, width() - left_margin - right_margin, height() - top_margin - bottom_margin);
        if (plot_rect.width() <= 4.0 || plot_rect.height() <= 4.0) return;

        painter.setPen(QPen(QColor(170, 170, 170, 120), 0.8, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
        for (int c = 0; c <= cols_; ++c) {
            const double x = plot_rect.left() + (plot_rect.width() * static_cast<double>(c) / static_cast<double>(cols_));
            painter.drawLine(QPointF(x, plot_rect.top()), QPointF(x, plot_rect.bottom()));
        }
        for (int r = 0; r <= rows_; ++r) {
            const double y = plot_rect.top() + (plot_rect.height() * static_cast<double>(r) / static_cast<double>(rows_));
            painter.drawLine(QPointF(plot_rect.left(), y), QPointF(plot_rect.right(), y));
        }

        const int band_count = (max_value_ > 0.0) ? 8 : 1;
        const double active_band_min = (hovered_band_index_ >= 0) ? (max_value_ * hovered_band_index_) / band_count : 0.0;
        const double active_band_max =
            (hovered_band_index_ >= 0) ? (max_value_ * (hovered_band_index_ + 1)) / band_count : 0.0;

        rendered_cells_.clear();
        const double cell_w = plot_rect.width() / static_cast<double>(cols_);
        const double cell_h = plot_rect.height() / static_cast<double>(rows_);
        for (int r = 0; r < rows_; ++r) {
            for (int c = 0; c < cols_; ++c) {
                const double value = matrix_.at(r).value(c);
                const double norm = (max_value_ > 0.0) ? (value / max_value_) : 0.0;
                const double anim_norm = qBound(0.0, norm * (0.35 + 0.65 * anim_progress_), 1.0);
                QColor fill = color_from_scale(anim_norm);

                const bool cell_hovered = (hovered_row_ == r && hovered_col_ == c);
                const bool band_hovered = (hovered_band_index_ >= 0);
                bool in_band = false;
                if (band_hovered) {
                    in_band = (hovered_band_index_ == band_count - 1) ? (value >= active_band_min && value <= active_band_max)
                                                                      : (value >= active_band_min && value < active_band_max);
                }
                if (band_hovered && !in_band) fill.setAlpha(68);
                if (cell_hovered) fill = fill.lighter(126);

                QRectF cell_rect(plot_rect.left() + c * cell_w + 0.6, plot_rect.top() + r * cell_h + 0.6, cell_w - 1.2,
                                 cell_h - 1.2);
                painter.fillRect(cell_rect, fill);

                QPen border(QColor(QStringLiteral("#ffffff")), 0.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
                border.setWidthF(cell_hovered || in_band ? 1.7 : 0.8);
                if (cell_hovered || in_band) border.setColor(QColor(QStringLiteral("#111111")));
                painter.setPen(border);
                painter.drawRect(cell_rect);

                rendered_cells_.append({cell_rect, r, c, value});
            }
        }

        QFont f_axis = painter.font();
        f_axis.setPointSize(10);
        painter.setFont(f_axis);
        painter.setPen(QColor(QStringLiteral("#1b1b1b")));

        const int x_step = (cols_ > 18) ? 2 : 1;
        for (int c = 0; c < cols_; c += x_step) {
            const double x = plot_rect.left() + (c + 0.5) * cell_w;
            painter.drawText(QRectF(x - 18.0, plot_rect.bottom() + 6.0, 36.0, 18.0), Qt::AlignCenter,
                             QString::number(c));
        }
        for (int r = 0; r < rows_; ++r) {
            const double y = plot_rect.top() + (r + 0.5) * cell_h;
            const QString row_label = (r < day_labels_.size() && !day_labels_.at(r).trimmed().isEmpty())
                                          ? day_labels_.at(r)
                                          : ui_text("day_index_label", QStringLiteral("Dia %1")).arg(r + 1);
            painter.drawText(QRectF(10.0, y - 12.0, left_margin - 18.0, 24.0), Qt::AlignRight | Qt::AlignVCenter, row_label);
        }

        QFont f_label = painter.font();
        f_label.setPointSize(12);
        painter.setFont(f_label);
        painter.drawText(QRectF(plot_rect.left(), plot_rect.bottom() + 32.0, plot_rect.width(), 28.0), Qt::AlignCenter, xlabel_);

        painter.save();
        painter.translate(24.0, plot_rect.center().y());
        painter.rotate(-90.0);
        painter.drawText(QRectF(-plot_rect.height() / 2.0, -16.0, plot_rect.height(), 24.0), Qt::AlignCenter, ylabel_);
        painter.restore();

        QRectF colorbar_rect(plot_rect.right() + 28.0, plot_rect.top(), 36.0, plot_rect.height());
        QLinearGradient color_grad(colorbar_rect.bottomLeft(), colorbar_rect.topLeft());
        color_grad.setColorAt(0.0, color_from_scale(0.0));
        color_grad.setColorAt(0.2, color_from_scale(0.2));
        color_grad.setColorAt(0.4, color_from_scale(0.4));
        color_grad.setColorAt(0.6, color_from_scale(0.6));
        color_grad.setColorAt(0.8, color_from_scale(0.8));
        color_grad.setColorAt(1.0, color_from_scale(1.0));
        painter.fillRect(colorbar_rect, color_grad);
        painter.setPen(QPen(QColor(QStringLiteral("#9a9a9a")), 1.1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawRect(colorbar_rect);

        rendered_bands_.clear();
        for (int b = 0; b < band_count; ++b) {
            const double minv = (max_value_ * b) / band_count;
            const double maxv = (max_value_ * (b + 1)) / band_count;
            const double y_top = colorbar_rect.bottom() - ((static_cast<double>(b + 1) / band_count) * colorbar_rect.height());
            const double y_bottom = colorbar_rect.bottom() - ((static_cast<double>(b) / band_count) * colorbar_rect.height());
            QRectF band_rect(colorbar_rect.left() - 9.0, y_top, colorbar_rect.width() + 18.0, y_bottom - y_top);
            rendered_bands_.append({band_rect, b, minv, maxv});
            if (hovered_band_index_ == b) {
                painter.setPen(QPen(QColor(QStringLiteral("#111111")), 2.0));
                painter.setBrush(Qt::NoBrush);
                painter.drawRect(band_rect);
            }
        }

        const int tick_count = 6;
        painter.setPen(QColor(QStringLiteral("#1b1b1b")));
        for (int t = 0; t <= tick_count; ++t) {
            const double ratio = static_cast<double>(t) / tick_count;
            const double y = colorbar_rect.bottom() - ratio * colorbar_rect.height();
            const double value = max_value_ * ratio;
            painter.drawLine(QPointF(colorbar_rect.right() + 3.0, y), QPointF(colorbar_rect.right() + 8.0, y));
            painter.drawText(QRectF(colorbar_rect.right() + 12.0, y - 10.0, 48.0, 20.0), Qt::AlignLeft | Qt::AlignVCenter,
                             QString::number(value, 'f', value >= 100.0 ? 0 : 1));
        }

        painter.save();
        painter.translate(colorbar_rect.right() + 72.0, colorbar_rect.center().y());
        painter.rotate(-90.0);
        painter.drawText(QRectF(-colorbar_rect.height() / 2.0, -14.0, colorbar_rect.height(), 24.0), Qt::AlignCenter,
                         ui_text("quantity", QStringLiteral("Quantidade")));
        painter.restore();
    }

    void mouseMoveEvent(QMouseEvent* event) override {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        const QPoint cursor_pos = event->position().toPoint();
        const QPoint global_pos = event->globalPosition().toPoint();
#else
        const QPoint cursor_pos = event->pos();
        const QPoint global_pos = event->globalPos();
#endif

        int new_row = -1;
        int new_col = -1;
        for (const TemporalHeatCell& cell : rendered_cells_) {
            if (cell.rect.contains(cursor_pos)) {
                new_row = cell.row;
                new_col = cell.col;
                break;
            }
        }

        int new_band = -1;
        if (new_row < 0) {
            for (const TemporalLegendBand& band : rendered_bands_) {
                if (band.rect.contains(cursor_pos)) {
                    new_band = band.index;
                    break;
                }
            }
        }

        const bool changed = (new_row != hovered_row_) || (new_col != hovered_col_) || (new_band != hovered_band_index_);
        hovered_row_ = new_row;
        hovered_col_ = new_col;
        hovered_band_index_ = new_band;

        if (hovered_row_ >= 0 && hovered_col_ >= 0) {
            const QString row_label =
                (hovered_row_ < day_labels_.size() && !day_labels_.at(hovered_row_).trimmed().isEmpty())
                    ? day_labels_.at(hovered_row_)
                    : ui_text("day_index_label", QStringLiteral("Dia %1")).arg(hovered_row_ + 1);
            const double value = matrix_.at(hovered_row_).value(hovered_col_);
            const QString tip = ui_text("temporal_heat_cell_tooltip", QStringLiteral("%1\n%2: %3\nQuantidade: %4"))
                                    .arg(row_label)
                                    .arg(xlabel_.isEmpty() ? ui_text("time_label", QStringLiteral("Tempo")) : xlabel_)
                                    .arg(QString::number(hovered_col_))
                                    .arg(QString::number(value, 'f', value >= 100.0 ? 0 : 1));
            QToolTip::showText(global_pos, tip, this);
        } else if (hovered_band_index_ >= 0) {
            const int band_count = (max_value_ > 0.0) ? 8 : 1;
            const double minv = (max_value_ * hovered_band_index_) / band_count;
            const double maxv = (max_value_ * (hovered_band_index_ + 1)) / band_count;
            const QString tip = ui_text("quantity_scale_range_tooltip", QStringLiteral("Escala de Quantidade\nFaixa: %1 - %2"))
                                    .arg(QString::number(minv, 'f', minv >= 100.0 ? 0 : 1))
                                    .arg(QString::number(maxv, 'f', maxv >= 100.0 ? 0 : 1));
            QToolTip::showText(global_pos, tip, this);
        } else {
            QToolTip::hideText();
        }

        if (changed) update();
        QWidget::mouseMoveEvent(event);
    }

    void leaveEvent(QEvent* event) override {
        hovered_row_ = -1;
        hovered_col_ = -1;
        hovered_band_index_ = -1;
        QToolTip::hideText();
        update();
        QWidget::leaveEvent(event);
    }

private:
    static QColor color_from_scale(double t) {
        struct Stop {
            double pos;
            QColor color;
        };

        const QList<Stop> stops = {
            {0.00, QColor(QStringLiteral("#ececbc"))},
            {0.18, QColor(QStringLiteral("#efd67c"))},
            {0.38, QColor(QStringLiteral("#f1aa42"))},
            {0.62, QColor(QStringLiteral("#ff5e2f"))},
            {0.82, QColor(QStringLiteral("#ea142c"))},
            {1.00, QColor(QStringLiteral("#8d002e"))}};

        const double clamped = qBound(0.0, t, 1.0);
        for (int i = 0; i < stops.size() - 1; ++i) {
            if (clamped >= stops.at(i).pos && clamped <= stops.at(i + 1).pos) {
                const double local_t = (clamped - stops.at(i).pos) / (stops.at(i + 1).pos - stops.at(i).pos);
                return interpolar_cor(stops.at(i).color, stops.at(i + 1).color, local_t);
            }
        }
        return stops.last().color;
    }

    QString title_;
    QString xlabel_;
    QString ylabel_;
    QStringList day_labels_;
    QVector<QVector<double>> matrix_;
    int rows_ = 0;
    int cols_ = 0;
    double max_value_ = 0.0;
    QVector<TemporalHeatCell> rendered_cells_;
    QVector<TemporalLegendBand> rendered_bands_;
    QTimer anim_timer_;
    double anim_progress_ = 0.0;
    int hovered_row_ = -1;
    int hovered_col_ = -1;
    int hovered_band_index_ = -1;
};

struct DirectoryTreeLegendItem {
    int depth = -1;
    QRectF rect;
    QColor color;
};

class DirectoryTreeFlowWidget final : public QWidget {
public:
    explicit DirectoryTreeFlowWidget(const QVariantMap& fig_map, QWidget* parent = nullptr) : QWidget(parent) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setMinimumHeight(560);
        setMouseTracking(true);

        parse_data(fig_map);

        if (is_export_render_mode()) {
            anim_progress_ = 1.0;
        } else {
            anim_timer_.setInterval(16);
            connect(&anim_timer_, &QTimer::timeout, this, [this]() {
                anim_progress_ = qMin(1.0, anim_progress_ + 0.055);
                update();
                if (anim_progress_ >= 1.0) anim_timer_.stop();
            });
            anim_timer_.start();
        }

        tooltip_refresh_timer_.setInterval(180);
        connect(&tooltip_refresh_timer_, &QTimer::timeout, this, [this]() {
            if (active_tooltip_.trimmed().isEmpty() || !underMouse()) {
                clear_tooltip();
                return;
            }
            QToolTip::showText(tooltip_global_pos_, active_tooltip_, this, rect(), 4500);
        });
    }

protected:
    void resizeEvent(QResizeEvent* event) override {
        recompute_layout();
        QWidget::resizeEvent(event);
    }

    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        QLinearGradient bg(0, 0, 0, height());
        bg.setColorAt(0.0, QColor(QStringLiteral("#f6f6f7")));
        bg.setColorAt(1.0, QColor(QStringLiteral("#ededee")));
        painter.fillRect(rect(), bg);

        QFont f_title = painter.font();
        f_title.setBold(true);
        f_title.setPointSize(17);
        painter.setFont(f_title);
        painter.setPen(QColor(QStringLiteral("#2e2e2e")));
        painter.drawText(QRectF(0.0, 8.0, width(), 28.0), Qt::AlignCenter, title_);

        if (!root_path_.trimmed().isEmpty()) {
            QString root_txt = root_path_;
            if (root_txt.size() > 70) root_txt = QStringLiteral("...") + root_txt.right(67);
            QFont f_sub = painter.font();
            f_sub.setBold(false);
            f_sub.setPointSize(10);
            painter.setFont(f_sub);
            painter.setPen(QColor(QStringLiteral("#4b4b4b")));
            painter.drawText(QRectF(0.0, 34.0, width(), 22.0), Qt::AlignCenter,
                             ui_text("root_label", QStringLiteral("Raiz: %1")).arg(root_txt));
        }

        const bool has_active = (hovered_legend_depth_ >= 0 || hovered_node_ >= 0 || hovered_link_ >= 0);
        painter.setPen(Qt::NoPen);
        for (const DirectoryTreeLegendItem& item : legend_items_) {
            QColor fill = item.color.lighter(122);
            if (has_active && hovered_legend_depth_ >= 0 && hovered_legend_depth_ != item.depth) fill.setAlpha(70);
            if (hovered_legend_depth_ == item.depth) fill = item.color.lighter(145);
            painter.setBrush(fill);
            painter.drawRoundedRect(item.rect, 5.0, 5.0);

            QPen border(item.color.darker(hovered_legend_depth_ == item.depth ? 200 : 140));
            border.setWidthF(hovered_legend_depth_ == item.depth ? 1.8 : 1.1);
            painter.setPen(border);
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(item.rect, 5.0, 5.0);

            QFont f_leg = painter.font();
            f_leg.setPointSize(9);
            f_leg.setBold(hovered_legend_depth_ == item.depth);
            painter.setFont(f_leg);
            painter.setPen(QColor(QStringLiteral("#2f2f2f")));
            painter.drawText(item.rect.adjusted(6.0, 0.0, -6.0, 0.0), Qt::AlignLeft | Qt::AlignVCenter,
                             ui_text("level_count_label", QStringLiteral("Nível %1 (%2)")).arg(item.depth).arg(contar_nos_nivel(item.depth)));
        }

        if (nodes_.isEmpty() || links_.isEmpty() || plot_rect_.width() <= 2.0 || plot_rect_.height() <= 2.0) {
            QFont f_empty = painter.font();
            f_empty.setPointSize(12);
            painter.setFont(f_empty);
            painter.setPen(QColor(QStringLiteral("#666666")));
            painter.drawText(QRectF(0.0, 60.0, width(), height() - 60.0), Qt::AlignCenter,
                             ui_no_data_text());
            return;
        }

        const qreal eased = anim_progress_ * (2.0 - anim_progress_);
        const qreal reveal = 0.08 + 0.92 * eased;
        const qreal max_clip_width = qMax<qreal>(16.0, width() - plot_rect_.left() - 8.0);
        QRectF clip_rect(plot_rect_.left(), plot_rect_.top(), max_clip_width * reveal, plot_rect_.height());

        painter.save();
        painter.setClipRect(clip_rect.adjusted(-8.0, -8.0, 8.0, 8.0));

        for (int i = 0; i < links_.size(); ++i) {
            const DirectoryLink& lk = links_.at(i);
            if (lk.source < 0 || lk.source >= nodes_.size() || lk.target < 0 || lk.target >= nodes_.size()) continue;
            const bool highlighted = is_link_highlighted(i);

            QColor c = lk.color;
            int alpha = static_cast<int>(110.0 * eased);
            if (has_active) alpha = highlighted ? 230 : 26;
            c.setAlpha(qBound(0, alpha, 255));

            QPen pen(c);
            pen.setCapStyle(Qt::RoundCap);
            pen.setJoinStyle(Qt::RoundJoin);
            pen.setWidthF(has_active ? (highlighted ? lk.stroke * 1.25 : qMax(0.8, lk.stroke * 0.72))
                                     : (lk.stroke * (0.45 + 0.55 * eased)));
            painter.setPen(pen);
            painter.setBrush(Qt::NoBrush);
            painter.drawPath(lk.curve);

            if (has_active && highlighted) {
                QPen accent(QColor(255, 255, 255, 185));
                accent.setCapStyle(Qt::RoundCap);
                accent.setJoinStyle(Qt::RoundJoin);
                accent.setWidthF(qMax(1.0, lk.stroke * 0.24));
                painter.setPen(accent);
                painter.drawPath(lk.curve);
            }
        }

        for (int i = 0; i < nodes_.size(); ++i) {
            const DirectoryNode& node = nodes_.at(i);
            if (!node.rect.isValid()) continue;
            const bool highlighted = is_node_highlighted(i);
            QRectF r = node.rect;
            if (highlighted) r = r.adjusted(-1.0, -1.0, 1.0, 1.0);

            QColor base = node.color;
            if (has_active && !highlighted) base.setAlpha(95);
            if (highlighted) base = base.lighter(118);

            QLinearGradient grad(r.topLeft(), r.bottomRight());
            grad.setColorAt(0.0, base.lighter(118));
            grad.setColorAt(1.0, base.darker(110));
            painter.setBrush(grad);
            QPen border(base.darker(highlighted ? 205 : 145));
            border.setWidthF(highlighted ? 1.8 : 1.0);
            painter.setPen(border);
            painter.drawRoundedRect(r, 2.0, 2.0);

            if (node.depth == max_depth_) {
                if (r.height() < 3.6) continue;
                QFont f_leaf = painter.font();
                f_leaf.setPointSize(r.height() > 13.0 ? 9 : 8);
                f_leaf.setBold(highlighted);
                painter.setFont(f_leaf);
                QColor tc = QColor(QStringLiteral("#505050"));
                if (has_active && !highlighted) tc.setAlpha(110);
                painter.setPen(tc);
                const QRectF tx(r.right() + 5.0, r.center().y() - 7.0, width() - r.right() - 8.0, 14.0);
                painter.drawText(tx, Qt::AlignLeft | Qt::AlignVCenter, node.label);
                continue;
            }

            if (r.height() < 10.0) continue;
            QFont f_mid = painter.font();
            f_mid.setPointSize(node.depth == 0 ? 11 : 10);
            f_mid.setBold(highlighted && node.depth <= 1);
            painter.setFont(f_mid);
            QColor tc = QColor(QStringLiteral("#3d3d3d"));
            if (has_active && !highlighted) tc.setAlpha(115);
            painter.setPen(tc);
            const QRectF tx(r.right() + 5.0, r.center().y() - 9.0, 220.0, 18.0);
            painter.drawText(tx, Qt::AlignLeft | Qt::AlignVCenter, node.label);
        }

        painter.restore();
    }

    void mouseMoveEvent(QMouseEvent* event) override {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        const QPoint cursor_pos = event->position().toPoint();
        const QPoint global_pos = event->globalPosition().toPoint();
#else
        const QPoint cursor_pos = event->pos();
        const QPoint global_pos = event->globalPos();
#endif

        int new_legend_depth = -1;
        for (const DirectoryTreeLegendItem& item : legend_items_) {
            if (item.rect.contains(cursor_pos)) {
                new_legend_depth = item.depth;
                break;
            }
        }

        int new_node = -1;
        int new_link = -1;
        if (new_legend_depth < 0) {
            for (int i = 0; i < nodes_.size(); ++i) {
                if (nodes_.at(i).rect.adjusted(-2.0, -1.0, 2.0, 1.0).contains(cursor_pos)) {
                    new_node = i;
                    break;
                }
            }
            if (new_node < 0) {
                for (int i = links_.size() - 1; i >= 0; --i) {
                    if (links_.at(i).hit_shape.contains(cursor_pos)) {
                        new_link = i;
                        break;
                    }
                }
            }
        }

        const bool changed = (new_legend_depth != hovered_legend_depth_) || (new_node != hovered_node_) ||
                             (new_link != hovered_link_);
        hovered_legend_depth_ = new_legend_depth;
        hovered_node_ = new_node;
        hovered_link_ = new_link;

        if (hovered_legend_depth_ >= 0) {
            const QString tip = ui_text("level_directories_tooltip", QStringLiteral("Nível %1\nDiretórios: %2"))
                                    .arg(hovered_legend_depth_)
                                    .arg(contar_nos_nivel(hovered_legend_depth_));
            show_tooltip(tip, global_pos);
        } else if (hovered_node_ >= 0) {
            const DirectoryNode& node = nodes_.at(hovered_node_);
            const QString tip =
                ui_text("directory_tree_node_tooltip", QStringLiteral("%1\nNível: %2\nEntrada: %3\nSaída: %4\nCaminho: %5"))
                    .arg(node.label)
                    .arg(node.depth)
                    .arg(format_value(node.incoming))
                    .arg(format_value(node.outgoing))
                    .arg(node.path.trimmed().isEmpty() ? QStringLiteral("-") : node.path);
            show_tooltip(tip, global_pos);
        } else if (hovered_link_ >= 0) {
            const DirectoryLink& lk = links_.at(hovered_link_);
            const DirectoryNode& src = nodes_.at(lk.source);
            const DirectoryNode& dst = nodes_.at(lk.target);
            const QString tip =
                ui_text("directory_tree_link_tooltip", QStringLiteral("%1 -> %2\nValor: %3\nNíveis: %4 -> %5"))
                    .arg(src.label)
                    .arg(dst.label)
                    .arg(format_value(lk.value))
                    .arg(src.depth)
                    .arg(dst.depth);
            show_tooltip(tip, global_pos);
        } else {
            clear_tooltip();
        }

        if (changed) update();
        QWidget::mouseMoveEvent(event);
    }

    void leaveEvent(QEvent* event) override {
        hovered_legend_depth_ = -1;
        hovered_node_ = -1;
        hovered_link_ = -1;
        clear_tooltip();
        update();
        QWidget::leaveEvent(event);
    }

private:
    struct DirectoryNode {
        QString label;
        QString path;
        int depth = 0;
        double incoming = 0.0;
        double outgoing = 0.0;
        double total = 1.0;
        QColor color;
        QRectF rect;
    };

    struct DirectoryLink {
        int source = -1;
        int target = -1;
        int source_depth = 0;
        int target_depth = 0;
        double value = 0.0;
        double stroke = 1.0;
        QColor color;
        QPainterPath curve;
        QPainterPath hit_shape;
    };

    static double to_double_safe(const QVariant& v) {
        bool ok = false;
        const double d = v.toDouble(&ok);
        return ok ? d : 0.0;
    }

    static QString format_value(double v) {
        if (std::abs(v - std::round(v)) < 0.001) return QString::number(v, 'f', 0);
        return QString::number(v, 'f', 2);
    }

    QColor color_for_depth(int depth) const {
        const QList<QColor> palette = {
            QColor(QStringLiteral("#1f77b4")), QColor(QStringLiteral("#d62728")), QColor(QStringLiteral("#e6b3d1")),
            QColor(QStringLiteral("#87cde4")), QColor(QStringLiteral("#8ec6d9")), QColor(QStringLiteral("#9ad0df")),
            QColor(QStringLiteral("#acd9e5"))};
        if (depth < palette.size()) return palette.at(depth);
        const double t = qBound(0.0, static_cast<double>(depth) / qMax(1, max_depth_ + 1), 1.0);
        return interpolar_cor(QColor(QStringLiteral("#9ad0df")), QColor(QStringLiteral("#d7ecf2")), t);
    }

    void parse_data(const QVariantMap& fig_map) {
        title_ = fig_map.value(QStringLiteral("title")).toString().trimmed();
        if (title_.isEmpty()) title_ = ui_text("directory_tree", QStringLiteral("Árvore de Diretórios"));
        root_path_ = fig_map.value(QStringLiteral("root_path")).toString().trimmed();

        nodes_.clear();
        links_.clear();
        node_index_.clear();
        legend_items_.clear();
        max_depth_ = 0;

        const QVariantMap depth_map = fig_map.value(QStringLiteral("depth_map")).toMap();
        const QVariantMap node_paths = fig_map.value(QStringLiteral("node_paths")).toMap();
        const QVariantList node_labels = fig_map.value(QStringLiteral("nodes")).toList();

        auto ensure_node = [&](const QString& label) -> int {
            const QString key = label.trimmed();
            if (key.isEmpty()) return -1;
            if (node_index_.contains(key)) return node_index_.value(key);
            DirectoryNode node;
            node.label = key;
            node.path = node_paths.value(key).toString();
            node.depth = qMax(0, depth_map.value(key).toInt());
            node.color = color_for_depth(node.depth);
            const int idx = nodes_.size();
            nodes_.append(node);
            node_index_.insert(key, idx);
            max_depth_ = qMax(max_depth_, node.depth);
            return idx;
        };

        for (const QVariant& nv : node_labels) {
            ensure_node(nv.toString());
        }

        const QVariantList links = fig_map.value(QStringLiteral("links")).toList();
        for (const QVariant& lv : links) {
            const QVariantMap link_map = lv.toMap();
            const QString src_label = link_map.value(QStringLiteral("source")).toString().trimmed();
            const QString dst_label = link_map.value(QStringLiteral("target")).toString().trimmed();
            double value = to_double_safe(link_map.value(QStringLiteral("value")));
            if (src_label.isEmpty() || dst_label.isEmpty() || value <= 0.0) continue;
            const int src = ensure_node(src_label);
            const int dst = ensure_node(dst_label);
            if (src < 0 || dst < 0 || src == dst) continue;

            DirectoryLink l;
            l.source = src;
            l.target = dst;
            l.value = value;
            links_.append(l);

            nodes_[src].outgoing += value;
            nodes_[dst].incoming += value;
        }

        for (DirectoryNode& node : nodes_) {
            node.total = qMax(1.0, qMax(node.incoming, node.outgoing));
            node.color = color_for_depth(node.depth);
            max_depth_ = qMax(max_depth_, node.depth);
        }

        recompute_layout();
    }

    void recompute_layout() {
        if (width() <= 2 || height() <= 2 || nodes_.isEmpty()) {
            plot_rect_ = QRectF();
            return;
        }

        legend_items_.clear();
        QVector<int> depth_counts(max_depth_ + 1, 0);
        for (const DirectoryNode& node : nodes_) {
            if (node.depth >= 0 && node.depth < depth_counts.size()) depth_counts[node.depth] += 1;
        }

        int legend_x = 16;
        int legend_y = 60;
        const int legend_w = 98;
        const int legend_h = 22;
        const int gap_x = 8;
        const int gap_y = 6;
        for (int d = 0; d <= max_depth_; ++d) {
            if (depth_counts.value(d) <= 0) continue;
            if (legend_x + legend_w > width() - 16) {
                legend_x = 16;
                legend_y += legend_h + gap_y;
            }
            DirectoryTreeLegendItem item;
            item.depth = d;
            item.color = color_for_depth(d);
            item.rect = QRectF(legend_x, legend_y, legend_w, legend_h);
            legend_items_.append(item);
            legend_x += legend_w + gap_x;
        }

        const double left_margin = 34.0;
        double right_margin = qBound(180.0, width() * 0.30, 360.0);
        if (width() - left_margin - right_margin < 150.0) right_margin = qMax(120.0, width() * 0.22);
        const double top_margin = legend_items_.isEmpty() ? 68.0 : (legend_items_.last().rect.bottom() + 10.0);
        const double bottom_margin = 24.0;
        plot_rect_ = QRectF(left_margin, top_margin, width() - left_margin - right_margin,
                            height() - top_margin - bottom_margin);
        if (plot_rect_.width() <= 4.0 || plot_rect_.height() <= 4.0) return;

        QVector<QVector<int>> depth_buckets(max_depth_ + 1);
        for (int i = 0; i < nodes_.size(); ++i) {
            int d = qBound(0, nodes_.at(i).depth, max_depth_);
            depth_buckets[d].append(i);
        }
        for (QVector<int>& bucket : depth_buckets) {
            std::sort(bucket.begin(), bucket.end(), [&](int a, int b) {
                const DirectoryNode& na = nodes_.at(a);
                const DirectoryNode& nb = nodes_.at(b);
                if (!qFuzzyCompare(na.total, nb.total)) return na.total > nb.total;
                return na.label.localeAwareCompare(nb.label) < 0;
            });
        }

        const double node_w = qBound(12.0, plot_rect_.width() / qMax(4.0, (max_depth_ + 1) * 8.0), 24.0);
        const double x_step = (max_depth_ > 0) ? (plot_rect_.width() - node_w) / max_depth_ : 0.0;

        for (int d = 0; d <= max_depth_; ++d) {
            const QVector<int>& bucket = depth_buckets.at(d);
            if (bucket.isEmpty()) continue;

            const int n = bucket.size();
            const double gap = (n > 120) ? 0.3 : ((n > 70) ? 0.6 : ((n > 35) ? 1.0 : 2.0));
            const double available_h = plot_rect_.height() - gap * (n - 1);
            if (available_h <= 1.0) continue;

            double min_h = (n > 90) ? 2.0 : ((n > 40) ? 3.2 : 5.0);
            if (min_h * n > available_h) min_h = available_h / n;

            double total_weight = 0.0;
            for (int idx : bucket) total_weight += qMax(1.0, nodes_.at(idx).total);
            const double extra_h = qMax(0.0, available_h - min_h * n);
            const double scale = total_weight > 0.0 ? (extra_h / total_weight) : 0.0;

            const double x = plot_rect_.left() + d * x_step;
            double y = plot_rect_.top();
            for (int idx : bucket) {
                const double h = min_h + qMax(1.0, nodes_.at(idx).total) * scale;
                nodes_[idx].rect = QRectF(x, y, node_w, h);
                y += h + gap;
            }
        }

        QVector<int> order(links_.size());
        for (int i = 0; i < order.size(); ++i) order[i] = i;
        std::sort(order.begin(), order.end(), [&](int a, int b) {
            const DirectoryLink& la = links_.at(a);
            const DirectoryLink& lb = links_.at(b);
            const qreal asy = nodes_.at(la.source).rect.center().y();
            const qreal bsy = nodes_.at(lb.source).rect.center().y();
            if (!qFuzzyCompare(asy, bsy)) return asy < bsy;
            return nodes_.at(la.target).rect.center().y() < nodes_.at(lb.target).rect.center().y();
        });

        QVector<double> out_offset(nodes_.size(), 0.0);
        QVector<double> in_offset(nodes_.size(), 0.0);
        QVector<DirectoryLink> arranged_links;
        arranged_links.reserve(links_.size());

        for (int idx : order) {
            DirectoryLink link = links_.at(idx);
            if (link.source < 0 || link.source >= nodes_.size() || link.target < 0 || link.target >= nodes_.size()) continue;
            const DirectoryNode& src = nodes_.at(link.source);
            const DirectoryNode& dst = nodes_.at(link.target);

            const double src_total = qMax(1.0, src.outgoing > 0.0 ? src.outgoing : src.total);
            const double dst_total = qMax(1.0, dst.incoming > 0.0 ? dst.incoming : dst.total);
            const double src_scale = src.rect.height() / src_total;
            const double dst_scale = dst.rect.height() / dst_total;
            double stroke = qMin(link.value * src_scale, link.value * dst_scale);
            stroke = qBound(1.0, stroke, 180.0);

            const double sy = src.rect.top() + out_offset[link.source] + stroke * 0.5;
            const double ty = dst.rect.top() + in_offset[link.target] + stroke * 0.5;
            out_offset[link.source] += stroke;
            in_offset[link.target] += stroke;

            const double x1 = src.rect.right();
            const double x2 = qMax(dst.rect.left(), x1 + 2.0);
            const double c1 = x1 + (x2 - x1) * 0.36;
            const double c2 = x1 + (x2 - x1) * 0.64;

            QPainterPath path;
            path.moveTo(x1, sy);
            path.cubicTo(c1, sy, c2, ty, x2, ty);

            QPainterPathStroker stroker;
            stroker.setCapStyle(Qt::RoundCap);
            stroker.setJoinStyle(Qt::RoundJoin);
            stroker.setWidth(qMax(6.0, stroke + 6.0));

            link.stroke = stroke;
            link.source_depth = src.depth;
            link.target_depth = dst.depth;
            link.curve = path;
            link.hit_shape = stroker.createStroke(path);
            QColor mix = interpolar_cor(src.color, dst.color, 0.48);
            mix.setAlpha(118);
            link.color = mix;
            arranged_links.append(link);
        }

        links_ = arranged_links;
    }

    int contar_nos_nivel(int depth) const {
        int total = 0;
        for (const DirectoryNode& n : nodes_) {
            if (n.depth == depth) ++total;
        }
        return total;
    }

    bool is_node_highlighted(int idx) const {
        if (idx < 0 || idx >= nodes_.size()) return false;
        if (hovered_legend_depth_ >= 0) return nodes_.at(idx).depth == hovered_legend_depth_;
        if (hovered_node_ >= 0) return idx == hovered_node_;
        if (hovered_link_ >= 0 && hovered_link_ < links_.size()) {
            return idx == links_.at(hovered_link_).source || idx == links_.at(hovered_link_).target;
        }
        return false;
    }

    bool is_link_highlighted(int idx) const {
        if (idx < 0 || idx >= links_.size()) return false;
        const DirectoryLink& link = links_.at(idx);
        if (hovered_legend_depth_ >= 0) {
            return link.source_depth == hovered_legend_depth_ || link.target_depth == hovered_legend_depth_;
        }
        if (hovered_node_ >= 0) {
            return link.source == hovered_node_ || link.target == hovered_node_;
        }
        if (hovered_link_ >= 0) return idx == hovered_link_;
        return false;
    }

    void show_tooltip(const QString& text, const QPoint& global_pos) {
        active_tooltip_ = text;
        tooltip_global_pos_ = global_pos;
        if (!tooltip_refresh_timer_.isActive()) tooltip_refresh_timer_.start();
        QToolTip::showText(global_pos, text, this, rect(), 4500);
    }

    void clear_tooltip() {
        active_tooltip_.clear();
        tooltip_refresh_timer_.stop();
        QToolTip::hideText();
    }

    QString title_;
    QString root_path_;
    QVector<DirectoryNode> nodes_;
    QVector<DirectoryLink> links_;
    QHash<QString, int> node_index_;
    QVector<DirectoryTreeLegendItem> legend_items_;
    QRectF plot_rect_;
    int max_depth_ = 0;

    QTimer anim_timer_;
    qreal anim_progress_ = 0.0;

    QTimer tooltip_refresh_timer_;
    QString active_tooltip_;
    QPoint tooltip_global_pos_;

    int hovered_legend_depth_ = -1;
    int hovered_node_ = -1;
    int hovered_link_ = -1;
};

struct EventFlowLegendItem {
    int node_index = -1;
    QRectF rect;
    QColor color;
    QString label;
};

class EventDirectoryFlowWidget final : public QWidget {
public:
    explicit EventDirectoryFlowWidget(const QVariantMap& fig_map, QWidget* parent = nullptr) : QWidget(parent) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        setMinimumHeight(kBaseMinHeight);
        setMouseTracking(true);

        parse_data(fig_map);

        if (is_export_render_mode()) {
            anim_progress_ = 1.0;
        } else {
            anim_timer_.setInterval(16);
            connect(&anim_timer_, &QTimer::timeout, this, [this]() {
                anim_progress_ = qMin(1.0, anim_progress_ + 0.06);
                update();
                if (anim_progress_ >= 1.0) anim_timer_.stop();
            });
            anim_timer_.start();
        }

        tooltip_refresh_timer_.setInterval(120);
        connect(&tooltip_refresh_timer_, &QTimer::timeout, this, [this]() {
            if (active_tooltip_.trimmed().isEmpty() || !underMouse()) {
                clear_tooltip();
                return;
            }
            QToolTip::showText(tooltip_global_pos_, active_tooltip_, this, rect(), 9000);
        });
    }

protected:
    void resizeEvent(QResizeEvent* event) override {
        recompute_layout();
        QWidget::resizeEvent(event);
    }

    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        QLinearGradient bg(0, 0, 0, height());
        bg.setColorAt(0.0, QColor(QStringLiteral("#f6f6f7")));
        bg.setColorAt(1.0, QColor(QStringLiteral("#ededee")));
        painter.fillRect(rect(), bg);

        QFont f_title = painter.font();
        f_title.setBold(true);
        f_title.setPointSize(17);
        painter.setFont(f_title);
        painter.setPen(QColor(QStringLiteral("#2e2e2e")));
        painter.drawText(QRectF(0.0, 8.0, width(), 28.0), Qt::AlignCenter, title_);

        if (!subtitle_.trimmed().isEmpty()) {
            QFont f_sub = painter.font();
            f_sub.setBold(false);
            f_sub.setPointSize(10);
            painter.setFont(f_sub);
            painter.setPen(QColor(QStringLiteral("#4b4b4b")));
            painter.drawText(QRectF(0.0, 34.0, width(), 22.0), Qt::AlignCenter, subtitle_);
        }

        const bool has_active = (hovered_legend_item_ >= 0 || hovered_node_ >= 0 || hovered_link_ >= 0);
        for (int i = 0; i < legend_items_.size(); ++i) {
            const EventFlowLegendItem& item = legend_items_.at(i);
            const bool highlighted = (hovered_legend_item_ == i);

            QColor fill = item.color.lighter(130);
            if (has_active && !highlighted) fill.setAlpha(70);
            if (highlighted) fill = item.color.lighter(150);
            painter.setBrush(fill);
            painter.setPen(Qt::NoPen);
            painter.drawRoundedRect(item.rect, 5.0, 5.0);

            QPen border(item.color.darker(highlighted ? 200 : 145));
            border.setWidthF(highlighted ? 1.8 : 1.1);
            painter.setPen(border);
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(item.rect, 5.0, 5.0);

            QFont f_leg = painter.font();
            f_leg.setPointSize(9);
            f_leg.setBold(highlighted);
            painter.setFont(f_leg);
            painter.setPen(QColor(QStringLiteral("#2f2f2f")));
            painter.drawText(item.rect.adjusted(7.0, 0.0, -7.0, 0.0), Qt::AlignLeft | Qt::AlignVCenter, item.label);
        }

        if (nodes_.isEmpty() || links_.isEmpty() || plot_rect_.width() <= 2.0 || plot_rect_.height() <= 2.0) {
            QFont f_empty = painter.font();
            f_empty.setPointSize(12);
            painter.setFont(f_empty);
            painter.setPen(QColor(QStringLiteral("#666666")));
            painter.drawText(QRectF(0.0, 60.0, width(), height() - 60.0), Qt::AlignCenter,
                             ui_no_data_text());
            return;
        }

        const qreal eased = anim_progress_ * (2.0 - anim_progress_);
        QRectF clip_rect = plot_rect_;
        clip_rect.setWidth(plot_rect_.width() * (0.08 + 0.92 * eased));

        painter.save();
        painter.setClipRect(clip_rect.adjusted(-8.0, -8.0, 8.0, 8.0));

        for (int i = 0; i < links_.size(); ++i) {
            const FlowLink& link = links_.at(i);
            if (link.source < 0 || link.source >= nodes_.size() || link.target < 0 || link.target >= nodes_.size()) continue;

            const bool highlighted = is_link_highlighted(i);
            QColor c = link.color;
            int alpha = static_cast<int>(120.0 * eased);
            if (has_active) alpha = highlighted ? 235 : 24;
            c.setAlpha(qBound(0, alpha, 255));

            QPen pen(c);
            pen.setCapStyle(Qt::RoundCap);
            pen.setJoinStyle(Qt::RoundJoin);
            pen.setWidthF(has_active ? (highlighted ? link.stroke * 1.25 : qMax(0.9, link.stroke * 0.68))
                                     : (link.stroke * (0.5 + 0.5 * eased)));
            painter.setPen(pen);
            painter.setBrush(Qt::NoBrush);
            painter.drawPath(link.curve);

            if (has_active && highlighted) {
                QPen accent(QColor(255, 255, 255, 185));
                accent.setCapStyle(Qt::RoundCap);
                accent.setJoinStyle(Qt::RoundJoin);
                accent.setWidthF(qMax(1.0, link.stroke * 0.23));
                painter.setPen(accent);
                painter.drawPath(link.curve);
            }
        }

        for (int i = 0; i < nodes_.size(); ++i) {
            const FlowNode& node = nodes_.at(i);
            if (!node.rect.isValid()) continue;

            const bool highlighted = is_node_highlighted(i);
            QRectF r = node.rect;
            if (highlighted) r = r.adjusted(-1.0, -1.0, 1.0, 1.0);

            QColor base = node.color;
            if (has_active && !highlighted) base.setAlpha(90);
            if (highlighted) base = base.lighter(120);

            QLinearGradient grad(r.topLeft(), r.bottomRight());
            grad.setColorAt(0.0, base.lighter(118));
            grad.setColorAt(1.0, base.darker(112));
            painter.setBrush(grad);
            QPen border(base.darker(highlighted ? 205 : 145));
            border.setWidthF(highlighted ? 1.8 : 1.0);
            painter.setPen(border);
            painter.drawRoundedRect(r, 2.0, 2.0);

            if (r.height() < 3.2) continue;
            QFont f_txt = painter.font();
            f_txt.setPointSize(r.height() > 12.0 ? 9 : 8);
            f_txt.setBold(highlighted);
            painter.setFont(f_txt);
            QColor tc = QColor(QStringLiteral("#3d3d3d"));
            if (has_active && !highlighted) tc.setAlpha(115);
            painter.setPen(tc);

            if (node.side == 0) {
                const QRectF tx(r.right() + 5.0, r.center().y() - 9.0, qMax(120.0, plot_rect_.width() * 0.22), 18.0);
                painter.drawText(tx, Qt::AlignLeft | Qt::AlignVCenter, node.label);
            } else {
                const QRectF tx(plot_rect_.left(), r.center().y() - 7.0, qMax(20.0, r.left() - plot_rect_.left() - 6.0), 14.0);
                painter.drawText(tx, Qt::AlignRight | Qt::AlignVCenter, node.label);
            }
        }

        painter.restore();
    }

    void mouseMoveEvent(QMouseEvent* event) override {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        const QPoint cursor_pos = event->position().toPoint();
        const QPoint global_pos = event->globalPosition().toPoint();
#else
        const QPoint cursor_pos = event->pos();
        const QPoint global_pos = event->globalPos();
#endif

        int new_legend_item = -1;
        for (int i = 0; i < legend_items_.size(); ++i) {
            if (legend_items_.at(i).rect.contains(cursor_pos)) {
                new_legend_item = i;
                break;
            }
        }

        int new_node = -1;
        int new_link = -1;
        if (new_legend_item < 0) {
            for (int i = 0; i < nodes_.size(); ++i) {
                if (nodes_.at(i).rect.adjusted(-2.0, -1.0, 2.0, 1.0).contains(cursor_pos)) {
                    new_node = i;
                    break;
                }
            }
            if (new_node < 0) {
                for (int i = links_.size() - 1; i >= 0; --i) {
                    if (links_.at(i).hit_shape.contains(cursor_pos)) {
                        new_link = i;
                        break;
                    }
                }
            }
        }

        const bool changed = (new_legend_item != hovered_legend_item_) || (new_node != hovered_node_) ||
                             (new_link != hovered_link_);
        hovered_legend_item_ = new_legend_item;
        hovered_node_ = new_node;
        hovered_link_ = new_link;

        if (hovered_legend_item_ >= 0) {
            const int idx = legend_items_.at(hovered_legend_item_).node_index;
            if (idx >= 0 && idx < nodes_.size()) {
                const FlowNode& n = nodes_.at(idx);
                const QString tip = QStringLiteral("%1\nTotal: %2")
                                        .arg(n.label)
                                        .arg(format_value(qMax(n.outgoing, n.incoming)));
                show_tooltip(tip, global_pos);
            }
        } else if (hovered_node_ >= 0) {
            const FlowNode& n = nodes_.at(hovered_node_);
            QString tip;
            if (n.side == 0) {
                tip = ui_text("sankey_node_io_tooltip", QStringLiteral("%1\nEntradas: %2\nSaídas: %3"))
                          .arg(n.label)
                          .arg(format_value(n.incoming))
                          .arg(format_value(n.outgoing));
            } else {
                tip = ui_text("sankey_node_path_tooltip", QStringLiteral("%1\nEntradas: %2\nCaminho: %3"))
                          .arg(n.label)
                          .arg(format_value(n.incoming))
                          .arg(n.full_path.trimmed().isEmpty() ? QStringLiteral("-") : n.full_path);
            }
            show_tooltip(tip, global_pos);
        } else if (hovered_link_ >= 0 && hovered_link_ < links_.size()) {
            const FlowLink& l = links_.at(hovered_link_);
            const FlowNode& src = nodes_.at(l.source);
            const FlowNode& dst = nodes_.at(l.target);
            const QString full_path =
                l.target_full_path.trimmed().isEmpty() ? (dst.full_path.trimmed().isEmpty() ? dst.label : dst.full_path)
                                                       : l.target_full_path;
            const QString tip =
                ui_text("sankey_link_directory_tooltip", QStringLiteral("%1 -> %2\nQuantidade: %3\nDiretório: %4"))
                    .arg(src.label)
                    .arg(dst.label)
                    .arg(format_value(l.value))
                    .arg(full_path);
            show_tooltip(tip, global_pos);
        } else {
            clear_tooltip();
        }

        if (hovered_legend_item_ >= 0 || hovered_node_ >= 0 || hovered_link_ >= 0) {
            setCursor(Qt::PointingHandCursor);
        } else {
            unsetCursor();
        }

        if (changed) update();
        QWidget::mouseMoveEvent(event);
    }

    void leaveEvent(QEvent* event) override {
        hovered_legend_item_ = -1;
        hovered_node_ = -1;
        hovered_link_ = -1;
        clear_tooltip();
        unsetCursor();
        update();
        QWidget::leaveEvent(event);
    }

private:
    struct FlowNode {
        QString label;
        QString full_path;
        int side = 0;
        double incoming = 0.0;
        double outgoing = 0.0;
        double total = 1.0;
        QColor color;
        QRectF rect;
    };

    struct FlowLink {
        int source = -1;
        int target = -1;
        double value = 0.0;
        double stroke = 1.0;
        QColor color;
        QString target_full_path;
        QPainterPath curve;
        QPainterPath hit_shape;
    };

    static constexpr int kBaseMinHeight = 560;
    static constexpr double kMinPlotHeight = 320.0;

    static QString format_value(double v) {
        if (std::abs(v - std::round(v)) < 0.001) return QString::number(v, 'f', 0);
        return QString::number(v, 'f', 2);
    }

    static double to_double_safe(const QVariant& v) {
        bool ok = false;
        const double d = v.toDouble(&ok);
        return ok ? d : 0.0;
    }

    QColor fallback_color_for_side(int side, int idx) const {
        const QList<QColor> left_palette = {
            QColor(QStringLiteral("#ff0000")), QColor(QStringLiteral("#0000ff")), QColor(QStringLiteral("#ff00ff")),
            QColor(QStringLiteral("#808080")), QColor(QStringLiteral("#ff6200")), QColor(QStringLiteral("#00b300")),
            QColor(QStringLiteral("#00a5cf")), QColor(QStringLiteral("#a23fd4"))};
        const QList<QColor> right_palette = {
            QColor(QStringLiteral("#9ecae1")), QColor(QStringLiteral("#6baed6")), QColor(QStringLiteral("#74c476")),
            QColor(QStringLiteral("#fdae6b")), QColor(QStringLiteral("#c7b9ff")), QColor(QStringLiteral("#fdd0a2")),
            QColor(QStringLiteral("#9edae5")), QColor(QStringLiteral("#c7e9c0"))};
        return side == 0 ? left_palette.at(idx % left_palette.size()) : right_palette.at(idx % right_palette.size());
    }

    void append_unique(QStringList& dst, const QVariantList& src) {
        for (const QVariant& v : src) {
            const QString txt = v.toString().trimmed();
            if (!txt.isEmpty() && !dst.contains(txt)) dst.append(txt);
        }
    }

    void parse_data(const QVariantMap& fig_map) {
        title_ = fig_map.value(QStringLiteral("title")).toString().trimmed();
        if (title_.isEmpty()) title_ = ui_text("event_flow_by_directory", QStringLiteral("Fluxo de Evento por Diretório"));
        subtitle_ = fig_map.value(QStringLiteral("subtitle")).toString().trimmed();

        nodes_.clear();
        links_.clear();
        node_index_.clear();
        legend_items_.clear();
        left_color_index_ = 0;
        right_color_index_ = 0;

        const QVariantMap color_map = fig_map.value(QStringLiteral("node_colors")).toMap();
        QHash<QString, QColor> node_colors;
        for (auto it = color_map.constBegin(); it != color_map.constEnd(); ++it) {
            QColor c(it.value().toString());
            if (c.isValid()) node_colors.insert(it.key(), c);
        }

        const QVariantList links_raw = fig_map.value(QStringLiteral("links")).toList();
        QStringList left_labels;
        QStringList right_labels;
        append_unique(left_labels, fig_map.value(QStringLiteral("events")).toList());
        append_unique(left_labels, fig_map.value(QStringLiteral("operations")).toList());
        append_unique(left_labels, fig_map.value(QStringLiteral("types")).toList());
        append_unique(right_labels, fig_map.value(QStringLiteral("paths")).toList());
        append_unique(right_labels, fig_map.value(QStringLiteral("types")).toList());

        for (const QVariant& lv : links_raw) {
            const QVariantMap l = lv.toMap();
            const QString s = l.value(QStringLiteral("source")).toString().trimmed();
            const QString t = l.value(QStringLiteral("target")).toString().trimmed();
            if (!s.isEmpty() && !left_labels.contains(s)) left_labels.append(s);
            if (!t.isEmpty() && !right_labels.contains(t)) right_labels.append(t);
        }

        auto ensure_node = [&](const QString& label, int side) -> int {
            const QString key = QStringLiteral("%1:%2").arg(side).arg(label);
            if (key.trimmed().isEmpty()) return -1;
            if (node_index_.contains(key)) return node_index_.value(key);

            FlowNode node;
            node.label = label;
            node.side = side;
            if (node_colors.contains(label)) {
                node.color = node_colors.value(label);
            } else {
                const int idx_side = (side == 0) ? left_color_index_++ : right_color_index_++;
                node.color = fallback_color_for_side(side, idx_side);
            }

            const int idx = nodes_.size();
            nodes_.append(node);
            node_index_.insert(key, idx);
            return idx;
        };

        for (const QString& label : left_labels) ensure_node(label, 0);
        for (const QString& label : right_labels) ensure_node(label, 1);

        for (const QVariant& lv : links_raw) {
            const QVariantMap l = lv.toMap();
            const QString source = l.value(QStringLiteral("source")).toString().trimmed();
            const QString target = l.value(QStringLiteral("target")).toString().trimmed();
            const double value = to_double_safe(l.value(QStringLiteral("value")));
            if (source.isEmpty() || target.isEmpty() || value <= 0.0) continue;

            const int src = ensure_node(source, 0);
            const int dst = ensure_node(target, 1);
            if (src < 0 || dst < 0 || src == dst) continue;

            FlowLink link;
            link.source = src;
            link.target = dst;
            link.value = value;
            link.target_full_path = l.value(QStringLiteral("target_full_path")).toString().trimmed();
            link.color = nodes_.at(src).color;
            links_.append(link);

            nodes_[src].outgoing += value;
            nodes_[dst].incoming += value;
            if (!link.target_full_path.isEmpty()) nodes_[dst].full_path = link.target_full_path;
        }

        QVector<FlowNode> compact_nodes;
        compact_nodes.reserve(nodes_.size());
        QVector<int> remap(nodes_.size(), -1);
        for (int i = 0; i < nodes_.size(); ++i) {
            const FlowNode& n = nodes_.at(i);
            if (qMax(n.incoming, n.outgoing) <= 0.0) continue;
            remap[i] = compact_nodes.size();
            compact_nodes.append(n);
        }

        QVector<FlowLink> compact_links;
        compact_links.reserve(links_.size());
        for (const FlowLink& link : links_) {
            if (link.source < 0 || link.source >= remap.size() || link.target < 0 || link.target >= remap.size()) continue;
            const int new_src = remap.at(link.source);
            const int new_dst = remap.at(link.target);
            if (new_src < 0 || new_dst < 0 || new_src == new_dst) continue;
            FlowLink c = link;
            c.source = new_src;
            c.target = new_dst;
            compact_links.append(c);
        }

        nodes_ = compact_nodes;
        links_ = compact_links;
        node_index_.clear();
        for (int i = 0; i < nodes_.size(); ++i) {
            node_index_.insert(QStringLiteral("%1:%2").arg(nodes_.at(i).side).arg(nodes_.at(i).label), i);
        }

        for (FlowNode& node : nodes_) {
            node.total = qMax(1.0, qMax(node.incoming, node.outgoing));
        }

        recompute_layout();
    }

    void recompute_layout() {
        legend_items_.clear();
        plot_rect_ = QRectF();
        if (width() <= 2 || height() <= 2 || nodes_.isEmpty()) return;

        QVector<int> left_nodes;
        QVector<int> right_nodes;
        left_nodes.reserve(nodes_.size());
        right_nodes.reserve(nodes_.size());
        for (int i = 0; i < nodes_.size(); ++i) {
            if (nodes_.at(i).side == 0) left_nodes.append(i);
            if (nodes_.at(i).side == 1) right_nodes.append(i);
        }
        if (left_nodes.isEmpty() || right_nodes.isEmpty()) return;

        std::sort(left_nodes.begin(), left_nodes.end(), [&](int a, int b) {
            if (!qFuzzyCompare(nodes_.at(a).total, nodes_.at(b).total)) return nodes_.at(a).total > nodes_.at(b).total;
            return nodes_.at(a).label.localeAwareCompare(nodes_.at(b).label) < 0;
        });
        std::sort(right_nodes.begin(), right_nodes.end(), [&](int a, int b) {
            if (!qFuzzyCompare(nodes_.at(a).total, nodes_.at(b).total)) return nodes_.at(a).total > nodes_.at(b).total;
            return nodes_.at(a).label.localeAwareCompare(nodes_.at(b).label) < 0;
        });

        int legend_x = 16;
        int legend_y = 60;
        const int legend_h = 22;
        const int gap_x = 8;
        const int gap_y = 6;
        for (int idx : left_nodes) {
            const QString& txt = nodes_.at(idx).label;
            const int legend_w = qBound(110, 44 + (txt.size() * 7), 210);
            if (legend_x + legend_w > width() - 16) {
                legend_x = 16;
                legend_y += legend_h + gap_y;
            }
            EventFlowLegendItem item;
            item.node_index = idx;
            item.color = nodes_.at(idx).color;
            item.label = txt;
            item.rect = QRectF(legend_x, legend_y, legend_w, legend_h);
            legend_items_.append(item);
            legend_x += legend_w + gap_x;
        }

        const double left_margin = 34.0;
        double right_margin = qBound(220.0, width() * 0.36, 520.0);
        if (width() - left_margin - right_margin < 180.0) right_margin = qMax(160.0, width() * 0.30);
        const double top_margin = legend_items_.isEmpty() ? 68.0 : (legend_items_.last().rect.bottom() + 10.0);
        const double bottom_margin = 24.0;
        const int max_nodes_por_coluna = qMax(left_nodes.size(), right_nodes.size());
        const double desired_plot_height = qBound(kMinPlotHeight, 160.0 + (max_nodes_por_coluna * 3.4), 1400.0);
        const int required_min_height = qMax(
            kBaseMinHeight, static_cast<int>(std::ceil(top_margin + bottom_margin + desired_plot_height)));

        if (minimumHeight() != required_min_height) {
            setMinimumHeight(required_min_height);
            updateGeometry();
        }

        const double effective_height =
            qMax(static_cast<double>(height()), static_cast<double>(required_min_height));
        plot_rect_ = QRectF(left_margin, top_margin, width() - left_margin - right_margin,
                            effective_height - top_margin - bottom_margin);
        if (plot_rect_.width() <= 8.0 || plot_rect_.height() <= 8.0) return;

        const double node_w = qBound(12.0, plot_rect_.width() / 14.0, 24.0);
        const double x_left = plot_rect_.left();
        const double x_right = plot_rect_.right() - node_w;

        auto place_column = [&](const QVector<int>& bucket, double x, bool use_outgoing) {
            if (bucket.isEmpty()) return;
            const int n = bucket.size();
            const double gap = (n > 120) ? 0.35 : ((n > 70) ? 0.7 : ((n > 35) ? 1.2 : 2.0));
            const double available_h = plot_rect_.height() - gap * qMax(0, n - 1);
            if (available_h <= 1.0) return;

            double min_h = (n > 120) ? 1.4 : ((n > 70) ? 2.0 : ((n > 35) ? 3.2 : 5.0));
            if (min_h * n > available_h) min_h = qMax(0.8, available_h / n);

            double total_weight = 0.0;
            for (int idx : bucket) {
                const double ref = use_outgoing ? qMax(1.0, nodes_.at(idx).outgoing) : qMax(1.0, nodes_.at(idx).incoming);
                total_weight += ref;
            }
            const double extra_h = qMax(0.0, available_h - min_h * n);
            const double scale = total_weight > 0.0 ? (extra_h / total_weight) : 0.0;

            double y = plot_rect_.top();
            for (int idx : bucket) {
                const double ref = use_outgoing ? qMax(1.0, nodes_.at(idx).outgoing) : qMax(1.0, nodes_.at(idx).incoming);
                const double h = min_h + ref * scale;
                nodes_[idx].rect = QRectF(x, y, node_w, h);
                y += h + gap;
            }
        };

        place_column(left_nodes, x_left, true);
        place_column(right_nodes, x_right, false);

        QVector<int> order(links_.size());
        for (int i = 0; i < order.size(); ++i) order[i] = i;
        std::sort(order.begin(), order.end(), [&](int a, int b) {
            const FlowLink& la = links_.at(a);
            const FlowLink& lb = links_.at(b);
            const qreal asy = nodes_.at(la.source).rect.center().y();
            const qreal bsy = nodes_.at(lb.source).rect.center().y();
            if (!qFuzzyCompare(asy, bsy)) return asy < bsy;
            return nodes_.at(la.target).rect.center().y() < nodes_.at(lb.target).rect.center().y();
        });

        QVector<double> out_offset(nodes_.size(), 0.0);
        QVector<double> in_offset(nodes_.size(), 0.0);
        QVector<FlowLink> arranged_links;
        arranged_links.reserve(links_.size());

        for (int idx : order) {
            FlowLink link = links_.at(idx);
            if (link.source < 0 || link.source >= nodes_.size() || link.target < 0 || link.target >= nodes_.size()) continue;
            const FlowNode& src = nodes_.at(link.source);
            const FlowNode& dst = nodes_.at(link.target);

            const double src_total = qMax(1.0, src.outgoing);
            const double dst_total = qMax(1.0, dst.incoming);
            const double src_scale = src.rect.height() / src_total;
            const double dst_scale = dst.rect.height() / dst_total;
            double stroke = qMin(link.value * src_scale, link.value * dst_scale);
            stroke = qBound(1.0, stroke, 90.0);

            const double sy = src.rect.top() + out_offset[link.source] + stroke * 0.5;
            const double ty = dst.rect.top() + in_offset[link.target] + stroke * 0.5;
            out_offset[link.source] += stroke;
            in_offset[link.target] += stroke;

            const double x1 = src.rect.right();
            const double x2 = qMax(dst.rect.left(), x1 + 2.0);
            const double c1 = x1 + (x2 - x1) * 0.35;
            const double c2 = x1 + (x2 - x1) * 0.65;

            QPainterPath path;
            path.moveTo(x1, sy);
            path.cubicTo(c1, sy, c2, ty, x2, ty);

            QPainterPathStroker stroker;
            stroker.setCapStyle(Qt::RoundCap);
            stroker.setJoinStyle(Qt::RoundJoin);
            stroker.setWidth(qMax(8.0, stroke + 8.0));

            link.stroke = stroke;
            link.curve = path;
            link.hit_shape = stroker.createStroke(path);
            QColor lc = link.color;
            lc.setAlpha(120);
            link.color = lc;
            arranged_links.append(link);
        }

        links_ = arranged_links;
    }

    bool is_link_highlighted(int idx) const {
        if (idx < 0 || idx >= links_.size()) return false;
        const FlowLink& link = links_.at(idx);

        if (hovered_legend_item_ >= 0 && hovered_legend_item_ < legend_items_.size()) {
            const int node_idx = legend_items_.at(hovered_legend_item_).node_index;
            return link.source == node_idx;
        }
        if (hovered_node_ >= 0) {
            return link.source == hovered_node_ || link.target == hovered_node_;
        }
        if (hovered_link_ >= 0) return idx == hovered_link_;
        return false;
    }

    bool is_node_highlighted(int idx) const {
        if (idx < 0 || idx >= nodes_.size()) return false;
        if (hovered_legend_item_ >= 0 && hovered_legend_item_ < legend_items_.size()) {
            const int src_idx = legend_items_.at(hovered_legend_item_).node_index;
            if (idx == src_idx) return true;
            for (const FlowLink& link : links_) {
                if (link.source == src_idx && link.target == idx) return true;
            }
            return false;
        }
        if (hovered_node_ >= 0) {
            if (idx == hovered_node_) return true;
            for (const FlowLink& link : links_) {
                if ((link.source == hovered_node_ && link.target == idx) ||
                    (link.target == hovered_node_ && link.source == idx)) {
                    return true;
                }
            }
            return false;
        }
        if (hovered_link_ >= 0 && hovered_link_ < links_.size()) {
            const FlowLink& l = links_.at(hovered_link_);
            return idx == l.source || idx == l.target;
        }
        return false;
    }

    void show_tooltip(const QString& text, const QPoint& global_pos) {
        active_tooltip_ = text;
        tooltip_global_pos_ = global_pos;
        if (!tooltip_refresh_timer_.isActive()) tooltip_refresh_timer_.start();
        QToolTip::showText(global_pos, text, this, rect(), 9000);
    }

    void clear_tooltip() {
        active_tooltip_.clear();
        tooltip_refresh_timer_.stop();
        QToolTip::hideText();
    }

    QString title_;
    QString subtitle_;
    QVector<FlowNode> nodes_;
    QVector<FlowLink> links_;
    QHash<QString, int> node_index_;
    QVector<EventFlowLegendItem> legend_items_;
    QRectF plot_rect_;
    int left_color_index_ = 0;
    int right_color_index_ = 0;

    QTimer anim_timer_;
    qreal anim_progress_ = 0.0;

    QTimer tooltip_refresh_timer_;
    QString active_tooltip_;
    QPoint tooltip_global_pos_;

    int hovered_legend_item_ = -1;
    int hovered_node_ = -1;
    int hovered_link_ = -1;
};

source::data::GeradorEstatisticas* obter_gerador(QObject* iface) {
    if (iface == nullptr) {
        return nullptr;
    }

    const qulonglong ptr = iface->property("_gerador_cpp_ptr").toULongLong();
    if (ptr == 0) {
        return nullptr;
    }
    return reinterpret_cast<source::data::GeradorEstatisticas*>(ptr);
}

QVariant chamar_funcao_grafico(source::data::GeradorEstatisticas* gerador, const QString& func_name) {
    if (gerador == nullptr) {
        return QVariant();
    }

    if (func_name == QStringLiteral("grafico_operacoes_pizza")) return gerador->grafico_operacoes_pizza();
    if (func_name == QStringLiteral("grafico_tipos_arquivo_barras")) return gerador->grafico_tipos_arquivo_barras();
    if (func_name == QStringLiteral("grafico_timeline_operacoes")) return gerador->grafico_timeline_operacoes();
    if (func_name == QStringLiteral("grafico_treemap_tipos")) return gerador->grafico_treemap_tipos();
    if (func_name == QStringLiteral("grafico_histograma_horarios")) return gerador->grafico_histograma_horarios();
    if (func_name == QStringLiteral("grafico_pareto_operacoes")) return gerador->grafico_pareto_operacoes();
    if (func_name == QStringLiteral("grafico_cluster_linha")) return gerador->grafico_cluster_linha();
    if (func_name == QStringLiteral("grafico_boxplot_distribuicao")) return gerador->grafico_boxplot_distribuicao();
    if (func_name == QStringLiteral("grafico_boxplot_distribuicao_por_tipo_operacao")) {
        return gerador->grafico_boxplot_distribuicao_por_tipo_operacao();
    }
    if (func_name == QStringLiteral("grafico_boxplot_distribuicao_por_tipo")) {
        return gerador->grafico_boxplot_distribuicao_por_tipo();
    }
    if (func_name == QStringLiteral("grafico_boxplot_distribuicao_por_hora_dia")) {
        return gerador->grafico_boxplot_distribuicao_por_hora_dia();
    }
    if (func_name == QStringLiteral("grafico_boxplot_distribuicao_por_categoria_tamanho")) {
        return gerador->grafico_boxplot_distribuicao_por_categoria_tamanho();
    }
    if (func_name == QStringLiteral("grafico_eventos_monitorados_por_tipo")) {
        return gerador->grafico_eventos_monitorados_por_tipo();
    }
    if (func_name == QStringLiteral("grafico_eventos_monitorados_por_tipo_operacao")) {
        return gerador->grafico_eventos_monitorados_por_tipo_operacao();
    }
    if (func_name == QStringLiteral("grafico_eventos_monitorados_por_dias_semana")) {
        return gerador->grafico_eventos_monitorados_por_dias_semana();
    }
    if (func_name == QStringLiteral("grafico_eventos_monitorados_por_hora_dia")) {
        return gerador->grafico_eventos_monitorados_por_hora_dia();
    }
    if (func_name == QStringLiteral("grafico_boxplot_eventos")) return gerador->grafico_boxplot_eventos();
    if (func_name == QStringLiteral("grafico_heatmap")) return gerador->grafico_heatmap();
    if (func_name == QStringLiteral("grafico_scatter")) return gerador->grafico_scatter();
    if (func_name == QStringLiteral("grafico_sankey")) return gerador->grafico_sankey();
    if (func_name == QStringLiteral("grafico_sankey_evento_caminho")) return gerador->grafico_sankey_evento_caminho();
    if (func_name == QStringLiteral("grafico_sankey_tipo_caminho")) return gerador->grafico_sankey_tipo_caminho();
    if (func_name == QStringLiteral("grafico_radar")) return gerador->grafico_radar();
    if (func_name == QStringLiteral("grafico_dotplot")) return gerador->grafico_dotplot();
    if (func_name == QStringLiteral("grafico_arvore_diretorios")) return gerador->grafico_arvore_diretorios();

    return QVariant();
}

QWidget* criar_widget_grafico(const QString& titulo, const QVariant& fig, QObject* loc = nullptr) {
    set_localization_context(loc);

    const QVariantMap fig_map = fig.toMap();
    const QString kind = fig_map.value(QStringLiteral("kind")).toString();
    const QString title = fig_map.value(QStringLiteral("title")).toString().trimmed().isEmpty()
                              ? titulo
                              : fig_map.value(QStringLiteral("title")).toString();

    auto to_double = [](const QVariant& value) -> double {
        bool ok = false;
        const double result = value.toDouble(&ok);
        return ok ? result : 0.0;
    };

    auto configure_layout = [](QVBoxLayout* layout) {
        layout->setContentsMargins(8, 8, 8, 8);
        layout->setSpacing(10);
    };

    auto make_message_widget = [&](const QString& message) -> QWidget* {
        auto* container = new QWidget();
        auto* layout = new QVBoxLayout(container);
        configure_layout(layout);

        auto* label = new QLabel(message, container);
        label->setWordWrap(true);
        label->setAlignment(Qt::AlignCenter);
        label->setMinimumHeight(240);
        layout->addWidget(label, 1);
        return container;
    };

    auto wrap_widget = [&](QWidget* child) -> QWidget* {
        auto* container = new QWidget();
        auto* layout = new QVBoxLayout(container);
        configure_layout(layout);
        if (child != nullptr) {
            child->setParent(container);
            layout->addWidget(child, 1);
        }
        return container;
    };

    auto wrap_scroll_widget = [&](QWidget* child) -> QWidget* {
        auto* container = new QWidget();
        auto* layout = new QVBoxLayout(container);
        configure_layout(layout);

        auto* scroll = new QScrollArea(container);
        scroll->setWidgetResizable(true);
        scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        if (child != nullptr) {
            child->setParent(scroll);
            scroll->setWidget(child);
        }
        layout->addWidget(scroll, 1);
        return container;
    };

    if (fig_map.isEmpty()) {
        return make_message_widget(title + QStringLiteral("\n") + ui_no_data_text());
    }

    if (kind == QStringLiteral("empty")) {
        const QString message = fig_map.value(QStringLiteral("message")).toString();
        return make_message_widget(title + QStringLiteral("\n") +
                                   (message.isEmpty() ? ui_no_data_text() : message));
    }

    if (kind == QStringLiteral("boxplot")) {
        auto* tab = new QWidget();
        auto* layout = new QVBoxLayout(tab);
        configure_layout(layout);

        auto* grid = new QGridLayout();
        grid->setContentsMargins(0, 0, 0, 0);
        grid->setHorizontalSpacing(10);
        grid->setVerticalSpacing(10);

        const QVariantMap by_operation = fig_map.value(QStringLiteral("by_operation")).toMap();
        const QVariantList operation_order = fig_map.value(QStringLiteral("operation_order")).toList();
        const QVariantMap op_colors = fig_map.value(QStringLiteral("op_colors")).toMap();
        const QVariantMap by_type = fig_map.value(QStringLiteral("by_type")).toMap();
        const QVariantList top_types = fig_map.value(QStringLiteral("top_types")).toList();
        const QVariantMap by_size_category = fig_map.value(QStringLiteral("by_size_category")).toMap();
        const QVariantList category_order = fig_map.value(QStringLiteral("category_order")).toList();

        QVariantMap by_hour;
        QVariantList hour_order;
        for (const QVariant& hour_value : fig_map.value(QStringLiteral("by_hour_daily_counts")).toList()) {
            const QVariantMap hour_map = hour_value.toMap();
            const QString hour_label = hour_map.value(QStringLiteral("hour")).toString();
            const QVariantList counts = hour_map.value(QStringLiteral("counts")).toList();
            if (hour_label.isEmpty() || counts.isEmpty()) {
                continue;
            }
            by_hour.insert(hour_label, counts);
            hour_order.append(hour_label);
        }

        struct BoxCell {
            QString title;
            QVariantMap sourceMap;
            QVariantList orderHint;
            QVariantMap colorMap;
            bool yLogScale = false;
            QString xLabel;
            QString yLabel;
            int maxItems = -1;
        };

        const QVector<BoxCell> cells = {
            {ui_text("operation_type", QStringLiteral("Tipo de Operação")), by_operation, operation_order, op_colors,
             true, ui_text("operation_type", QStringLiteral("Tipo de Operação")),
             ui_text("size_megabytes_mb", QStringLiteral("Tamanho (Megabytes) (MB)")), 10},
            {ui_text("type", QStringLiteral("Tipo")), by_type, top_types, QVariantMap(), true,
             ui_text("type", QStringLiteral("Tipo")),
             ui_text("size_megabytes_mb", QStringLiteral("Tamanho (Megabytes) (MB)")), 8},
            {ui_text("hour_of_day", QStringLiteral("Hora do Dia")), by_hour, hour_order, QVariantMap(), false,
             ui_text("hour_of_day", QStringLiteral("Hora do Dia")),
             ui_text("size_megabytes_mb", QStringLiteral("Tamanho (Megabytes) (MB)")), 24},
            {ui_text("size_categories", QStringLiteral("Categorias de Tamanho")), by_size_category, category_order,
             QVariantMap(), true, ui_text("category", QStringLiteral("Categoria")),
             ui_text("size_megabytes_mb", QStringLiteral("Tamanho (Megabytes) (MB)")), 5}};

        for (int i = 0; i < cells.size(); ++i) {
            QWidget* cell_widget = nullptr;
            if (!cells.at(i).sourceMap.isEmpty()) {
                const QVariantMap box_figure = source::charts_lgpl::make_boxplot_single_figure(
                    cells.at(i).title, cells.at(i).sourceMap, cells.at(i).orderHint, cells.at(i).colorMap,
                    cells.at(i).yLogScale, cells.at(i).xLabel, cells.at(i).yLabel, cells.at(i).maxItems);
                cell_widget = source::charts_lgpl::create_legacy_raster_chart_widget(cells.at(i).title, box_figure, tab);
            } else {
                cell_widget = make_message_widget(cells.at(i).title + QStringLiteral("\n") + ui_no_data_text());
            }

            cell_widget->setMinimumHeight(300);
            grid->addWidget(cell_widget, i / 2, i % 2);
        }

        layout->addLayout(grid);
        return tab;
    }

    if (kind == QStringLiteral("radar_eventos")) {
        auto parse_summary = [&](const QVariantMap& summary) {
            QPair<QStringList, QVector<double>> result;
            const QVariantList labels_raw = summary.value(QStringLiteral("labels")).toList();
            const QVariantList values_raw = summary.value(QStringLiteral("values")).toList();
            const int count = qMin(labels_raw.size(), values_raw.size());
            for (int i = 0; i < count; ++i) {
                const QString label = labels_raw.at(i).toString().trimmed();
                if (label.isEmpty()) {
                    continue;
                }
                result.first.append(label);
                result.second.append(to_double(values_raw.at(i)));
            }

            if (!result.first.isEmpty()) {
                return result;
            }

            for (auto it = summary.constBegin(); it != summary.constEnd(); ++it) {
                if (it.key() == QStringLiteral("labels") || it.key() == QStringLiteral("values")) {
                    continue;
                }
                const QString label = it.key().trimmed();
                if (label.isEmpty()) {
                    continue;
                }
                result.first.append(label);
                result.second.append(to_double(it.value()));
            }
            return result;
        };

        auto has_values = [](const QVector<double>& values) {
            for (double value : values) {
                if (value > 0.0) {
                    return true;
                }
            }
            return false;
        };

        const auto types = parse_summary(fig_map.value(QStringLiteral("top_types")).toMap());
        const auto operations = parse_summary(fig_map.value(QStringLiteral("top_operations")).toMap());

        QVector<double> weekday_values(7, 0.0);
        const QVariantList by_weekday = fig_map.value(QStringLiteral("by_weekday")).toList();
        for (int i = 0; i < qMin(7, by_weekday.size()); ++i) {
            weekday_values[i] = to_double(by_weekday.at(i));
        }

        QVector<double> hourly_values(24, 0.0);
        const QVariantList by_hour = fig_map.value(QStringLiteral("by_hour")).toList();
        for (int i = 0; i < qMin(24, by_hour.size()); ++i) {
            hourly_values[i] = to_double(by_hour.at(i));
        }

        const QStringList weekday_labels = {
            ui_text("weekday_monday", QStringLiteral("Segunda")),
            ui_text("weekday_tuesday", QStringLiteral("Terça")),
            ui_text("weekday_wednesday", QStringLiteral("Quarta")),
            ui_text("weekday_thursday", QStringLiteral("Quinta")),
            ui_text("weekday_friday", QStringLiteral("Sexta")),
            ui_text("weekday_saturday", QStringLiteral("Sábado")),
            ui_text("weekday_sunday", QStringLiteral("Domingo"))};

        QStringList hourly_labels;
        hourly_labels.reserve(24);
        for (int hour = 0; hour < 24; ++hour) {
            hourly_labels.append(ui_text("hour_format", QStringLiteral("%1h")).arg(hour));
        }

        struct RadarCell {
            QString title;
            QStringList labels;
            QVector<double> values;
            QColor color;
        };

        const QVector<RadarCell> cells = {
            {ui_text("type", QStringLiteral("Tipo")), types.first, types.second, QColor(QStringLiteral("#2563eb"))},
            {ui_text("operation_type", QStringLiteral("Tipo de Operação")), operations.first, operations.second,
             QColor(QStringLiteral("#dc2626"))},
            {ui_text("week_days", QStringLiteral("Dias da Semana")), weekday_labels, weekday_values,
             QColor(QStringLiteral("#059669"))},
            {ui_text("hour_of_day", QStringLiteral("Hora do Dia")), hourly_labels, hourly_values,
             QColor(QStringLiteral("#7c3aed"))}};

        auto* tab = new QWidget();
        auto* layout = new QVBoxLayout(tab);
        configure_layout(layout);

        auto* grid = new QGridLayout();
        grid->setContentsMargins(0, 0, 0, 0);
        grid->setHorizontalSpacing(10);
        grid->setVerticalSpacing(10);

        for (int i = 0; i < cells.size(); ++i) {
            QWidget* cell_widget = nullptr;
            if (!cells.at(i).labels.isEmpty() && has_values(cells.at(i).values)) {
                const QVariantMap radar_figure = source::charts_lgpl::make_radar_single_figure(
                    cells.at(i).title, cells.at(i).labels, cells.at(i).values, cells.at(i).color, cells.at(i).title);
                cell_widget = source::charts_lgpl::create_legacy_raster_chart_widget(cells.at(i).title, radar_figure, tab);
            } else {
                cell_widget = make_message_widget(cells.at(i).title + QStringLiteral("\n") + ui_no_data_text());
            }

            cell_widget->setMinimumHeight(320);
            grid->addWidget(cell_widget, i / 2, i % 2);
        }

        layout->addLayout(grid);
        return tab;
    }

    if (kind == QStringLiteral("treemap")) {
        const QVariantList labels = fig_map.value(QStringLiteral("labels")).toList();
        const QVariantList values = fig_map.value(QStringLiteral("values")).toList();
        const QVariantList colors = fig_map.value(QStringLiteral("colors")).toList();
        const int count = qMin(labels.size(), values.size());

        QList<TreemapItem> items;
        items.reserve(count);
        double min_value = std::numeric_limits<double>::infinity();
        double max_value = 0.0;

        for (int i = 0; i < count; ++i) {
            const QString label = labels.at(i).toString().trimmed();
            const double value = to_double(values.at(i));
            if (label.isEmpty() || value <= 0.0) {
                continue;
            }

            TreemapItem item{label, value, value, QColor()};
            if (i < colors.size()) {
                const QColor explicit_color(colors.at(i).toString());
                if (explicit_color.isValid()) {
                    item.color = explicit_color;
                }
            }

            min_value = qMin(min_value, value);
            max_value = qMax(max_value, value);
            items.append(item);
        }

        if (items.isEmpty()) {
            return make_message_widget(title + QStringLiteral("\n") + ui_no_data_text());
        }

        for (TreemapItem& item : items) {
            if (item.color.isValid()) {
                continue;
            }
            const double normalized = (max_value > min_value)
                                          ? ((item.area - min_value) / (max_value - min_value))
                                          : 0.5;
            item.color = cor_viridis(normalized);
        }

        return wrap_widget(new TreemapWidget(title, items));
    }

    if (kind == QStringLiteral("heatmap")) {
        const QVariantList rows = fig_map.value(QStringLiteral("matrix")).toList();
        if (rows.isEmpty()) {
            return make_message_widget(title + QStringLiteral("\n") + ui_no_data_text());
        }

        int columns = 0;
        for (const QVariant& row_value : rows) {
            columns = qMax(columns, row_value.toList().size());
        }
        if (columns <= 0) {
            return make_message_widget(title + QStringLiteral("\n") + ui_no_data_text());
        }

        QVector<QVector<double>> matrix(rows.size(), QVector<double>(columns, 0.0));
        QStringList row_labels;
        for (const QVariant& label : fig_map.value(QStringLiteral("day_labels")).toList()) {
            row_labels.append(label.toString());
        }

        for (int row = 0; row < rows.size(); ++row) {
            const QVariantList row_values = rows.at(row).toList();
            for (int column = 0; column < qMin(columns, row_values.size()); ++column) {
                matrix[row][column] = to_double(row_values.at(column));
            }
        }

        const QString xlabel = fig_map.value(QStringLiteral("xlabel")).toString().trimmed().isEmpty()
                                   ? ui_text("time_label", QStringLiteral("Tempo"))
                                   : fig_map.value(QStringLiteral("xlabel")).toString();
        const QString ylabel = fig_map.value(QStringLiteral("ylabel")).toString().trimmed().isEmpty()
                                   ? ui_text("week_days", QStringLiteral("Dias da Semana"))
                                   : fig_map.value(QStringLiteral("ylabel")).toString();

        return wrap_widget(new TemporalHeatmapWidget(title, xlabel, ylabel, row_labels, matrix));
    }

    if (kind == QStringLiteral("arvore_diretorios")) {
        if (fig_map.value(QStringLiteral("links")).toList().isEmpty()) {
            return make_message_widget(title + QStringLiteral("\n") + ui_no_data_text());
        }
        return wrap_widget(new DirectoryTreeFlowWidget(fig_map));
    }

    if (kind == QStringLiteral("sankey") || kind == QStringLiteral("sankey_evento_caminho") ||
        kind == QStringLiteral("sankey_tipo_caminho")) {
        if (fig_map.value(QStringLiteral("links")).toList().isEmpty()) {
            return make_message_widget(title + QStringLiteral("\n") + ui_no_data_text());
        }
        return wrap_scroll_widget(new EventDirectoryFlowWidget(fig_map));
    }

    if (source::charts_lgpl::supports_legacy_kind(kind)) {
        return source::charts_lgpl::create_legacy_raster_chart_widget(title, fig, nullptr);
    }

    const QVariantList labels = fig_map.value(QStringLiteral("labels")).toList();
    const QVariantList values = fig_map.value(QStringLiteral("values")).toList();
    if (!labels.isEmpty() && !values.isEmpty()) {
        QVariantMap fallback_figure = fig_map;
        fallback_figure.insert(QStringLiteral("kind"), QStringLiteral("barras"));
        if (fallback_figure.value(QStringLiteral("title")).toString().trimmed().isEmpty()) {
            fallback_figure.insert(QStringLiteral("title"), title);
        }
        return source::charts_lgpl::create_legacy_raster_chart_widget(title, fallback_figure, nullptr);
    }

    return make_message_widget(title + QStringLiteral("\n") + ui_no_data_text());
}

QWidget* criar_placeholder_grafico(const QString& titulo, const QVariant& fig, QObject* loc, QWidget* parent) {
    auto* container = new QWidget(parent);
    auto* layout = new QVBoxLayout(container);
    layout->setContentsMargins(28, 28, 28, 28);
    layout->setSpacing(10);

    auto* title_label = new QLabel(titulo, container);
    title_label->setAlignment(Qt::AlignCenter);
    title_label->setWordWrap(true);
    QFont title_font = title_label->font();
    title_font.setBold(true);
    title_font.setPointSize(qMax(12, title_font.pointSize() + 2));
    title_label->setFont(title_font);
    layout->addWidget(title_label);

    auto* info_label =
        new QLabel(get_text(loc, "chart_load_on_demand",
                            QStringLiteral("O gráfico será carregado quando esta aba for aberta.")),
                   container);
    info_label->setAlignment(Qt::AlignCenter);
    info_label->setWordWrap(true);
    layout->addWidget(info_label);
    layout->addStretch(1);

    container->setProperty(kStatsPendingChartFigureProperty, fig);
    container->setProperty(kStatsPendingChartTitleProperty, titulo);
    container->setProperty(kStatsPendingChartLoadedProperty, false);
    return container;
}

bool aba_grafico_pendente(QWidget* tab) {
    if (tab == nullptr) return false;
    if (!tab->property(kStatsPendingChartFigureProperty).isValid()) return false;
    return !tab->property(kStatsPendingChartLoadedProperty).toBool();
}

void materializar_aba_grafico(QTabWidget* tab_widget, int index, QObject* loc) {
    if (tab_widget == nullptr || index < 0 || index >= tab_widget->count()) {
        return;
    }

    QWidget* atual = tab_widget->widget(index);
    if (!aba_grafico_pendente(atual)) {
        return;
    }

    const QVariant fig = atual->property(kStatsPendingChartFigureProperty);
    const QString titulo = atual->property(kStatsPendingChartTitleProperty).toString();
    QWidget* novo = criar_widget_grafico(titulo, fig, loc);
    if (novo == nullptr) {
        return;
    }

    novo->setProperty(kStatsPendingChartLoadedProperty, true);
    const QString titulo_aba = tab_widget->tabText(index);
    const int indice_atual = tab_widget->currentIndex();
    {
        QSignalBlocker blocker(tab_widget);
        tab_widget->removeTab(index);
        tab_widget->insertTab(index, novo, titulo_aba);
        tab_widget->setCurrentIndex(indice_atual >= 0 ? indice_atual : index);
    }
    atual->deleteLater();
}

void garantir_materializacao_abas(source::ui::GerenciadorEstatisticasUI* self) {
    if (self == nullptr || self->interface_obj() == nullptr) {
        return;
    }

    auto* tab_widget = self->interface_obj()->property("tab_widget").value<QTabWidget*>();
    if (tab_widget == nullptr || tab_widget->property(kStatsTabMaterializerConnectedProperty).toBool()) {
        return;
    }

    QObject::connect(tab_widget, &QTabWidget::currentChanged, tab_widget, [self, tab_widget](int index) {
        QTimer::singleShot(0, tab_widget, [self, tab_widget, index]() {
            materializar_aba_grafico(tab_widget, index, self ? self->loc_obj() : nullptr);
        });
    });
    tab_widget->setProperty(kStatsTabMaterializerConnectedProperty, true);
}

void atualizar_progresso_preparacao(source::ui::GerenciadorEstatisticasUI* self, int atual, int total) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        if (auto* progress = iface->property("progress_dialog").value<QProgressDialog*>()) {
            progress->setValue(atual);
            progress->setLabelText(
                QString("%1 %2/%3")
                    .arg(get_text(self->loc_obj(), "preparing_chart_data", "Preparando dados do gráfico"))
                    .arg(atual)
                    .arg(total));
            QApplication::processEvents();
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar progresso de preparação", true);
    }
}

void enfileirar_grafico(source::ui::GerenciadorEstatisticasUI* self,
                        const QString& titulo,
                        const QVariant& dados,
                        const QVariant& func,
                        const QString& titulo_original) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        QVariantList fila = iface->property("fila_graficos").toList();

        QVariantMap grafico;
        grafico.insert("titulo", titulo);
        grafico.insert("dados", dados);
        grafico.insert("func", func);
        grafico.insert("titulo_original", titulo_original);
        fila.append(grafico);

        iface->setProperty("fila_graficos", fila);
    } catch (...) {
        source::utils::LogManager::get_logger().error(QString("Erro ao enfileirar gráfico %1").arg(titulo), true);
    }
}

void finalizar_processo(source::ui::GerenciadorEstatisticasUI* self, qulonglong generation_id) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        if (!generation_id_valido(iface, generation_id)) {
            return;
        }
        sincronizar_graficos_idioma_ativo(iface, self->loc_obj());
        iface->setProperty("gerando_grafico", false);
        iface->setProperty(kStatsPreparationFinishedProperty, false);

        if (auto* progress = iface->property("progress_dialog").value<QProgressDialog*>()) {
            progress->close();
            progress->deleteLater();
            iface->setProperty("progress_dialog", QVariant());
        }

        if (auto* worker = iface->property("worker").value<source::ui::GerenciamentoEstatisticasUI::GraficoWorker*>()) {
            if (!worker->isRunning()) {
                worker->deleteLater();
                iface->setProperty("worker", QVariant());
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao finalizar processo", true);
    }
}

void processar_proximo_grafico(source::ui::GerenciadorEstatisticasUI* self, qulonglong generation_id);
void gerar_e_adicionar_grafico(source::ui::GerenciadorEstatisticasUI* self,
                               const QVariantMap& grafico,
                               qulonglong generation_id);
void cancelar_processo(source::ui::GerenciadorEstatisticasUI* self);

QObject* contexto_agendamento_estatisticas(QObject* iface) {
    if (iface == nullptr) {
        return nullptr;
    }
    QObject* dialog = iface->property("dialog_estatisticas").value<QObject*>();
    return dialog != nullptr ? dialog : iface;
}

bool garantir_janelas_estatisticas_visiveis(QObject* iface) {
    if (iface == nullptr) {
        return false;
    }

    auto* dialog = iface->property("dialog_estatisticas").value<QDialog*>();
    if (dialog == nullptr) {
        return false;
    }

    if (dialog->isMinimized()) {
        dialog->showNormal();
    }
    if (!dialog->isVisible()) {
        return false;
    }

    if (auto* progress = iface->property("progress_dialog").value<QProgressDialog*>()) {
        if (progress->isMinimized()) {
            progress->showNormal();
        } else if (!progress->isVisible()) {
            progress->show();
        }
        progress->raise();
    }

    return dialog->isVisible();
}

void agendar_processamento_proximo_grafico(source::ui::GerenciadorEstatisticasUI* self,
                                           int delay_ms,
                                           qulonglong generation_id) {
    if (self == nullptr || self->interface_obj() == nullptr) {
        return;
    }

    QObject* contexto = contexto_agendamento_estatisticas(self->interface_obj());
    if (contexto != nullptr) {
        QTimer::singleShot(delay_ms, contexto, [self, generation_id]() {
            if (!self || !self->interface_obj() || !generation_id_valido(self->interface_obj(), generation_id)) {
                return;
            }
            processar_proximo_grafico(self, generation_id);
        });
        return;
    }

    QTimer::singleShot(delay_ms, [self, generation_id]() {
        if (!self || !self->interface_obj() || !generation_id_valido(self->interface_obj(), generation_id)) {
            return;
        }
        processar_proximo_grafico(self, generation_id);
    });
}

void agendar_geracao_grafico(source::ui::GerenciadorEstatisticasUI* self,
                             const QVariantMap& grafico,
                             int delay_ms,
                             qulonglong generation_id) {
    if (self == nullptr || self->interface_obj() == nullptr) {
        return;
    }

    QObject* contexto = contexto_agendamento_estatisticas(self->interface_obj());
    if (contexto != nullptr) {
        QTimer::singleShot(delay_ms, contexto, [self, grafico, generation_id]() {
            if (!self || !self->interface_obj() || !generation_id_valido(self->interface_obj(), generation_id)) {
                return;
            }
            gerar_e_adicionar_grafico(self, grafico, generation_id);
        });
        return;
    }

    QTimer::singleShot(delay_ms, [self, grafico, generation_id]() {
        if (!self || !self->interface_obj() || !generation_id_valido(self->interface_obj(), generation_id)) {
            return;
        }
        gerar_e_adicionar_grafico(self, grafico, generation_id);
    });
}

void gerar_e_adicionar_grafico(source::ui::GerenciadorEstatisticasUI* self,
                               const QVariantMap& grafico,
                               qulonglong generation_id) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        if (!generation_id_valido(iface, generation_id)) {
            return;
        }
        if (!garantir_janelas_estatisticas_visiveis(iface)) {
            cancelar_processo(self);
            return;
        }
        auto* tab_widget = iface->property("tab_widget").value<QTabWidget*>();
        if (tab_widget == nullptr) {
            return;
        }

        const QString titulo = grafico.value("titulo").toString();
        const QString titulo_original = grafico.value("titulo_original").toString();
        const QString func_name = grafico.value("func").toString();
        QVariant fig = grafico.value("dados");
        if (!fig.isValid() || fig.toMap().isEmpty()) {
            source::data::GeradorEstatisticas* gerador = obter_gerador(iface);
            fig = chamar_funcao_grafico(gerador, func_name);
        }

        QVariantHash dados = iface->property("graficos_dados").toHash();
        QVariantMap entry;
        entry.insert("fig", fig);
        entry.insert("func", func_name);
        entry.insert("titulo", titulo_original);
        dados.insert(titulo, entry);
        iface->setProperty("graficos_dados", dados);
        sincronizar_graficos_idioma_ativo(iface, self ? self->loc_obj() : nullptr);

        QWidget* placeholder = criar_placeholder_grafico(titulo, fig, self ? self->loc_obj() : nullptr, tab_widget);
        const int tab_index = tab_widget->addTab(placeholder, titulo);
        if (tab_widget->currentIndex() < 0) {
            tab_widget->setCurrentIndex(tab_index);
        }
        iface->setProperty("graficos_processados", iface->property("graficos_processados").toInt() + 1);
        garantir_janelas_estatisticas_visiveis(iface);
        if (tab_widget->currentIndex() == tab_index) {
            QTimer::singleShot(0, tab_widget, [self, tab_widget, tab_index]() {
                materializar_aba_grafico(tab_widget, tab_index, self ? self->loc_obj() : nullptr);
            });
        }
        QApplication::processEvents();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao gerar gráfico", true);
        if (self && self->interface_obj()) {
            QObject* iface = self->interface_obj();
            QWidget* parent = iface->property("dialog_estatisticas").value<QWidget*>();
            QMessageBox::warning(parent, get_text(self->loc_obj(), "error", "Erro"),
                                 QString("%1 %2")
                                     .arg(get_text(self->loc_obj(), "error_generating_chart",
                                                   "Erro ao gerar gráfico"))
                                     .arg(grafico.value("titulo").toString()));
        }
    }

    if (self && self->interface_obj()) {
        QObject* iface = self->interface_obj();
        if (generation_id_valido(iface, generation_id)) {
            iface->setProperty("gerando_grafico", false);
        }
    }
    agendar_processamento_proximo_grafico(self, 50, generation_id);
}

void processar_proximo_grafico(source::ui::GerenciadorEstatisticasUI* self, qulonglong generation_id) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        if (!generation_id_valido(iface, generation_id)) {
            return;
        }
        if (iface->property("gerando_grafico").toBool()) {
            return;
        }
        if (!garantir_janelas_estatisticas_visiveis(iface)) {
            cancelar_processo(self);
            return;
        }

        QVariantList fila = iface->property("fila_graficos").toList();
        if (fila.isEmpty()) {
            iface->setProperty("gerando_grafico", false);
            if (iface->property(kStatsPreparationFinishedProperty).toBool()) {
                finalizar_processo(self, generation_id);
            }
            return;
        }

        iface->setProperty("gerando_grafico", true);
        const QVariantMap grafico = fila.takeFirst().toMap();
        iface->setProperty("fila_graficos", fila);

        agendar_geracao_grafico(self, grafico, 50, generation_id);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao processar próximo gráfico", true);
        if (self && self->interface_obj()) {
            if (generation_id_valido(self->interface_obj(), generation_id)) {
                self->interface_obj()->setProperty("gerando_grafico", false);
            }
        }
        agendar_processamento_proximo_grafico(self, 50, generation_id);
    }
}

void iniciar_geracao_graficos(source::ui::GerenciadorEstatisticasUI* self, qulonglong generation_id) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        if (!generation_id_valido(iface, generation_id)) {
            return;
        }
        iface->setProperty(kStatsPreparationFinishedProperty, true);
        if (!iface->property("gerando_grafico").toBool() &&
            iface->property("fila_graficos").toList().isEmpty()) {
            finalizar_processo(self, generation_id);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao iniciar geração de gráficos", true);
    }
}

void cancelar_processo(source::ui::GerenciadorEstatisticasUI* self) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        avancar_generation_id(iface);
        liberar_worker_estatisticas(iface);

        iface->setProperty("fila_graficos", QVariantList());
        iface->setProperty("gerando_grafico", false);
        iface->setProperty(kStatsPreparationFinishedProperty, false);

        if (auto* progress = iface->property("progress_dialog").value<QProgressDialog*>()) {
            progress->close();
            progress->deleteLater();
            iface->setProperty("progress_dialog", QVariant());
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao cancelar processo", true);
    }
}

void tratar_erro_grafico(source::ui::GerenciadorEstatisticasUI* self,
                         const QString& titulo,
                         const QString& erro) {
    source::utils::LogManager::get_logger().error(
        QString("Erro recebido do worker para gráfico %1: %2").arg(titulo, erro), true);

    if (!self || !self->interface_obj()) return;
    QWidget* parent = self->interface_obj()->property("dialog_estatisticas").value<QWidget*>();
    QMessageBox::warning(
        parent, get_text(self->loc_obj(), "error", "Erro"),
        QString("%1 %2: %3")
            .arg(get_text(self->loc_obj(), "error_preparing_chart", "Erro ao preparar dados do gráfico"))
            .arg(titulo)
            .arg(erro));
}
}

namespace source::ui::GerenciamentoEstatisticasUI {

QWidget* criar_widget_grafico_localizado(const QString& titulo, const QVariant& fig, QObject* loc) {
    return criar_widget_grafico(titulo, fig, loc);
}

void gerar_todos_graficos(source::ui::GerenciadorEstatisticasUI* self, const QVariantList& graficos) {
    try {
        if (!self || !self->interface_obj()) return;
        set_localization_context(self->loc_obj());
        QObject* iface = self->interface_obj();
        auto* tab_widget = iface->property("tab_widget").value<QTabWidget*>();
        if (!tab_widget) return;
        garantir_materializacao_abas(self);

        cancelar_processo(self);
        const qulonglong generation_id = avancar_generation_id(iface);

        while (tab_widget->count() > 0) {
            QWidget* w = tab_widget->widget(0);
            tab_widget->removeTab(0);
            if (w) w->deleteLater();
        }

        iface->setProperty("graficos_dados", QVariantHash());
        sincronizar_graficos_idioma_ativo(iface, self ? self->loc_obj() : nullptr);
        iface->setProperty("fila_graficos", QVariantList());
        iface->setProperty("gerando_grafico", false);
        iface->setProperty("total_graficos", graficos.size());
        iface->setProperty("graficos_processados", 0);
        iface->setProperty(kStatsPreparationFinishedProperty, false);

        auto* dialog = iface->property("dialog_estatisticas").value<QDialog*>();
        auto* progress_dialog = new QProgressDialog(
            QString("%1 0/%2")
                .arg(get_text(self->loc_obj(), "preparing_chart_data", "Preparando dados do gráfico"))
                .arg(graficos.size()),
            get_text(self->loc_obj(), "cancel", "Cancelar"),
            0,
            graficos.size(),
            dialog);
        progress_dialog->setWindowTitle(get_text(self->loc_obj(), "please_wait", "Aguarde"));
        progress_dialog->setModal(false);
        progress_dialog->setMinimumDuration(0);
        progress_dialog->setAutoClose(false);
        progress_dialog->setAutoReset(false);
        progress_dialog->setProperty("interface_principal", QVariant::fromValue(iface));
        iface->setProperty("progress_dialog", QVariant::fromValue(progress_dialog));
        progress_dialog->setValue(0);
        progress_dialog->show();
        progress_dialog->raise();

        QString db_path = iface->property(kStatsDbPathProperty).toString();
        if (db_path.isEmpty()) {
            db_path = iface->property("_monitoramento_db_path").toString();
        }
        auto* worker = new GraficoWorker(graficos, db_path, self->loc_obj(), iface, iface);
        iface->setProperty("worker", QVariant::fromValue(worker));

        QObject::connect(worker, &GraficoWorker::dados_grafico_prontos, iface,
                         [self, generation_id](const QString& titulo, const QVariant& dados, const QVariant& func, const QString& titulo_original) {
                             if (!self || !self->interface_obj() ||
                                 !generation_id_valido(self->interface_obj(), generation_id)) {
                                 return;
                             }
                             enfileirar_grafico(self, titulo, dados, func, titulo_original);
                             if (!self->interface_obj()->property("gerando_grafico").toBool()) {
                                 agendar_processamento_proximo_grafico(self, 0, generation_id);
                             }
                         });
        QObject::connect(worker, &GraficoWorker::todos_concluidos, iface, [self, generation_id]() {
            if (!self || !self->interface_obj() ||
                !generation_id_valido(self->interface_obj(), generation_id)) {
                return;
            }
            iniciar_geracao_graficos(self, generation_id);
        });
        QObject::connect(worker, &GraficoWorker::progresso_atualizado, iface,
                         [self, generation_id](int atual, int total) {
                             if (!self || !self->interface_obj() ||
                                 !generation_id_valido(self->interface_obj(), generation_id)) {
                                 return;
                             }
                             atualizar_progresso_preparacao(self, atual, total);
                         });
        QObject::connect(worker, &GraficoWorker::erro_ocorrido, iface,
                         [self, generation_id](const QString& titulo, const QString& erro) {
                             if (!self || !self->interface_obj() ||
                                 !generation_id_valido(self->interface_obj(), generation_id)) {
                                 return;
                             }
                             tratar_erro_grafico(self, titulo, erro);
                         });
        QObject::connect(progress_dialog, &QProgressDialog::canceled, iface, [self]() { cancelar_processo(self); });

        worker->start();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao gerar todos os gráficos", true);
    }
}

void cancelar_geracao_graficos(source::ui::GerenciadorEstatisticasUI* self) {
    cancelar_processo(self);
}

bool renderizar_grafico_para_imagem(
    const QString& titulo,
    const QVariant& fig,
    const QString& arquivo_destino,
    const QSize& tamanho,
    QObject* loc) {
    try {
        ScopedExportRenderMode export_mode_scope(true);
        set_localization_context(loc);
        const QSize tamanho_base = tamanho.isValid() ? tamanho : QSize(1920, 1200);
        std::unique_ptr<QWidget> widget(criar_widget_grafico(titulo, fig, loc != nullptr ? loc : localization_context_obj()));
        if (!widget) {
            return false;
        }

        if (auto* root_view = qobject_cast<source::charts_lgpl::RasterChartView*>(widget.get())) {
            root_view->setExportMode(true);
            root_view->setAnimationsEnabled(false);
        }
        const auto raster_views = widget->findChildren<source::charts_lgpl::RasterChartView*>();
        for (source::charts_lgpl::RasterChartView* view : raster_views) {
            if (view == nullptr) {
                continue;
            }
            view->setExportMode(true);
            view->setAnimationsEnabled(false);
        }

        widget->setAttribute(Qt::WA_DontShowOnScreen, true);
        QSize tamanho_final = tamanho_base;
        const auto expand_size = [&tamanho_final](const QSize& candidate) {
            if (candidate.isValid()) {
                tamanho_final = tamanho_final.expandedTo(candidate);
            }
        };
        expand_size(widget->minimumSizeHint());
        expand_size(widget->sizeHint());
        if (auto* layout = widget->layout()) {
            layout->activate();
            expand_size(layout->minimumSize());
            expand_size(layout->sizeHint());
        }
        widget->resize(tamanho_final);
        widget->setMinimumSize(tamanho_final);
        widget->setMaximumSize(tamanho_final);
        widget->ensurePolished();
        if (auto* layout = widget->layout()) {
            layout->activate();
        }
        widget->updateGeometry();
        widget->show();
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        widget->update();
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        QImage image(tamanho_final, QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::white);
        {
            QPainter painter(&image);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setRenderHint(QPainter::TextAntialiasing, true);
            widget->render(&painter, QPoint(), QRegion(), QWidget::DrawWindowBackground | QWidget::DrawChildren);
            painter.end();
        }

        return image.save(arquivo_destino, "PNG");
    } catch (...) {
        source::utils::LogManager::get_logger().error(
            QString("Erro ao renderizar gráfico para imagem: %1").arg(titulo),
            true);
        return false;
    }
}

}
