#ifndef DRAWINGWIDGET_H
#define DRAWINGWIDGET_H

#include <QWidget>
#include <QPoint>
#include <QVector>
#include <QPainter>
#include <QString>
#include <algorithm>
#include <cmath>
#include <QMouseEvent>

struct Point {
    int x, y;
};

class DrawingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DrawingWidget(QWidget *parent = nullptr);
    enum DisplayMode { POINTS_ONLY, SLOW_HULL, FAST_HULL };

signals:
    void statusUpdate(const QString& message);

public slots:
    void clearCanvas();
    void runConvexHull();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<Point> points;
    QVector<Point> slowHull;
    QVector<Point> fastHull;

    DisplayMode currentMode;
    long long slowIterations;
    long long fastIterations;

    int orientation(Point p, Point q, Point r);

    void findSlowConvexHull(const QVector<Point>& data);
    void findFastConvexHull(const QVector<Point>& data);
};

#endif // DRAWINGWIDGET_H
