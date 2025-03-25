#pragma once

class HoldingType {
public:
    HoldingType();
    HoldingType(const std::string& name);

    std::string GetName() const;
    void SetName(const std::string& name);

private:
    std::string m_Name;
};

class TerrainType {
public:
    TerrainType();
    TerrainType(const std::string& name, const sf::Color& color);

    std::string GetName() const;
    void SetName(const std::string& name);
    
    sf::Color GetColor() const;
    void SetColor(const sf::Color& color);

private:
    std::string m_Name;
    sf::Color m_Color;
};

enum class ProvinceFlags {
    NONE       = 0,
    COASTAL    = 1 << 0,
    LAKE       = 1 << 1,
    ISLAND     = 1 << 2,
    LAND       = 1 << 3,
    SEA        = 1 << 4,
    RIVER      = 1 << 5,
    IMPASSABLE = 1 << 6,
};

ProvinceFlags operator|(ProvinceFlags a, ProvinceFlags b);
ProvinceFlags operator&(ProvinceFlags a, ProvinceFlags b);
ProvinceFlags operator~(ProvinceFlags a);
ProvinceFlags& operator|=(ProvinceFlags& a, ProvinceFlags b);
ProvinceFlags& operator&=(ProvinceFlags& a, ProvinceFlags b);

class Province {
friend PropertiesTab;
public:
    Province(int id, sf::Color color, std::string name);

    int GetId() const;
    sf::Color GetColor() const;
    uint32_t GetColorId() const;
    std::string GetName() const;
    ProvinceFlags GetFlags() const;
    bool HasFlag(ProvinceFlags flag) const;
    std::string GetHolding() const;
    std::string GetTerrain() const;
    std::string GetCulture() const;
    std::string GetReligion() const;

    void SetName(std::string name);
    void SetColor(sf::Color color);
    void SetFlags(ProvinceFlags flags);
    void SetFlag(ProvinceFlags flag, bool enabled);
    void SetHolding(std::string holding);
    void SetTerrain(std::string terrain);
    void SetCulture(std::string culture);
    void SetReligion(std::string religion);
    
    std::string GetOriginalFilePath() const;
    SharedPtr<Parser::Object> GetOriginalData() const;
    void SetOriginalFilePath(const std::string& filePath);
    void SetOriginalData(SharedPtr<Parser::Object> data);
    
    sf::Vector2i GetImagePosition() const;
    uint GetImagePixelsCount() const;
    void SetImagePosition(sf::Vector2i pos);
    void SetImagePixelsCount(uint count);
    void IncrementImagePixelsCount();

private:
    int m_Id;
    std::string m_Name;
    sf::Color m_Color;
    ProvinceFlags m_Flags;

    std::string m_Holding;
    std::string m_Terrain;
    
    std::string m_Culture;
    std::string m_Religion;

    std::string m_OriginalFilePath;
    SharedPtr<Parser::Object> m_OriginalData;

    sf::Vector2i m_ImagePosition;
    uint m_ImagePixelsCount;

    // Sea-zone for port
    // Terrain
    // History (modifiers with date, buildings, owners...)
};