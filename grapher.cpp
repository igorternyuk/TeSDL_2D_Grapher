#include "grapher.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <string.h>

Grapher::Grapher(SDL_Renderer *r, const std::string &pathToSettingsFile):
    m_renderer(r), m_pathToSettingsFile(pathToSettingsFile)
{
    loadSettings(pathToSettingsFile);
    try
    {
        loadData(m_pathToEquationFile);
    }
    catch(std::exception &ex)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", ex.what(), NULL);
    }
    m_font = TTF_OpenFont(m_pathToFontFile.c_str(), m_fontSize);
    m_colorText = {0, 0, 0, 255};
    fillTextData();
    calculateLinesData();
}

Grapher::~Grapher()
{
    TTF_CloseFont(m_font);
    for(auto &d: m_textData)
        SDL_DestroyTexture(d.first);
    m_textData.clear();
}

void Grapher::loadSettings(const std::string &pathToFile)
{
    enum LoadState
    {
        EQUATION_FILE,
        WINDOW_SIZE,
        AREA_LIMITS,
        ARGUMENT_STEP,
        MAY_BE_DRAW_AXIS,
        AXIS_COLOR,
        MAY_BE_DRAW_GRID,
        GRID_COLOR,
        GRID_STEP,
        FONT_PARS,
        STOP
    };
    LoadState ls;
    std::string line, tmp;
    std::ifstream fi(pathToFile);
    std::stringstream ss;
    unsigned int red, green, blue, alpha;
    if(fi.is_open())
    {
        while(!fi.eof())
        {
            std::getline(fi, line);
            if(line == "[Equations file]")
            {
                ls = EQUATION_FILE;
            }
            else if(line == "[Window size]")
            {
                ls = WINDOW_SIZE;
            }
            else if(line == "[Graphs area limits(Xmin, Xmax, Ymin, Ymax)]")
            {
                ls = AREA_LIMITS;
            }
            else if(line == "[Argument step(dX)]")
            {
                ls = ARGUMENT_STEP;
            }
            else if(line == "[Draw axis?(Yes/No)]")
            {
                ls = MAY_BE_DRAW_AXIS;
            }
            else if(line == "[Axis color(RGBA)]")
            {
                ls = AXIS_COLOR;
            }
            else if(line == "[Draw grid?(Yes/No)]")
            {
                ls = MAY_BE_DRAW_GRID;
            }
            else if(line == "[Grid color(RGBA)]")
            {
                ls = GRID_COLOR;
            }
            else if(line == "[grid step(dx dy)]")
            {
                ls = GRID_STEP;
            }
            else if(line == "[Font(path size)]")
            {
                ls = FONT_PARS;
            }
            else
            {
                ls = STOP;
            }
            std::getline(fi, line);
            switch(ls)
            {
                case EQUATION_FILE:
                    m_pathToEquationFile = line;
                    break;
                case WINDOW_SIZE:
                    ss.clear();
                    ss << line;
                    ss >> m_windowWidth >> m_windowHeight;
                    break;
                case AREA_LIMITS:
                    ss.clear();
                    ss << line;
                    ss >> m_Xmin >> m_Xmax >> m_Ymin >> m_Ymax;
                    break;
                case ARGUMENT_STEP:
                    m_dX = std::atof(line.c_str());
                    break;
                case MAY_BE_DRAW_AXIS:
                    std::transform(line.begin(), line.end(), line.begin(), ::tolower);
                    m_drawAxis = line == "yes";
                    break;
                case AXIS_COLOR:
                    ss.clear();
                    ss << line;
                    ss >> red >> green >> blue >> alpha;
                    m_colorAxis = {red, green, blue, alpha};
                    //m_colorAxis = {0, 0, 0, 255};
                    break;
                case MAY_BE_DRAW_GRID:
                    std::transform(line.begin(), line.end(), line.begin(), ::tolower);
                    m_drawGrid = line == "yes";
                    break;
                case GRID_COLOR:
                    ss.clear();
                    ss << line;
                    ss >> red >> green >> blue >> alpha;
                    m_colorGrid = {red, green, blue, alpha};
                    //m_colorGrid = {207, 239, 127,255};
                    break;
                case GRID_STEP:
                    ss.clear();
                    ss << line;
                    ss >> m_gridStepX >> m_gridStepY;
                    break;
                case FONT_PARS:
                    ss.clear();
                    ss << line;
                    std::getline(ss, m_pathToFontFile, ' ');
                    std::getline(ss, tmp, ' ');
                    m_fontSize = std::atoi(tmp.c_str());
                    break;
                case STOP:
                    break;
                default:
                    break;
            }
            if(ls == STOP) break;
        }
        fi.close();
    }
    else
    {
        //default values
        m_pathToEquationFile = "equations.dat";
        m_windowWidth = 1200;
        m_windowHeight = 600;
        m_Xmin = -10;
        m_Xmax = 10;
        m_Ymin = -5;
        m_Ymax = 5;
        m_dX = 0.1;
        m_drawAxis = true;
        m_colorAxis = {0, 0, 0, 255};
        m_drawGrid = true;
        m_colorGrid = {207, 239, 127,255};
        m_gridStepX = 1;
        m_gridStepY = 1;
        m_pathToFontFile = "orbitron-bold.otf";
        m_fontSize = 28;
    }
}

