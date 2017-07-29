#ifndef GRAPHER_H
#define GRAPHER_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <vector>
#include <tuple>
#include <string>
#include "parser.h"

using Point = std::pair<double, double>;
using Line = std::vector<Point>;
using LineData = std::pair<Line, SDL_Color>;

class Grapher
{
public:
    enum class Direction {UP, DOWN, LEFT, RIGHT};
    explicit Grapher(SDL_Renderer *r, const std::string &pathToSettingsFile);
    ~Grapher();
    void loadSettings(const std::string &pathToFile);
    void loadData(const std::string &pathToFile);
    void draw_all();
    void zoomIn();
    void zoomOut();
    void move(Direction dir, double vel);
    void move(int dx, int dy);
    void setXmin(double Xmin);
    void setXmax(double Xmax);
    void setYmin(double Ymin);
    void setYmax(double Ymax);
    void setDX(double dX);
    void setDrawAxis(bool draw_axis);
    void setColorAxis(const SDL_Color &colorAxis);
    void setDrawGrid(bool draw_grid);
    void setColorGrid(const SDL_Color &colorGrid);
    double Xmin() const;
    double Xmax() const;
    double Ymin() const;
    double Ymax() const;
    double dX() const;
    bool drawAxis() const;
    SDL_Color colorAxis() const;
    bool drawGrid() const;
    SDL_Color colorGrid() const;

private:
    SDL_Renderer *m_renderer;
    std::string m_pathToSettingsFile, m_pathToEquationFile;
    double m_windowWidth, m_windowHeight;
    double m_Xmin, m_Xmax, m_Ymin, m_Ymax, m_dX;
    bool m_drawAxis;
    SDL_Color m_colorAxis;
    bool m_drawGrid;
    SDL_Color m_colorGrid;
    double m_gridStepX, m_gridStepY;
    std::string m_pathToFontFile;
    int m_fontSize;
    TTF_Font *m_font;
    SDL_Color m_colorText;
    std::vector<std::pair<std::string, SDL_Color>> m_exprList;
    std::vector<LineData> m_linesData;
    enum {TEXT, TEXT_X, TEXT_Y};
    std::vector<std::tuple<std::string,int,int>> m_labels;
    std::vector<std::pair<SDL_Texture*, SDL_Rect>> m_textData;
    void fillTextData();
    void reloadTextData();
    double map(double min_val, double max_val, double mapped_min_val, double mapped_max_val, double val);
    void calculateLinesData();
    void draw_axis();
    void draw_grid();
    void draw_graph(const Line &line, const SDL_Color &color);
    void draw_text_info();
    void draw_all_graphs();
    std::string doubleToString(double val, int prec = 6);

};

#endif // GRAPHER_H
