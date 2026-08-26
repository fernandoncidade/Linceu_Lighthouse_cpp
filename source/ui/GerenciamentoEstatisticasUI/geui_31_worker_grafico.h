#pragma once

#include <atomic>

#include <QPointer>
#include <QString>
#include <QThread>
#include <QVariantList>

namespace source::data {
class GeradorEstatisticas;
}

namespace source::ui::GerenciamentoEstatisticasUI {

class GraficoWorker : public QThread {
    Q_OBJECT
public:
    explicit GraficoWorker(const QVariantList& graficos,
                           const QString& db_path,
                           QObject* localizador = nullptr,
                           QObject* interface_principal = nullptr,
                           QObject* parent = nullptr);
    void parar();

signals:
    void dados_grafico_prontos(const QString& titulo, const QVariant& dados, const QVariant& func, const QString& titulo_original);
    void todos_concluidos();
    void progresso_atualizado(int atual, int total);
    void erro_ocorrido(const QString& titulo, const QString& erro);

protected:
    void run() override;

private:
    QVariant preparar_dados_grafico(const QVariant& func_gerador,
                                    source::data::GeradorEstatisticas* gerador) const;

private:
    QVariantList graficos_;
    QString db_path_;
    QPointer<QObject> loc_;
    QPointer<QObject> interface_;
    std::atomic_bool parar_flag_;
};

}