void Grapher::loadData(const std::string &pathToFile)
{
    std::ifstream fi(pathToFile);
    if(fi.is_open())
    {
        std::string line;
        std::getline(fi, line);
        if(line == "[begin]")
        {
            m_exprList.clear();
            while(!fi.eof())
            {
                std::string equation, colorData;
                std::getline(fi, equation);
                if(equation != "[end]")
                {
                    std::getline(fi, colorData);
                    std::stringstream ss;
                    ss << colorData;
                    int red, green, blue, alpha;
                    ss >> red >> green >> blue >> alpha;
                    SDL_Color color;
                    color.r = red;
                    color.g = green;
                    color.b = blue;
                    color.a = alpha;
                    m_exprList.push_back(std::make_pair(equation, color));
                }
                else
                    break;
            }
        }
        fi.close();
    }
    else
    {
        throw std::runtime_error("Could not find file with equations");
    }
}

double Grapher::map(double min_val, double max_val, double mapped_min_val,
                    double mapped_max_val, double val)
{
    return (val - min_val) / (max_val - min_val) * (mapped_max_val - mapped_min_val) +
            mapped_min_val;
}

void Grapher::calculateLinesData()
{
    m_linesData.clear();
    static const double esp = 0.000001;
    for(const auto &expr: m_exprList)
    {
        Line line;
        for(double x = m_Xmin; x <= m_Xmax; x += m_dX)
        {
            std::vector<std::pair<char, double>> vars;
            if(fabs(x) < esp) x = 0;
            vars.emplace_back('X', x);
            iat::Parser p(expr.first, vars);
            double y = p.calculateExpression();
            double mappedX = map(m_Xmin, m_Xmax, 0, m_windowWidth, x);
            double mappedY = map(m_Ymin, m_Ymax, 0, m_windowHeight, -y);
            line.emplace_back(mappedX,mappedY);
        }
        m_linesData.emplace_back(line, expr.second);
    }
}

void Grapher::draw_all()
{
    if(m_drawGrid)
        draw_grid();
    if(m_drawAxis)
        draw_axis();
    draw_all_graphs();
    draw_text_info();
}

void Grapher::zoomIn()
{
    m_Xmin *= 0.9;
    m_Xmax *= 0.9;
    m_Ymin *= 0.9;
    m_Ymax *= 0.9;
    calculateLinesData();
    reloadTextData();
}

void Grapher::zoomOut()
{
    m_Xmin *= 1.1;
    m_Xmax *= 1.1;
    m_Ymin *= 1.1;
    m_Ymax *= 1.1;
    calculateLinesData();
    reloadTextData();
}

