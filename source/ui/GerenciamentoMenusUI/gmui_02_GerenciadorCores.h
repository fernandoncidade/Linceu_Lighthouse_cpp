#pragma once

#include <QObject>
#include <QHash>
#include <QString>
#include <QThread>

class QColor;
class QIcon;
class QWidget;

namespace source::ui::GerenciamentoMenusUI {

QString get_palette_icon_path(const QString& tipo = QStringLiteral("paleta"));

class GerenciadorCores;

class ThreadAtualizarCores : public QThread {
    Q_OBJECT
public:
    explicit ThreadAtualizarCores(GerenciadorCores* gerenciador_cores, QObject* interface_obj);
    void run() override;

signals:
    void sinal_concluido();

private:
    GerenciadorCores* gerenciador_cores_;
    QObject* interface_;
};

class GerenciadorCores : public QObject {
    Q_OBJECT
public:
    explicit GerenciadorCores(QObject* interface_principal, QObject* parent = nullptr);

    void carregar_cores();
    bool salvar_cores();
    QString obter_cor_hex(const QString& tipo_operacao) const;
    QColor obter_cor_qcolor(const QString& tipo_operacao) const;
    bool definir_cor(const QString& tipo_operacao, const QString& cor_hex);
    void atualizar_cores_no_sistema();
    void atualizar_cores_no_sistema_threadsafe();

    static void aplicar_icone_paleta(QWidget* dialog, const QString& tipo = QStringLiteral("paleta"));

private:
    QString obter_diretorio_config() const;

private:
    QObject* interface_;
    QObject* loc_;
    ThreadAtualizarCores* thread_cores_;
    QHash<QString, QString> cores_operacoes_;
};

}
