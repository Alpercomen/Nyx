#pragma once
#include "SceneViewGL.h"
#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QCursor>

#include <Application/Core/Services/CameraService/CameraService.h>

namespace Nyx
{

    SceneViewGL::SceneViewGL(QWidget* parent) : QOpenGLWidget(parent)
    {
        setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
        setFocusPolicy(Qt::StrongFocus);
        setMouseTracking(true);

        // --- tiny grid tick overlay ---
        m_gridTick = new QCheckBox(tr("Grid"), this);
        m_gridTick->setChecked(true);  // default ON like before
        m_gridTick->setToolTip(tr("Show/hide grid"));
        // keep it small & subtle
        m_gridTick->setStyleSheet(
            "QCheckBox { font-size: 10px; color: #ddd; background: rgba(0,0,0,90); "
            "padding: 2px 6px; border-radius: 4px; }"
            "QCheckBox::indicator { width: 11px; height: 11px; }"
        );
        m_gridTick->raise(); // make sure it’s above the GL surface
        connect(m_gridTick, &QCheckBox::toggled, this, [this](bool on) {
            if (m_engine)
                m_engine->SetGrid(on);
            update(); // redraw quickly
        });

        positionOverlay();

        auto* t = new QTimer(this);
        connect(t, &QTimer::timeout, this, QOverload<>::of(&SceneViewGL::update));
        t->start(0);
    }

    void SceneViewGL::initializeGL()
    {
        if (!m_engine)
            return;

        glewExperimental = GL_TRUE;
        m_engine->EnsureGL(); // glewInit in this Qt context

        emit glInitialized();
    }

    void SceneViewGL::resizeGL(int w, int h)
    {
        if (m_engine && m_scene)
            m_engine->ResizeFBO({ (float)w,(float)h }, m_scene);
    }

