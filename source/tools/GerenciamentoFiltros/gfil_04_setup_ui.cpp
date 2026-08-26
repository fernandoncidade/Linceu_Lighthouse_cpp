#include "gfil_04_setup_ui.h"

#include <QCheckBox>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>
#include <QSizePolicy>
#include <QVBoxLayout>

#include "gfil_06_sincronizar_com_menu_principal.h"
#include "source/tools/fil_01_Filtros.h"
#include "source/tools/fil_02_AdministradorCalendario.h"
#include "source/tools/fil_03_AdministradorFiltros.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/IconUtils.h"
#include "source/utils/LogManager.h"

namespace {
QString gt(source::tools::Filtros* self, const QString& key, const QString& fallback = QString()) {
    if (!self || !self->localizador()) return fallback;
    QString t = self->localizador()->get_text(key);
    return t.isEmpty() ? fallback : t;
}
}

namespace source::tools::GerenciamentoFiltros {

void setup_ui(source::tools::Filtros* self) {
    try {
        if (!self) return;

        auto* layout = new QVBoxLayout(self);

        auto* grupo_operacao = new QGroupBox(gt(self, "operation_filter", "Filtro de operação"), self);
        grupo_operacao->setObjectName(QStringLiteral("grupo_operacao"));
        auto* layout_operacao = new QVBoxLayout(grupo_operacao);

        QVariantMap checkboxes_operacao;
        const QStringList ops = {"op_moved", "op_renamed", "op_added", "op_deleted", "op_modified", "op_scanned"};
        for (const QString& op : ops) {
            auto* cb = new QCheckBox(gt(self, op, op), grupo_operacao);
            cb->setChecked(true);
            QObject::connect(cb, &QCheckBox::checkStateChanged, self,
                             [self](Qt::CheckState) { self->on_filtro_alterado(); });
            layout_operacao->addWidget(cb);
            checkboxes_operacao.insert(op, QVariant::fromValue(static_cast<QObject*>(cb)));
        }
        self->setProperty("checkboxes_operacao", checkboxes_operacao);

        auto* grupo_busca = new QGroupBox(gt(self, "search", "Busca"), self);
        grupo_busca->setObjectName(QStringLiteral("grupo_busca"));
        auto* layout_busca = new QHBoxLayout(grupo_busca);
        auto* campo_busca = new QLineEdit(grupo_busca);
        campo_busca->setObjectName(QStringLiteral("campo_busca"));
        QObject::connect(campo_busca, &QLineEdit::textChanged, self, [self]() {
            if (self->administrador_filtros_obj()) self->administrador_filtros_obj()->aplicar_filtros();
        });
        layout_busca->addWidget(campo_busca);

        auto* grupo_extensao = new QGroupBox(gt(self, "extension_filter", "Filtro de extensão"), self);
        grupo_extensao->setObjectName(QStringLiteral("grupo_extensao"));
        auto* layout_ext = new QHBoxLayout(grupo_extensao);
        auto* campo_ext = new QLineEdit(grupo_extensao);
        campo_ext->setObjectName(QStringLiteral("campo_extensao"));
        campo_ext->setPlaceholderText(QStringLiteral("pdf, txt, doc..."));
        QObject::connect(campo_ext, &QLineEdit::textChanged, self, [self]() {
            if (self->administrador_filtros_obj()) self->administrador_filtros_obj()->aplicar_filtros();
        });
        layout_ext->addWidget(campo_ext);

        auto* grupo_data = new QGroupBox(gt(self, "date_filter", "Filtro por data"), self);
        grupo_data->setObjectName(QStringLiteral("grupo_data"));
        auto* layout_data = new QFormLayout(grupo_data);
        layout_data->setHorizontalSpacing(8);
        layout_data->setVerticalSpacing(4);

        auto makeIgnore = [&](const QString& objName, const QString& keyState, const QString& labelKey) {
            auto* cb = new QCheckBox(gt(self, labelKey, labelKey), grupo_data);
            cb->setObjectName(objName);
            cb->setChecked(source::tools::AdministradorFiltros::filtros_estado.value(keyState, true));
            QObject::connect(cb, &QCheckBox::checkStateChanged, self, [self](Qt::CheckState) {
                if (self->administrador_filtros_obj()) self->administrador_filtros_obj()->aplicar_filtros();
            });
            layout_data->addRow(cb);
            return cb;
        };

        auto* ignorar_mover = makeIgnore("ignorar_mover", "ignorar_mover", "ignore_move_filter");
        auto* ignorar_renomeados = makeIgnore("ignorar_renomeados", "ignorar_renomeados", "ignore_rename_filter");
        auto* ignorar_adicionados = makeIgnore("ignorar_adicionados", "ignorar_adicionados", "ignore_add_filter");
        auto* ignorar_excluidos = makeIgnore("ignorar_excluidos", "ignorar_excluidos", "ignore_delete_filter");
        auto* ignorar_modificados = makeIgnore("ignorar_data_modificados", "ignorar_data_modificados", "ignore_modified_filter");
        auto* ignorar_escaneados = makeIgnore("ignorar_escaneados", "ignorar_escaneados", "ignore_scanned_filter");

        auto* container_data_inicial = new QHBoxLayout();
        container_data_inicial->setContentsMargins(0, 0, 0, 0);
        container_data_inicial->setSpacing(4);
        auto* data_inicial = new QDateTimeEdit(grupo_data);
        data_inicial->setObjectName(QStringLiteral("data_inicial"));
        data_inicial->setDateTime(QDateTime::currentDateTime().addDays(-30));
        QObject::connect(data_inicial, &QDateTimeEdit::dateTimeChanged, self, [self]() {
            if (self->administrador_filtros_obj()) self->administrador_filtros_obj()->aplicar_filtros();
        });
        container_data_inicial->addWidget(data_inicial);

        auto* btn_calendario_inicial = new QPushButton(grupo_data);
        btn_calendario_inicial->setIcon(QIcon(source::utils::get_icon_path(QStringLiteral("calendar.ico"))));
        btn_calendario_inicial->setFixedSize(24, 24);
        QObject::connect(btn_calendario_inicial, &QPushButton::clicked, self, [self, data_inicial]() {
            if (self->administrador_calendario_obj()) self->administrador_calendario_obj()->mostrar_calendario(data_inicial);
        });
        container_data_inicial->addWidget(btn_calendario_inicial);

        auto* container_data_final = new QHBoxLayout();
        container_data_final->setContentsMargins(0, 0, 0, 0);
        container_data_final->setSpacing(4);
        auto* data_final = new QDateTimeEdit(grupo_data);
        data_final->setObjectName(QStringLiteral("data_final"));
        data_final->setDateTime(QDateTime::currentDateTime());
        QObject::connect(data_final, &QDateTimeEdit::dateTimeChanged, self, [self]() {
            if (self->administrador_filtros_obj()) self->administrador_filtros_obj()->aplicar_filtros();
        });
        container_data_final->addWidget(data_final);

        auto* btn_calendario_final = new QPushButton(grupo_data);
        btn_calendario_final->setIcon(QIcon(source::utils::get_icon_path(QStringLiteral("calendar.ico"))));
        btn_calendario_final->setFixedSize(24, 24);
        QObject::connect(btn_calendario_final, &QPushButton::clicked, self, [self, data_final]() {
            if (self->administrador_calendario_obj()) self->administrador_calendario_obj()->mostrar_calendario(data_final);
        });
        container_data_final->addWidget(btn_calendario_final);

        auto* label_data_inicial = new QLabel(gt(self, "start_date", "Data inicial"), grupo_data);
        label_data_inicial->setObjectName(QStringLiteral("label_data_inicial"));
        auto* label_data_final = new QLabel(gt(self, "end_date", "Data final"), grupo_data);
        label_data_final->setObjectName(QStringLiteral("label_data_final"));

        QWidget* dataIniWidget = new QWidget(grupo_data);
        dataIniWidget->setLayout(container_data_inicial);
        QWidget* dataFimWidget = new QWidget(grupo_data);
        dataFimWidget->setLayout(container_data_final);

        layout_data->addRow(label_data_inicial, dataIniWidget);
        layout_data->addRow(label_data_final, dataFimWidget);

        layout->addWidget(grupo_operacao);
        layout->addWidget(grupo_busca);
        layout->addWidget(grupo_extensao);
        layout->addWidget(grupo_data);

        auto* hbox_limpar = new QHBoxLayout();
        hbox_limpar->setContentsMargins(0, 0, 0, 0);
        hbox_limpar->setSpacing(6);
        auto* label_limpar = new QLabel(gt(self, "clear_filters", "Limpar filtros"), self);
        label_limpar->setObjectName(QStringLiteral("label_limpar_filtros"));
        hbox_limpar->addWidget(label_limpar);

        auto* botao_limpar = new QPushButton(self);
        botao_limpar->setObjectName(QStringLiteral("botao_limpar"));
        botao_limpar->setText(QString());
        botao_limpar->setIcon(QIcon(source::utils::get_icon_path(QStringLiteral("clear_button3.ico"))));
        botao_limpar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        botao_limpar->setFixedSize(botao_limpar->sizeHint());
        QObject::connect(botao_limpar, &QPushButton::clicked, self, [self]() {
            if (self->administrador_filtros_obj()) self->administrador_filtros_obj()->limpar_filtros();
        });
        hbox_limpar->addWidget(botao_limpar);
        hbox_limpar->addStretch();

        layout->addLayout(hbox_limpar);
        layout->addStretch();

        self->setProperty("ignorar_mover_obj", QVariant::fromValue(static_cast<QObject*>(ignorar_mover)));
        self->setProperty("ignorar_renomeados_obj", QVariant::fromValue(static_cast<QObject*>(ignorar_renomeados)));
        self->setProperty("ignorar_adicionados_obj", QVariant::fromValue(static_cast<QObject*>(ignorar_adicionados)));
        self->setProperty("ignorar_excluidos_obj", QVariant::fromValue(static_cast<QObject*>(ignorar_excluidos)));
        self->setProperty("ignorar_data_modificados_obj", QVariant::fromValue(static_cast<QObject*>(ignorar_modificados)));
        self->setProperty("ignorar_escaneados_obj", QVariant::fromValue(static_cast<QObject*>(ignorar_escaneados)));

        sincronizar_com_menu_principal(self);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao configurar interface de filtros", true);
    }
}

} // namespace source::tools::GerenciamentoFiltros