void Grapher::move(Direction dir, double vel)
{
    switch (dir) {
        case Direction::UP:
            m_Ymin -= vel;
            m_Ymax -= vel;
        break;
        case Direction::DOWN:
            m_Ymin += vel;
            m_Ymax += vel;
        break;
        case Direction::LEFT:
            m_Xmin += vel;
            m_Xmax += vel;
        break;
        case Direction::RIGHT:
            m_Xmin -= vel;
            m_Xmax -= vel;
        break;
    }
    calculateLinesData();
    reloadTextData();
}

void Grapher::move(int dx, int dy)
{
    double cos_ = dx / sqrt(dx * dx + dy * dy);
    double sin_ = dy / sqrt(dx * dx + dy * dy);
    m_Ymin -= 0.01 * sin_;
    m_Ymax -= 0.01 * sin_;
    m_Xmin -= 0.01 * cos_;
    m_Xmax -= 0.01 * cos_;
    calculateLinesData();
    reloadTextData();
}

void Grapher::setXmin(double Xmin)
{
    m_Xmin = Xmin;
    calculateLinesData();
    reloadTextData();
}

void Grapher::setXmax(double Xmax)
{
    m_Xmax = Xmax;
    calculateLinesData();
    reloadTextData();
}

void Grapher::setYmin(double Ymin)
{
    m_Ymin = Ymin;
    calculateLinesData();
    reloadTextData();
}

void Grapher::setYmax(double Ymax)
{
    m_Ymax = Ymax;
    calculateLinesData();
    reloadTextData();
}

void Grapher::setDX(double dX)
{
    m_dX = dX;
    calculateLinesData();
}

void Grapher::setDrawAxis(bool drawAxis)
{
    m_drawAxis = drawAxis;
    calculateLinesData();
    reloadTextData();
}

void Grapher::setDrawGrid(bool drawGrid)
{
    m_drawGrid = drawGrid;
}

void Grapher::setColorAxis(const SDL_Color &colorAxis)
{
    m_colorAxis = colorAxis;
    calculateLinesData();
}

void Grapher::setColorGrid(const SDL_Color &colorGrid)
{
    m_colorGrid = colorGrid;
    calculateLinesData();
}

double Grapher::Xmin() const
{
    return m_Xmin;
}

double Grapher::Xmax() const
{
    return m_Xmax;
}

double Grapher::Ymin() const
{
    return m_Ymin;
}

double Grapher::Ymax() const
{
    return m_Ymax;
}

double Grapher::dX() const
{
    return m_dX;
}

bool Grapher::drawAxis() const
{
    return m_drawAxis;
}

bool Grapher::drawGrid() const
{
    return m_drawGrid;
}

SDL_Color Grapher::colorAxis() const
{
    return m_colorAxis;
}

SDL_Color Grapher::colorGrid() const
{
    return m_colorGrid;
}

void Grapher::fillTextData()
{
    std::setprecision(1);
    m_labels.push_back(std::make_tuple(doubleToString(m_Xmin, 1), 20, m_windowHeight / 2 - 40));
    m_labels.push_back(std::make_tuple(doubleToString(m_Xmax, 1), m_windowWidth - 100, m_windowHeight / 2 - 40));
    m_labels.push_back(std::make_tuple(doubleToString(m_Ymin, 1), m_windowWidth / 2 + 20, m_windowHeight - 40));
    m_labels.push_back(std::make_tuple(doubleToString(m_Ymax, 1), m_windowWidth / 2 + 20, 20));
    //std::vector<std::tuple<SDL_Surface*, SDL_Texture*, SDL_Rect,
    //std::string>> m_textData;
    for(int i = 0; i < m_labels.size(); ++i)
    {
        SDL_Surface *textSurface = TTF_RenderText_Solid(m_font, std::get<TEXT>(m_labels.at(i)).c_str(), m_colorText);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
        SDL_FreeSurface(textSurface);
        SDL_Rect textRect;
        textRect.x = std::get<TEXT_X>(m_labels.at(i));
        textRect.y = std::get<TEXT_Y>(m_labels.at(i));
        SDL_QueryTexture(texture, NULL, NULL, &textRect.w, &textRect.h);
        m_textData.push_back(std::make_pair(texture, textRect));
    }

}

