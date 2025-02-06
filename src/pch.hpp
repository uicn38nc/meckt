#include <iostream>
#include <unordered_map>
#include <memory>
#include <string>
#include <math.h>
#include <vector>
#include <variant>
#include <deque>
#include <set>
#include <functional>
#include <random>
#include <algorithm>
#include <ranges>

#ifdef _WIN32
typedef unsigned long ulong;
typedef unsigned short ushort;
typedef unsigned int uint;
#endif

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "util/Ptr.hpp"
#include "util/Logger.hpp"
#include "util/String.hpp"
#include "util/Math.hpp"
#include "util/File.hpp"
#include "util/Color.hpp"
#include "util/Date.hpp"
#include "util/ScopedString.hpp"
#include "util/Image.hpp"
#include "app/Configuration.hpp"

#include "app/map/TitleType.hpp"
#include "app/map/MapMode.hpp"
#include "app/map/TerrainType.hpp"
#include "app/menu/selection/SelectionCallbackResult.hpp"

// Custom backward::SignalHandling to print stacktrace to file.
class SignalHandler;

class App;

namespace Parser {
    class Token;
    class Node;
    class AbstractValueHolder;
    class NodeHolder;
    class LeafHolder;
}

class Mod;
class Province;
class Title;
class HighTitle;
class BaronyTitle;
class CountyTitle;
class DuchyTitle;
class KingdomTitle;
class EmpireTitle;

class Menu;
class HomeMenu;
class EditorMenu;

class Tab;
class TitlesTab;
class PropertiesTab;

#include "app/menu/tab/Tab.hpp"