    void SceneViewGL::paintGL()
    {
        processKeyboard();
        const int w = width(), h = height();

        if (m_engine && m_scene)
        {
            m_engine->Render(*m_scene); // renders into engine FBO in this context
            ::glBindFramebuffer(GL_READ_FRAMEBUFFER, m_engine->GetSceneFBO());
            ::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, defaultFramebufferObject());
            ::glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_LINEAR);
            ::glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
        }
        else
        {
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
    }

    void SceneViewGL::resizeEvent(QResizeEvent* e)
    {
        QOpenGLWidget::resizeEvent(e);
        positionOverlay();
    }

    /* ------------------ KEYBOARD ------------------ */

    void SceneViewGL::keyPressEvent(QKeyEvent* e)
    {
        if (!HasFlag(m_pressedMask, MouseButton::RIGHT_BUTTON))
            return;

        if (e->isAutoRepeat())
            return;

        InputEvent ev(EventType::KEY_PRESS);

        ev.m_eventList.keyCode = e->key();
        m_pressedKeys.insert(e->key());
        InputEventDispatcher::Get().DispatchCallback(ev); // Listener

        CameraService::Get().StopFocus();
        emit statusMessageRequested("Exited camera focus mode");
    }

    void SceneViewGL::keyReleaseEvent(QKeyEvent* e)
    {
        if (e->isAutoRepeat())
            return;

        InputEvent ev(EventType::KEY_RELEASE);
        ev.m_eventList.keyCode = e->key();
        m_pressedKeys.remove(e->key());
        InputEventDispatcher::Get().DispatchCallback(ev); // Listener
    }

    /* ------------------ MOUSE BUTTONS ------------------ */

    void SceneViewGL::mousePressEvent(QMouseEvent* e)
    {
        setFocus(Qt::MouseFocusReason);

        InputEvent ev(EventType::MOUSE_PRESS);
        switch (e->button())
        {
        case Qt::LeftButton:
            SetFlag(m_pressedMask, MouseButton::LEFT_BUTTON);
            break;
        case Qt::RightButton:
            SetFlag(m_pressedMask, MouseButton::RIGHT_BUTTON);
            break;
        case Qt::MiddleButton:
            SetFlag(m_pressedMask, MouseButton::MIDDLE_BUTTON);
            break;
        default:
            break;
        }

        ev.m_eventList.mouseButton = m_pressedMask;
        InputEventDispatcher::Get().DispatchCallback(ev);

        if (e->button() == Qt::RightButton && !m_cursorHidden)
        {
            m_cursorHidden = true;
            setCursor(Qt::BlankCursor);
            grabMouse();

            m_anchorGlobal = mapToGlobal(rect().center());

            m_virtualX = width() * 0.5;
            m_virtualY = height() * 0.5;

            firstMouse = true;

            {
                InputEvent init(EventType::MOUSE_MOVE);
                init.m_eventList.mouseX = static_cast<int>(m_virtualX);
                init.m_eventList.mouseY = static_cast<int>(m_virtualY);
                InputEventDispatcher::Get().DispatchCallback(init);
            }

            m_ignoreNextMove = true;
            QCursor::setPos(m_anchorGlobal);

            m_primeLook = true;
        }
    }

    void SceneViewGL::mouseReleaseEvent(QMouseEvent* e)
    {
        InputEvent ev(EventType::MOUSE_RELEASE);

        switch (e->button())
        {
        case Qt::LeftButton:
            ClearFlag(m_pressedMask, MouseButton::LEFT_BUTTON);
            break;
        case Qt::RightButton:
            ClearFlag(m_pressedMask, MouseButton::RIGHT_BUTTON);
            break;
        case Qt::MiddleButton:
            ClearFlag(m_pressedMask, MouseButton::MIDDLE_BUTTON);
            break;
        default:
            break;
        }

        ev.m_eventList.mouseButton = m_pressedMask;
        InputEventDispatcher::Get().DispatchCallback(ev);

        if (e->button() == Qt::RightButton && m_cursorHidden)
        {
            m_cursorHidden = false;
            releaseMouse();
            unsetCursor();
        }
    }

    /* ------------------ MOUSE MOVE / WHEEL ------------------ */

    void SceneViewGL::mouseMoveEvent(QMouseEvent* e)
    {
        if (!HasFlag(m_pressedMask, Nyx::MouseButton::RIGHT_BUTTON) || !m_cursorHidden)
            return;

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
        const QPoint global = e->globalPosition().toPoint();
#else
        const QPoint global = e->globalPos();
#endif

        if (m_ignoreNextMove)
        {
            m_ignoreNextMove = false;
            return;
        }

        // relative delta from anchor, scaled by DPI
        const qreal scale = devicePixelRatioF();
        const QPoint d = global - m_anchorGlobal;

        m_virtualX += d.x() * scale;
        m_virtualY += d.y() * scale;

        if (m_primeLook)
        {
            m_primeLook = false;
            m_ignoreNextMove = true;
            QCursor::setPos(m_anchorGlobal);
            return;
        }

        InputEvent ev(EventType::MOUSE_MOVE);
        ev.m_eventList.mouseX = static_cast<int>(m_virtualX);
        ev.m_eventList.mouseY = static_cast<int>(m_virtualY);
        InputEventDispatcher::Get().DispatchCallback(ev);

        m_ignoreNextMove = true;
        QCursor::setPos(m_anchorGlobal);
    }

    void SceneViewGL::wheelEvent(QWheelEvent* e)
    {
        InputEvent ev(EventType::MOUSE_SCROLL_WHEEL);
        ev.m_eventList.scrollDelta = float(e->angleDelta().y()) / 120.0f;
        InputEventDispatcher::Get().DispatchCallback(ev);
    }

    /* ----------- focus helpers (optional) ------------ */

    void SceneViewGL::enterEvent(QEnterEvent*)
    {
        m_prevPos = mapFromGlobal(QCursor::pos());
    }

    void SceneViewGL::leaveEvent(QEvent*)
    {
        if (m_cursorHidden)
        {
            releaseMouse();
            unsetCursor();
            m_cursorHidden = false;
        }
    }

    /* ---------------------- Helper functions ---------------------- */

    void SceneViewGL::processKeyboard()
    {
        if (!m_cursorHidden)
            return;

        auto& cameraIDs = ECS::Get().GetAllComponentIDs<Camera>();
        if (cameraIDs.empty())
            return;

        const EntityID& id = cameraIDs[0];
        Camera& camera = *ECS::Get().GetComponent<Camera>(id);

        float effectiveDelta = DELTA_TIME;

        if (m_pressedKeys.contains(Qt::Key_Shift))
            effectiveDelta *= camera.GetMovementSpeedMultiplier();
        if (m_pressedKeys.contains(Qt::Key_Alt))
            effectiveDelta /= camera.GetMovementSpeedMultiplier();

        if (m_pressedKeys.contains(Qt::Key_W))
            camera.ProcessKeyboardMovement(FORWARD, effectiveDelta);
        if (m_pressedKeys.contains(Qt::Key_S))
            camera.ProcessKeyboardMovement(BACKWARD, effectiveDelta);
        if (m_pressedKeys.contains(Qt::Key_A))
            camera.ProcessKeyboardMovement(LEFT, effectiveDelta);
        if (m_pressedKeys.contains(Qt::Key_D))
            camera.ProcessKeyboardMovement(RIGHT, effectiveDelta);
        if (m_pressedKeys.contains(Qt::Key_Space))
            camera.ProcessKeyboardMovement(UP, effectiveDelta);
        if (m_pressedKeys.contains(Qt::Key_Control))
            camera.ProcessKeyboardMovement(DOWN, effectiveDelta);
    }

    void SceneViewGL::positionOverlay()
    {
        if (!m_gridTick)
            return;

        const int margin = 8;
        const QSize s = m_gridTick->sizeHint();

        m_gridTick->move(width() - s.width() - margin, margin);
    }

}
