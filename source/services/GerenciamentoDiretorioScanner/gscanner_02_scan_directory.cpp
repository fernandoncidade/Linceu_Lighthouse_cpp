#include "gscanner_02_scan_directory.h"

#include <atomic>
#include <functional>

#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QMutex>
#include <QMutexLocker>
#include <QSemaphore>
#include <QSet>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QThread>
#include <QThreadPool>
#include <QVector>

#include "source/services/ob_01_Observador.h"
#include "source/services/ob_02_BaseEvento.h"
#include "source/services/ob_03_DiretorioScanner.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace {
QString conn_name(const QString& db) { return QStringLiteral("scanner_snapshot_%1").arg(reinterpret_cast<quintptr>(db.constData())); }

bool should_abort(source::services::DiretorioScanner* self) {
    return !self || QThread::currentThread()->isInterruptionRequested() ||
           (self->observador && self->observador->desligando);
}

bool wait_if_paused(source::services::DiretorioScanner* self) {
    while (self && !self->pause_event_set.load()) {
        if (should_abort(self)) return false;
        QThread::msleep(100);
    }
    return !should_abort(self);
}

bool should_skip_descend(const QFileInfo& info) {
    if (!info.exists() || !info.isDir()) return true;
    if (info.isSymLink()) return true;
#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
    if (info.isJunction()) return true;
#endif
    return false;
}

QString normalized_dir_id(const QString& path) {
    const QFileInfo info(path);
    QString id = info.canonicalFilePath();
    if (id.isEmpty()) {
        id = QDir(path).absolutePath();
    }
#ifdef Q_OS_WIN
    id = id.toLower();
#endif
    return QDir::cleanPath(id);
}

int count_entries_recursive(const QString& directory) {
    if (!QDir(directory).exists()) return 0;

    int total = 0;
    QSet<QString> visited;
    QVector<QString> pending{QDir(directory).absolutePath()};

    while (!pending.isEmpty()) {
        const QString root = pending.takeLast();
        const QString rootId = normalized_dir_id(root);
        if (rootId.isEmpty() || visited.contains(rootId)) {
            continue;
        }
        visited.insert(rootId);

        QDir dir(root);
        if (!dir.exists()) {
            continue;
        }

        const QFileInfoList dirs = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs, QDir::Name);
        const QFileInfoList files = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files, QDir::Name);
        total += dirs.size() + files.size();

        for (const QFileInfo& d : dirs) {
            if (should_skip_descend(d)) continue;
            pending.push_back(d.absoluteFilePath());
        }
    }

    return total;
}

int scan_registration_parallelism() {
    bool ok = false;
    const int configured = qEnvironmentVariableIntValue("LINCEU_SCAN_SECURITY_THREADS", &ok);
    if (ok) {
        return qBound(1, configured, 16);
    }

    const int ideal = QThread::idealThreadCount();
    if (ideal <= 2) {
        return 2;
    }
    return qBound(2, ideal - 1, 8);
}

QVariantMap build_evento_scan(source::services::DiretorioScanner* self, const QString& nome, const QString& caminho,
                              const QString& tipo, const QVariantMap& m) {
    return {
        {"tipo_operacao", QStringLiteral("op_scanned")},
        {"nome", nome},
        {"dir_anterior", ""},
        {"dir_atual", caminho},
        {"data_criacao", m.value("data_criacao", "")},
        {"data_modificacao", m.value("data_modificacao", "")},
        {"data_acesso", m.value("data_acesso", "")},
        {"tipo", tipo},
        {"mime", m.value("mime", "")},
        {"size_b", m.value("size_b", "")},
        {"size_kb", m.value("size_kb", "")},
        {"size_mb", m.value("size_mb", "")},
        {"size_gb", m.value("size_gb", "")},
        {"size_tb", m.value("size_tb", "")},
        {"atributos", m.value("atributos", "")},
        {"autor", m.value("autor", "")},
        {"dimensoes", m.value("dimensoes", "")},
        {"duracao", m.value("duracao", "")},
        {"taxa_bits", m.value("taxa_bits", "")},
        {"protegido", m.value("protegido", "")},
        {"paginas", m.value("paginas", "")},
        {"linhas", m.value("linhas", "")},
        {"palavras", m.value("palavras", "")},
        {"paginas_estimadas", m.value("paginas_estimadas", "")},
        {"linhas_codigo", m.value("linhas_codigo", "")},
        {"total_linhas", m.value("total_linhas", "")},
        {"slides_estimados", m.value("slides_estimados", "")},
        {"arquivos", m.value("arquivos", "")},
        {"unzipped_b", m.value("unzipped_b", "")},
        {"unzipped_kb", m.value("unzipped_kb", "")},
        {"unzipped_mb", m.value("unzipped_mb", "")},
        {"unzipped_gb", m.value("unzipped_gb", "")},
        {"unzipped_tb", m.value("unzipped_tb", "")},
        {"slides", m.value("slides", "")},
        {"binary_file_b", m.value("binary_file_b", "")},
        {"binary_file_kb", m.value("binary_file_kb", "")},
        {"binary_file_mb", m.value("binary_file_mb", "")},
        {"binary_file_gb", m.value("binary_file_gb", "")},
        {"binary_file_tb", m.value("binary_file_tb", "")},
        {"planilhas", m.value("planilhas", "")},
        {"colunas", m.value("colunas", "")},
        {"registros", m.value("registros", "")},
        {"tabelas", m.value("tabelas", "")},
    };
}

