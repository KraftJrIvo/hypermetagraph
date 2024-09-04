#include "hypergraph.h"
#include "raylib.h"
#include <string>

namespace mhg {

    Texture2D Edge::getArrowHead() {
        static Texture2D tex;
        static bool loaded = false;
        if (loaded)
            return tex;
        tex = LoadTexture("res/arrowhead.png");
        loaded = true;
        return tex;
    }

    Vector2 Edge::getPoint(float t) {
        float x = pow(1 - t, 2) * _pts[0].x + 2 * t * (1 - t) * _pts[1].x + t * t * _pts[2].x;
        float y = pow(1 - t, 2) * _pts[0].y + 2 * t * (1 - t) * _pts[1].y + t * t * _pts[2].y;
        return Vector2{ x, y};
    }

    void Edge::reposition() {
        via->pos = 0.5f * (from->pos + to->pos);
    }

    bool Edge::draw(Vector2 origin, Vector2 offset, float scale, const Font& font, bool physics) {
        float ls = via->hg->scale() * scale;
        float minLvlNodeScale = scale * ((from->hg->scale() > to->hg->scale()) ? from->hg->scale() : to->hg->scale());
        float maxLvlNodeScale = scale * ((from->hg->scale() < to->hg->scale()) ? from->hg->scale() : to->hg->scale());
        bool fromSameHG = (from->hg == via->hg);
        bool toSameHG = (to->hg == via->hg);
        bool notSame = !fromSameHG || !toSameHG;

        _pts[0] = fromSameHG ? (origin + ls * from->pos + offset) : from->_posCache;
        _pts[2] = toSameHG ? (origin + ls * to->pos + offset) : to->_posCache;
        _pts[1] = (notSame || !physics) ? (0.5f * (_pts[0] + _pts[2])) : (origin + ls * via->pos + offset);

        Vector2 apos; float angle; float t1, t2; 
        findArrowPositionBezier(false, minLvlNodeScale, apos, angle, t1);
        {
            Vector2 apos2; float angle2;
            findArrowPositionBezier(true, minLvlNodeScale, apos2, angle2, t2);
        }

        float start = fromSameHG ? t2 : t1;
        float end   = fromSameHG ? t1 : t2;
        float thick = std::clamp(EDGE_THICK * maxLvlNodeScale, 1.0f, EDGE_THICK);
        bool hover = DrawSplineSegmentBezierQuadraticPart(_pts[0], _pts[1], _pts[2], thick, RED, start, end);

        bool drawArrows = (maxLvlNodeScale > HIDE_ARROW_SCALE);
        if (drawArrows) {
            auto arrow = Edge::getArrowHead();
            float arscl = std::clamp(EDGE_THICK * maxLvlNodeScale, 1.0f, EDGE_THICK) * ARROW_SZ;
            Vector2 off = Vector2Rotate(Vector2{(float)arrow.width, (float)arrow.height} * 0.5f, angle);
            angle = 180.0f * angle / PI;
            DrawTextureEx(arrow, apos - off * arscl, angle, arscl, highlight ? ColorBrightness(RED, 0.25f) : RED);
        }

        return hover;
    }
}