#include "ob_06_EventoModificado.h"

#include "ob_01_Observador.h"
#include "GerenciamentoEventoModificado/gevmod_01_calcular_intervalo.h"
#include "GerenciamentoEventoModificado/gevmod_02_calcular_intervalo_original.h"
#include "GerenciamentoEventoModificado/gevmod_03_is_arquivo_codigo_grande.h"
#include "GerenciamentoEventoModificado/gevmod_04_limpar_cache_metadados.h"
#include "GerenciamentoEventoModificado/gevmod_05_processar.h"
#include "GerenciamentoEventoModificado/gevmod_06_processar_massivo.h"
#include "GerenciamentoEventoModificado/gevmod_07_processar_normal.h"
#include "source/utils/LogManager.h"

namespace source::services {

EventoModificado::EventoModificado(source::services::Observador* observador)
    : BaseEvento(observador),
      intervalo_base(0.1),
      tamanho_threshold(100ll * 1024ll * 1024ll),
      max_intervalo(5.0),
      exclusao_threshold(0.1),
      min_intervalo(0.01),
      grande_arquivo_threshold(50ll * 1024ll * 1024ll),
      tempo_espera_grande_arquivo(3.0),
      CODIGO_LINHA_THRESHOLD(1000) {
    try {
        EXTENSOES_VIDEO = {".mp4", ".mov", ".avi", ".mkv", ".wmv", ".flv", ".webm", ".mts", ".m2ts", ".mpeg", ".m4v"};
        EXTENSOES_RAPIDAS = {".txt", ".log", ".json", ".xml", ".csv", ".ini", ".cfg", ".conf", ".dat"};
        EXTENSOES_CODIGO = {
            ".tmp", ".temp", ".~", ".bak", ".swp", ".swo", ".$$", ".old", ".part", ".cache",
            ".crdownload", ".download", ".partial", ".jpg", ".jpeg", ".png", ".gif", ".bmp",
            ".tiff", ".tif", ".psd", ".svg", ".webp", ".raw", ".heic", ".heif", ".cr2", ".nef",
            ".arw", ".wav", ".mp3", ".aac", ".flac", ".ogg", ".aiff", ".wma", ".m4a", ".aif",
            ".py", ".java", ".cpp", ".c", ".h", ".hpp", ".cs", ".js", ".html", ".htm",
            ".mht", ".xhtml", ".mhml", ".css", ".php", ".sql", ".json", ".xml", ".yaml", ".yml",
            ".ini", ".cfg", ".conf", ".log", ".md", ".rst", ".bat", ".sh", ".ps1", ".psm1",
            ".psd1", ".ps1xml", ".pssc", ".psc1", ".doc", ".docx", ".pdf", ".rtf", ".txt",
            ".odt", ".wpd", ".pages", ".xls", ".xlsx", ".ods", ".csv", ".tsv", ".numbers",
            ".ppt", ".pptx", ".odp", ".key", ".db", ".sqlite", ".mdb", ".accdb", ".sav",
            ".spss", ".exe", ".dll", ".bin", ".app", ".apk", ".msi", ".run", ".cmd",
            ".zip", ".rar", ".7z", ".tar", ".gz", ".bz2", ".xz", ".cab", ".dat"};
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar EventoModificado", true);
    }
}

double EventoModificado::calcular_intervalo(const QString& caminho) { return GerenciamentoEventoModificado::calcular_intervalo(this, caminho); }
double EventoModificado::_calcular_intervalo_original(const QString& caminho) { return GerenciamentoEventoModificado::calcular_intervalo_original(this, caminho); }
bool EventoModificado::is_arquivo_codigo_grande(const QString& caminho) { return GerenciamentoEventoModificado::is_arquivo_codigo_grande(this, caminho); }
void EventoModificado::_limpar_cache_metadados(const QString& caminho_completo, const QString& ext) { GerenciamentoEventoModificado::limpar_cache_metadados(this, caminho_completo, ext); }
bool EventoModificado::processar(const QString& nome_arquivo, const QString& caminho_completo, double tempo_atual) { return GerenciamentoEventoModificado::processar(this, nome_arquivo, caminho_completo, tempo_atual); }
bool EventoModificado::_processar_massivo(const QString& nome_arquivo, const QString& caminho_completo, double tempo_atual) { return GerenciamentoEventoModificado::processar_massivo(this, nome_arquivo, caminho_completo, tempo_atual); }
bool EventoModificado::_processar_normal(const QString& nome_arquivo, const QString& caminho_completo, double tempo_atual) { return GerenciamentoEventoModificado::processar_normal(this, nome_arquivo, caminho_completo, tempo_atual); }

}  // namespace source::services
