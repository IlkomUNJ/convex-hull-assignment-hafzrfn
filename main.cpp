#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <QStatusBar>
#include "drawing_widget.h"
#include <Qt>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Main Window Setup
    QMainWindow mainWindow;
    mainWindow.setWindowTitle("Computer Graphics: Convex Hull Comparison (Qt Assignment)");

    QWidget *centralWidget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    DrawingWidget *drawingWidget = new DrawingWidget();
    mainLayout->addWidget(drawingWidget, 1);

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QPushButton *runButton = new QPushButton("Run Convex Hull (Compare)");
    QPushButton *clearButton = new QPushButton("Clear Canvas");

    buttonLayout->addWidget(runButton);
    buttonLayout->addWidget(clearButton);

    mainLayout->addLayout(buttonLayout);

    mainWindow.setCentralWidget(centralWidget);

    QStatusBar *statusBar = new QStatusBar();
    mainWindow.setStatusBar(statusBar);
    statusBar->showMessage("Welcome! Click the canvas to add points.");

    QObject::connect(clearButton, &QPushButton::clicked,
                     drawingWidget, &DrawingWidget::clearCanvas);

    QObject::connect(runButton, &QPushButton::clicked,
                     drawingWidget, &DrawingWidget::runConvexHull);

    QObject::connect(
        drawingWidget, SIGNAL(statusUpdate(QString)), // Normalized: removed 'const' and '&'
        statusBar, SLOT(showMessage(QString))       // Normalized: removed 'const' and '&'
        );

    mainWindow.show();
    return a.exec();
}