QVariantMap prepare_scan_event(source::services::DiretorioScanner* self,
                               const QString& nome,
                               const QString& caminho) {
    const QString tipo = self->get_file_type(caminho);
    QVariantMap item{{"nome", nome}, {"dir_atual", caminho}};
    self->current_item = item;

    QVariantMap metadados;
    if (!caminho.isEmpty() && QFileInfo::exists(caminho) && self->gerenciador_colunas) {
        if (auto* gc = qobject_cast<source::services::GerenciadorColunas*>(self->gerenciador_colunas)) {
            metadados = gc->get_metadados(item);
        }
    }

    return build_evento_scan(self, nome, caminho, tipo, metadados);
}

void notify_scan_item_completed(source::services::DiretorioScanner* self,
                                std::atomic<int>& contador_concluido,
                                QMutex& progress_lock) {
    if (self == nullptr) {
        return;
    }

    const int contador = contador_concluido.fetch_add(1) + 1;
    const int progresso =
        self->total_arquivos > 0 ? static_cast<int>((contador * 100.0) / self->total_arquivos) : 0;

    bool publish_progress = false;
    {
        QMutexLocker locker(&progress_lock);
        self->contador_processados = contador;

        const qint64 now_ms = QDateTime::currentMSecsSinceEpoch();
        const bool first_item = (contador <= 1);
        const bool last_item = (self->total_arquivos > 0 && contador >= self->total_arquivos);
        const bool periodic = (now_ms - self->ultimo_update_ui_ms) >= 40;
        if (first_item || last_item || periodic) {
            self->ultimo_update_ui_ms = now_ms;
            self->ultimo_progresso = progresso;
            publish_progress = true;
        }
    }

    if (publish_progress) {
        emit self->progresso_atualizado(progresso, contador, self->total_arquivos);
    }
}
}

namespace source::services::GerenciamentoDiretorioScanner {

void scan_directory(source::services::DiretorioScanner* self, const QString& directory) {
    try {
        if (!self) return;

        self->total_arquivos = count_entries_recursive(directory);
        self->contador_processados = 0;
        self->ultimo_progresso = 0.0;
        self->ultimo_update_ui_ms = 0;
        emit self->progresso_atualizado(0, 0, self->total_arquivos);

        const QString connName = conn_name(self->db_path);
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
            db.setDatabaseName(self->db_path);
            if (db.open()) {
                QSqlQuery q(db);
                q.exec("DELETE FROM snapshot");
                db.close();
            }
        }
        QSqlDatabase::removeDatabase(connName);

        const int registration_workers = scan_registration_parallelism();
        QThreadPool registration_pool;
        registration_pool.setMaxThreadCount(registration_workers);
        registration_pool.setExpiryTimeout(-1);

        QSemaphore inflight_slots(registration_workers * 2);
        std::atomic<int> contador_concluido{0};
        QMutex progress_lock;

        auto schedule_event = [&](const QVariantMap& evento) {
            inflight_slots.acquire();
            registration_pool.start([self, evento, &contador_concluido, &progress_lock, &inflight_slots]() {
                try {
                    if (!should_abort(self) && self != nullptr && self->observador != nullptr &&
                        self->observador->evento_base != nullptr) {
                        self->observador->evento_base->registrar_evento_no_banco(evento);
                    }
                } catch (...) {
                    source::utils::LogManager::get_logger().error(
                        "Erro ao registrar evento de scan em paralelo", true);
                }

                if (!should_abort(self)) {
                    notify_scan_item_completed(self, contador_concluido, progress_lock);
                }
                inflight_slots.release();
            });
        };

        QSet<QString> visited;
        QVector<QString> pending{QDir(directory).absolutePath()};
        bool interrupted = false;

        while (!pending.isEmpty() && !interrupted) {
            if (!wait_if_paused(self)) {
                interrupted = true;
                break;
            }

            const QString root = pending.takeLast();
            const QString rootId = normalized_dir_id(root);
            if (rootId.isEmpty() || visited.contains(rootId)) {
                continue;
            }
            visited.insert(rootId);

            QDir dir(root);
            if (!dir.exists()) {
                continue;
            }

            const QFileInfoList dirs = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs, QDir::Name);
            const QFileInfoList files = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files, QDir::Name);

            for (const QFileInfo& d : dirs) {
                if (!wait_if_paused(self)) {
                    interrupted = true;
                    break;
                }
                schedule_event(prepare_scan_event(self, d.fileName(), d.absoluteFilePath()));
            }
            if (interrupted) break;

            for (const QFileInfo& f : files) {
                if (!wait_if_paused(self)) {
                    interrupted = true;
                    break;
                }
                schedule_event(prepare_scan_event(self, f.fileName(), f.absoluteFilePath()));
            }
            if (interrupted) break;

            for (const QFileInfo& d : dirs) {
                if (should_skip_descend(d)) continue;
                pending.push_back(d.absoluteFilePath());
            }
        }

        registration_pool.waitForDone();

        if (interrupted) {
            return;
        }

        self->contador_processados = self->total_arquivos;
        self->ultimo_progresso = 100.0;
        emit self->progresso_atualizado(100, self->contador_processados, self->total_arquivos);
        emit self->scan_finalizado();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro no scan_directory", true);
        throw;
    }
}

}  // namespace source::services::GerenciamentoDiretorioScanner
