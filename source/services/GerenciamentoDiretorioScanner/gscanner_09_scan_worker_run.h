#pragma once

namespace source::services {
class ScanWorker;
}

namespace source::services::GerenciamentoDiretorioScanner {
void scan_worker_run(source::services::ScanWorker* self);
}
