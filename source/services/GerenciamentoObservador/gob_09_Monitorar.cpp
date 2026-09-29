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

    {
        std::lock_guard<std::mutex> lock(self->evento_lock);
        for (QVariantMap& evento : eventos) {
            self->buffer_eventos.push_back(std::move(evento));
        }
    }
    self->cv_buffer.notify_one();
}

#ifdef _WIN32
void parsear_buffer_seguro(const BYTE* buffer_data, DWORD bytes_retornados, double tempo_batch,
                           std::vector<QVariantMap>& eventos_do_batch, int& eventos_lidos) {
    if (!buffer_data || bytes_retornados < sizeof(FILE_NOTIFY_INFORMATION)) {
        return;
    }

    DWORD offset = 0;
    while (offset < bytes_retornados) {
        if (offset + sizeof(FILE_NOTIFY_INFORMATION) > bytes_retornados) {
            break;
        }

        const auto* info = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(buffer_data + offset);
        const DWORD max_name_bytes = bytes_retornados - (offset + offsetof(FILE_NOTIFY_INFORMATION, FileName));
        if (info->FileNameLength > max_name_bytes) {
            break;
        }

        const int chars_count = static_cast<int>(info->FileNameLength / sizeof(WCHAR));
        const QString nome = QString::fromWCharArray(info->FileName, chars_count);

        QVariantMap evento;
        evento.insert(QStringLiteral("acao"), static_cast<int>(info->Action));
        evento.insert(QStringLiteral("nome_arquivo"), nome);
        evento.insert(QStringLiteral("tempo_evento"), tempo_batch);

        eventos_do_batch.push_back(std::move(evento));
        ++eventos_lidos;

        if (info->NextEntryOffset == 0) {
            break;
        }
        if (offset + info->NextEntryOffset > bytes_retornados || info->NextEntryOffset < sizeof(FILE_NOTIFY_INFORMATION)) {
            break;
        }
        offset += info->NextEntryOffset;
    }
}
#endif

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
                                    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr);
        if (handle == INVALID_HANDLE_VALUE) {
            source::utils::LogManager::get_logger().error("Erro ao criar handle de diretório", true);
            self->handle_dir = 0;
            return;
        }

        HANDLE hStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
        self->handle_stop_event = reinterpret_cast<quintptr>(hStopEvent);
        self->handle_dir = reinterpret_cast<quintptr>(handle);

        OVERLAPPED ov = {};
        ov.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

        HANDLE waitHandles[2] = { hStopEvent, ov.hEvent };
        std::vector<BYTE> buffer(BUFFER_SIZE);

        while (self->ativo) {
            if (!self->pause_event_set) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            ResetEvent(ov.hEvent);
            DWORD bytes_retornados = 0;
            const BOOL ok = ReadDirectoryChangesW(
                handle, buffer.data(), static_cast<DWORD>(buffer.size()), TRUE,
                FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES |
                    FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SECURITY |
                    FILE_NOTIFY_CHANGE_CREATION,
                &bytes_retornados, &ov, nullptr);

            if (!ok && GetLastError() != ERROR_IO_PENDING) {
                if (self->desligando) break;
                source::utils::LogManager::get_logger().error("Erro no monitoramento de diretório", true);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }

            const DWORD waitResult = WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);
            if (waitResult == WAIT_OBJECT_0) {
                CancelIo(handle);
                break;
            } else if (waitResult == WAIT_OBJECT_0 + 1) {
                if (GetOverlappedResult(handle, &ov, &bytes_retornados, FALSE) && bytes_retornados > 0) {
                    const double tempo_batch =
                        std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();

                    int eventos_lidos = 0;
                    std::vector<QVariantMap> eventos_do_batch;
                    parsear_buffer_seguro(buffer.data(), bytes_retornados, tempo_batch, eventos_do_batch, eventos_lidos);

                    if (!eventos_do_batch.empty()) {
                        enfileirar_eventos_lidos(self, eventos_do_batch);
                        self->total_eventos_recebidos += eventos_lidos;
                        detectar_operacao_massiva(self);
                    }
                }
            } else {
                if (self->desligando) break;
            }
        }

        if (ov.hEvent != nullptr) {
            CloseHandle(ov.hEvent);
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
    HANDLE hStop = reinterpret_cast<HANDLE>(self->handle_stop_event);
    if (hStop != nullptr && hStop != INVALID_HANDLE_VALUE) {
        CloseHandle(hStop);
    }
    self->handle_stop_event = 0;

    HANDLE handle = reinterpret_cast<HANDLE>(self->handle_dir);
    if (handle != nullptr && handle != INVALID_HANDLE_VALUE) {
        CloseHandle(handle);
    }
    self->handle_dir = 0;
#endif
}

}  // namespace source::services::GerenciamentoObservador
