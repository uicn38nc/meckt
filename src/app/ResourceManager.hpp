#pragma once

template <typename R, typename I = int>
class ResourceManager {
public:
    ResourceManager(const std::string& name) : m_Name(name) {}
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    void Load(const I& id, const std::string& path) {
        std::unique_ptr<R> ptr(new R());
        if(!ptr->loadFromFile(path)) {
            LOG_ERROR("Failed to load resource {} {} from file {}", m_Name, (int) id, path);
            throw std::runtime_error("failed to load resource file.");
        }
        m_Resources.emplace(id, std::move(ptr));
        // LOG_INFO("loaded {} {} from {}", m_Name, (int) id, path);
    }
    
    template <typename ...Args>
    void Load(const I& id, const std::string& path, Args&& ...args) {
        std::unique_ptr<R> ptr(new R());
        if(!ptr->loadFromFile(path, std::forward<Args>(args)...)) {
            LOG_ERROR("Failed to load resource {} {} from file {}", m_Name, (int) id, path);
            throw std::runtime_error("failed to load resource file.");
        }
        m_Resources.emplace(id, std::move(ptr));
        // LOG_INFO("loaded {} {} from {}", m_Name, (int) id, path);
    }

    R& Get(const I& id) const {
        return *m_Resources.at(id);
    }

    // R& Get(const I& id) {
    //     return *m_Resources.at(id);
    // }

private:
    std::string m_Name;
    std::unordered_map<I, std::unique_ptr<R>> m_Resources;
};

template<typename I>
class ResourceManager<sf::Music, I>
{
public:
    ResourceManager() : m_Name("music") {}
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    void Load(const I& id, const std::string& path) {
        std::unique_ptr<sf::Music> ptr(new sf::Music());
        if(!ptr->openFromFile(path)) {
            LOG_ERROR("Failed to load music {} from file {}", (int) id, path);
            throw std::runtime_error("failed to load music file.");
        }
        m_Resources.emplace(id, std::move(ptr));
        // LOG_INFO("loaded {} {} from {}", m_Name, (int) id, path);
    }
    
    template<typename ...Args>
    void Load(const I& id, const std::string& path, Args&& ...args) {
        std::unique_ptr<sf::Music> ptr(new sf::Music());
        if(!ptr->openFromFile(path, std::forward<Args>(args)...)) {
            LOG_ERROR("Failed to load music {} from file {}", (int) id, path);
            throw std::runtime_error("failed to load music file.");
        }
        m_Resources.emplace(id, std::move(ptr));
        // LOG_INFO("loaded {} {} from {}", m_Name, (int) id, path);
    }

    sf::Music& Get(const I& id) const {
        return *m_Resources.at(id);
    }

private:
    std::string m_Name;
    std::unordered_map<I, std::unique_ptr<sf::Music>> m_Resources;
};

template<typename I>
class ResourceManager<sf::Shader, I>
{
public:
    ResourceManager() : m_Name("shader") {}
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    
    void Load(const I& id, const std::string& path) = delete;
    
    template<typename ...Args>
    void Load(const I& id, const std::string& path, Args&& ...args) {
        std::unique_ptr<sf::Shader> ptr(new sf::Shader());
        if(!ptr->loadFromFile(path, std::forward<Args>(args)...)) {
            LOG_ERROR("Failed to load shader {} from file {}", (int) id, path);
            throw std::runtime_error("failed to load shader file.");
        }
        m_Resources.emplace(id, std::move(ptr));
        // LOG_INFO("loaded {} {} from {}", m_Name, (int) id, path);
    }

    sf::Shader& Get(const I& id) const {
        return *m_Resources.at(id);
    }

private:
    std::string m_Name;
    std::unordered_map<I, std::unique_ptr<sf::Shader>> m_Resources;
};