#include "drawing_widget.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPalette>

// --- DrawingWidget Implementation ---

DrawingWidget::DrawingWidget(QWidget *parent)
    : QWidget(parent), currentMode(POINTS_ONLY), slowIterations(0), fastIterations(0)
{
    // Set white background for the canvas
    setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, Qt::white);
    setPalette(palette);
    setMinimumSize(600, 400);
}

int DrawingWidget::orientation(Point p, Point q, Point r)
{
    long long val = (long long)(q.y - p.y) * (r.x - q.x) -
                    (long long)(q.x - p.x) * (r.y - q.y);

    if (val == 0) return 0;
    return (val > 0) ? 1 : 2; 
}

// --- ALGORITHM 1: SLOW (BRUTE FORCE) CONVEX HULL O(n^3) ---
void DrawingWidget::findSlowConvexHull(const QVector<Point>& data)
{
    slowHull.clear();
    slowIterations = 0;
    int n = data.size();
    if (n < 3) { slowHull = data; return; }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;
            bool is_hull_edge = true;
            int required_orientation = 0;

            for (int k = 0; k < n; ++k) {
                if (k == i || k == j) continue;

                slowIterations++; // *ITERATION COUNTED HERE*

                int o = orientation(data[i], data[j], data[k]);

                if (o == 0) continue;

                if (required_orientation == 0) {
                    required_orientation = o;
                } else if (required_orientation != o) {
                    is_hull_edge = false;
                    break;
                }
            }

            if (is_hull_edge) {
                // Add the unique hull point
                bool already_added = false;
                for (const auto& p : slowHull) {
                    if (p.x == data[i].x && p.y == data[i].y) {
                        already_added = true;
                        break;
                    }
                }
                if (!already_added) slowHull.append(data[i]);
            }
        }
    }
}

// --- ALGORITHM 2: FAST (JARVIS MARCH / GIFT WRAPPING) CONVEX HULL O(n*h) ---
void DrawingWidget::findFastConvexHull(const QVector<Point>& data)
{
    fastHull.clear();
    fastIterations = 0;
    int n = data.size();
    if (n < 3) { fastHull = data; return; }

    // Start with the leftmost point
    int l = 0;
    for (int i = 1; i < n; i++) {
        if (data[i].x < data[l].x) l = i;
    }

    int p = l, q;
    do {
        fastHull.append(data[p]);
        q = (p + 1) % n;

        for (int i = 0; i < n; i++) {
            fastIterations++; // *ITERATION COUNTED HERE*
            if (orientation(data[p], data[i], data[q]) == 2) {
                q = i;
            }
        }
        p = q;

    } while (p != l); // Loop until we return to the starting point
}

// --- EVENT AND DRAWING HANDLERS ---

void DrawingWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        points.append({event->x(), event->y()});
        slowHull.clear();
        fastHull.clear();
        currentMode = POINTS_ONLY;
        update();
        emit statusUpdate(QString("Added point. Total points: %1").arg(points.size()));
    }
}

void DrawingWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 1. Draw all input points
    painter.setPen(QPen(Qt::black, 1));
    painter.setBrush(QBrush(Qt::red));
    for (const auto& p : points) {
        painter.drawEllipse(p.x - 4, p.y - 4, 8, 8);
    }

    // 2. Draw the Convex Hull
    QVector<Point> *hullToDraw = nullptr;
    QString hullType = "";
    long long iterations = 0;

    if (currentMode == SLOW_HULL && !slowHull.isEmpty()) {
        hullToDraw = &slowHull;
        hullType = "Slow Hull (O(n³))";
        iterations = slowIterations;
        painter.setPen(QPen(Qt::blue, 3, Qt::DashLine));
    } else if (currentMode == FAST_HULL && !fastHull.isEmpty()) {
        hullToDraw = &fastHull;
        hullType = "Fast Hull (Jarvis March O(n⋅h))";
        iterations = fastIterations;
        painter.setPen(QPen(Qt::darkGreen, 3, Qt::SolidLine));
    }

    if (hullToDraw && !hullToDraw->isEmpty()) {
        QPolygonF polygon;
        for (const auto& p : *hullToDraw) {
            polygon << QPointF(p.x, p.y);
        }
        painter.drawPolygon(polygon);

        // 3. Draw iteration count text (required by assignment)
        painter.setPen(QPen(Qt::black, 1));
        painter.setFont(QFont("Arial", 12, QFont::Bold));

        QString iterationText = QString("%1\nTotal Iterations (Orientations Checked): %2")
                                    .arg(hullType)
                                    .arg(iterations);

        painter.drawText(rect().adjusted(10, 10, -10, -10), Qt::AlignTop | Qt::AlignRight, iterationText);
    }
}

void DrawingWidget::clearCanvas()
{
    points.clear();
    slowHull.clear();
    fastHull.clear();
    currentMode = POINTS_ONLY;
    slowIterations = 0;
    fastIterations = 0;
    emit statusUpdate("Canvas cleared. Start clicking to add new points.");
    update();
}

void DrawingWidget::runConvexHull()
{
    if (points.size() < 3) {
        emit statusUpdate("Cannot run Convex Hull: Need at least 3 points.");
        return;
    }

    findSlowConvexHull(points);
    findFastConvexHull(points);

    // Toggle display mode
    if (currentMode == FAST_HULL || currentMode == POINTS_ONLY) {
        currentMode = SLOW_HULL;
        emit statusUpdate(QString("Switched to SLOW Hull (Blue Dashed). Iterations: %1. Click RUN again to see Fast Hull.").arg(slowIterations));
    } else {
        currentMode = FAST_HULL;
        emit statusUpdate(QString("Switched to FAST Hull (Green Solid). Iterations: %1. Click RUN again to see Slow Hull.").arg(fastIterations));
    }

    update();
}
