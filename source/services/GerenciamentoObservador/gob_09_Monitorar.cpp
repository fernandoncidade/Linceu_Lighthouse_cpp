#include "gob_09_Monitorar.h"

#include <chrono>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#ifdef interface
#undef interface
#endif
#endif

#include "gob_01_DetectarOperacaoMassiva.h"
#include "source/services/ob_01_Observador.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoObservador {
namespace {

void enfileirar_eventos_lidos(source::services::Observador* self, std::vector<QVariantMap>& eventos) {
    if (self == nullptr || eventos.empty()) {
        return;
    }

    std::lock_guard<std::mutex> lock(self->evento_lock);
    for (QVariantMap& evento : eventos) {
        self->buffer_eventos.push_back(std::move(evento));
    }
}

}  // namespace

void monitorar(source::services::Observador* self) {
    try {
        if (!self) return;

#ifdef _WIN32
        constexpr DWORD BUFFER_SIZE = 16 * 1024 * 1024;
    constexpr DWORD FILE_LIST_DIRECTORY_ACCESS = 0x0001;

        const std::wstring dir_w = self->diretorio.toStdWString();
    HANDLE handle = CreateFileW(dir_w.c_str(), FILE_LIST_DIRECTORY_ACCESS,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING,
                                    FILE_FLAG_BACKUP_SEMANTICS, nullptr);
        if (handle == INVALID_HANDLE_VALUE) {
            source::utils::LogManager::get_logger().error("Erro ao criar handle de diretório", true);
            self->handle_dir = 0;
            return;
        }

        self->handle_dir = reinterpret_cast<quintptr>(handle);
        std::vector<BYTE> buffer(BUFFER_SIZE);

        while (self->ativo) {
            if (!self->pause_event_set) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            DWORD bytes_retornados = 0;
            const BOOL ok = ReadDirectoryChangesW(
                handle, buffer.data(), static_cast<DWORD>(buffer.size()), TRUE,
                FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES |
                    FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SECURITY |
                    FILE_NOTIFY_CHANGE_CREATION,
                &bytes_retornados, nullptr, nullptr);

            if (!ok) {
                if (self->desligando) break;
                source::utils::LogManager::get_logger().error("Erro no monitoramento de diretório", true);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }

            const double tempo_batch =
                std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();

            int eventos_lidos = 0;
            std::vector<QVariantMap> eventos_do_batch;
            BYTE* ptr = buffer.data();
            while (ptr) {
                const auto* info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(ptr);
                const QString nome =
                    QString::fromWCharArray(info->FileName, static_cast<int>(info->FileNameLength / sizeof(WCHAR)));

                QVariantMap evento;
                evento.insert("acao", static_cast<int>(info->Action));
                evento.insert("nome_arquivo", nome);
                evento.insert("tempo_evento", tempo_batch);

                eventos_do_batch.push_back(std::move(evento));
                ++eventos_lidos;

                if (info->NextEntryOffset == 0) break;
                ptr += info->NextEntryOffset;
            }

            enfileirar_eventos_lidos(self, eventos_do_batch);
            self->total_eventos_recebidos += eventos_lidos;
            detectar_operacao_massiva(self);
        }

#else
        while (self->ativo) {
            if (!self->pause_event_set) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            detectar_operacao_massiva(self);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
#endif

        close_handle_dir_safely(self);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro desconhecido no monitoramento", true);
    }
}

void close_handle_dir_safely(source::services::Observador* self) {
    if (!self) return;
#ifdef _WIN32
    HANDLE handle = reinterpret_cast<HANDLE>(self->handle_dir);
    if (handle != nullptr && handle != INVALID_HANDLE_VALUE) {
        CloseHandle(handle);
    }
#endif
    self->handle_dir = 0;
}

}  // namespace source::services::GerenciamentoObservador
