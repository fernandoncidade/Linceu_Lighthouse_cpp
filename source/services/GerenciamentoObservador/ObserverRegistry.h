#pragma once

#include <memory>
#include <mutex>
#include <unordered_map>
#include <QObject>

namespace source::services {
class Observador;
}

namespace source::services::GerenciamentoObservador {

class ObserverRegistry {
public:
    static ObserverRegistry& instance() {
        static ObserverRegistry s_inst;
        return s_inst;
    }

    source::services::Observador* obter(QObject* interface_obj) {
        std::lock_guard<std::mutex> lock(mtx_);
        auto it = map_.find(interface_obj);
        if (it != map_.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    void registrar(QObject* interface_obj, std::unique_ptr<source::services::Observador> obs) {
        std::lock_guard<std::mutex> lock(mtx_);
        map_[interface_obj] = std::move(obs);
    }

    std::unique_ptr<source::services::Observador> remover(QObject* interface_obj) {
        std::lock_guard<std::mutex> lock(mtx_);
        auto it = map_.find(interface_obj);
        if (it != map_.end()) {
            auto obs = std::move(it->second);
            map_.erase(it);
            return obs;
        }
        return nullptr;
    }

    void limpar() {
        std::lock_guard<std::mutex> lock(mtx_);
        map_.clear();
    }

private:
    ObserverRegistry() = default;
    ~ObserverRegistry() = default;
    ObserverRegistry(const ObserverRegistry&) = delete;
    ObserverRegistry& operator=(const ObserverRegistry&) = delete;

    std::mutex mtx_;
    std::unordered_map<QObject*, std::unique_ptr<source::services::Observador>> map_;
};

}  // namespace source::services::GerenciamentoObservador
