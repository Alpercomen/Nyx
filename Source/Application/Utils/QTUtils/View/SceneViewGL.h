#pragma once
#include <Application/Core/Engine/Engine.h>
#include <Application/Core/Services/Input/InputDispatcher.h>
#include <Application/Core/Services/Input/InputQueue.h>

#include <GL/glew.h>
#include <QOpenGLWidget>
#include <QCheckBox>

namespace Nyx 
{ 
    extern bool firstMouse;

    class SceneViewGL : public QOpenGLWidget {
        Q_OBJECT
    public:
        explicit SceneViewGL(QWidget* parent = nullptr);
        void setEngine(SharedPtr<Engine> e) { m_engine = e; }
        void setScene(SharedPtr<Scene> s) { m_scene = s; }

    signals:
        void glInitialized();
        void statusMessageRequested(QString);

    protected:
        void initializeGL() override;
        void resizeGL(int w, int h) override;
        void paintGL() override;
        void resizeEvent(QResizeEvent* e) override;

        // Input events
        void keyPressEvent(QKeyEvent* e) override;
        void keyReleaseEvent(QKeyEvent* e) override;
        void mousePressEvent(QMouseEvent* e) override;
        void mouseReleaseEvent(QMouseEvent* e) override;
        void mouseMoveEvent(QMouseEvent* e) override;
        void wheelEvent(QWheelEvent* e) override;
        void enterEvent(QEnterEvent* e) override;
        void leaveEvent(QEvent* e) override;

    private:
        void processKeyboard();
        void positionOverlay();

        SharedPtr<Engine> m_engine;
        SharedPtr<Scene>  m_scene;

        MouseButton m_pressedMask = MouseButton::NONE;
        bool m_cursorHidden = false;
        bool m_ignoreNextMove = false;
        bool m_primeLook = false;

        // Holds the coordinates of the center point of the scene
        double m_virtualX = 0.0;
        double m_virtualY = 0.0;

        QPoint m_prevPos;
        QPoint m_anchorGlobal;
        QSet<int> m_pressedKeys;
        QCheckBox* m_gridTick = nullptr;
    };
}
