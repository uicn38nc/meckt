#pragma once

class Mod {
public:
    Mod(const std::string& dir);

    std::string GetDir() const;
    sf::Image& GetHeightmapImage();
    sf::Image& GetProvinceImage();
    sf::Image& GetRiversImage();
    sf::Image GetCultureImage();
    sf::Image GetReligionImage();
    sf::Image GetTitleImage(TitleType type);
    bool HasMap() const;

    std::map<uint32_t, SharedPtr<Province>>& GetProvinces();
    std::map<int, SharedPtr<Province>>& GetProvincesByIds();
    SharedPtr<Title> GetProvinceLiegeTitle(const SharedPtr<Province>& province, TitleType type);
    SharedPtr<Title> GetProvinceFocusedTitle(const SharedPtr<Province>& province, TitleType type);
    int GetMaxProvinceId() const;

    std::map<std::string, SharedPtr<Title>>& GetTitles();
    std::map<TitleType, std::vector<SharedPtr<Title>>>& GetTitlesByType();
    std::map<int, SharedPtr<BaronyTitle>>& GetBaroniesByProvinceIds();

    void HarmonizeTitlesColors(const std::vector<SharedPtr<Title>>& titles, sf::Color color, float hue, float saturation);
    void GenerateMissingProvinces();

    void Load();
    void LoadProvinceImage();
    void LoadDefaultMapFile();
    void LoadProvincesDefinition();
    void LoadProvincesTerrain();
    void LoadProvincesHistory();
    void LoadTitles();
    void LoadTitlesHistory();
    void LoadCultures();
    void LoadReligions();

    std::vector<SharedPtr<Title>> ParseTitles(const std::string& filePath, SharedPtr<Parser::Object> data);

    void Export();
    void ExportDefaultMapFile();
    void ExportProvincesDefinition();
    void ExportProvincesTerrain();
    void ExportProvincesHistory();
    void ExportTitles();
    void ExportTitlesHistory();

    void ExportTitle(const SharedPtr<Title>& title, std::ofstream& file, int depth);

private:
    std::string m_Dir;
    sf::Image m_HeightmapImage;
    sf::Image m_ProvinceImage;
    sf::Image m_RiversImage;

    std::map<uint32_t, SharedPtr<Province>> m_Provinces;
    std::map<int, SharedPtr<Province>> m_ProvincesByIds;
    
    std::map<std::string, SharedPtr<Title>> m_Titles;
    std::map<TitleType, std::vector<SharedPtr<Title>>> m_TitlesByType;
    std::map<int, SharedPtr<BaronyTitle>> m_BaroniesByProvinceIds;

    std::map<std::string, SharedPtr<Culture>> m_Cultures;
    std::map<std::string, SharedPtr<Religion>> m_Religions;

    TerrainType m_DefaultLandTerrain;
    TerrainType m_DefaultSeaTerrain;
    TerrainType m_DefaultCoastalSeaTerrain;
};