void Grapher::reloadTextData()
{
    for(auto &d: m_textData)
        SDL_DestroyTexture(d.first);
    m_textData.clear();
    m_labels.clear();
    fillTextData();
}

void Grapher::draw_axis()
{
    if(m_drawAxis)
    {
        SDL_SetRenderDrawColor(m_renderer,  m_colorAxis.r, m_colorAxis.g, m_colorAxis.b,
                               m_colorAxis.a);
        //Horizontal
        SDL_RenderDrawLine(m_renderer, map(m_Xmin, m_Xmax, 0, m_windowWidth, m_Xmin),
                           map(m_Ymin, m_Ymax, 0, m_windowHeight, 0),
                           map(m_Xmin, m_Xmax, 0, m_windowWidth, m_Xmax),
                           map(m_Ymin, m_Ymax, 0, m_windowHeight, 0)
                          );
         //Vertical
        SDL_RenderDrawLine(m_renderer, map(m_Xmin, m_Xmax, 0, m_windowWidth, 0),
                            map(m_Ymin, m_Ymax, 0, m_windowHeight, m_Ymin),
                            map(m_Xmin, m_Xmax, 0, m_windowWidth, 0),
                            map(m_Ymin, m_Ymax, 0, m_windowHeight, m_Ymax)
                            );
    }
}

void Grapher::draw_grid()
{
    if(m_drawGrid)
    {
        SDL_SetRenderDrawColor(m_renderer, m_colorGrid.r, m_colorGrid.g, m_colorGrid.b,
                               m_colorGrid.a);
        //Vertical lines
        for(double x = m_Xmin; x < m_Xmax; x += m_gridStepX)
        {
            if(x == 0) continue;
            SDL_RenderDrawLine(m_renderer, map(m_Xmin, m_Xmax, 0, m_windowWidth, x),
                               map(m_Ymin, m_Ymax, 0, m_windowHeight, m_Ymin),
                               map(m_Xmin, m_Xmax, 0, m_windowWidth, x),
                               map(m_Ymin, m_Ymax, 0, m_windowHeight, m_Ymax)
                              );
        }
        //Horizontal lines
        for(double y = m_Ymin; y < m_Ymax; y += m_gridStepY)
        {
            if(y == 0) continue;
            SDL_RenderDrawLine(m_renderer, map(m_Xmin, m_Xmax, 0, m_windowWidth, m_Xmin),
                               map(m_Ymin, m_Ymax, 0, m_windowHeight, y),
                               map(m_Xmin, m_Xmax, 0, m_windowWidth, m_Xmax),
                               map(m_Ymin, m_Ymax, 0, m_windowHeight, y)
                              );
        }
    }
}

void Grapher::draw_graph(const Line &line, const SDL_Color &color)
{
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    auto oldX = line.at(0).first;
    auto oldY = line.at(0).second;
    for(int i = 1; i < line.size(); ++i)
    {
        auto newX = line.at(i).first;
        auto newY = line.at(i).second;
        //lineRGBA(m_renderer, oldX, oldY, newX, newY, color.r, color.g, color.b, color.a);
        SDL_RenderDrawLine(m_renderer, oldX, oldY, newX, newY);
        oldX = newX;
        oldY = newY;
    }
}

void Grapher::draw_text_info()
{
    for(const auto &t: m_textData)
    {
        SDL_RenderCopy(m_renderer, t.first, NULL, &t.second);
    }
}

void Grapher::draw_all_graphs()
{
    for(const auto &ld: m_linesData)
    {
        draw_graph(ld.first, ld.second);
    }
}

std::string Grapher::doubleToString(double val, int prec)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(prec) << val;
    return ss.str();
}

