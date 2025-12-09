// --------------------------------------------------------------
//  PCLMockAPI.cpp  —  Unified Mock PixInsight API for Testing
// --------------------------------------------------------------
//  ARCHITECTURE:
//  - control_handle is ALWAYS pcl::Control* (the client pointer)
//  - g_objects maps: pcl::Control* -> MockBase*
//  - MockBase wraps: QWidget* OR QTreeWidgetItem* OR QLayout*
//  - All handles are consistent: client pointer is the key
// --------------------------------------------------------------

#include "PCLMockAPI.h"
#include "PCLThreadMock.h"
#include <unordered_map>
#include <memory>
#include <cstdio>
#include <fftw3.h>
#include <QSvgRenderer>
#include <QScrollArea>
#include <QFontMetrics>
#include <QFont>
#include <QTreeWidget>
#include <pcl/XISF.h>
#include <pcl/api/APIInterface.h>

// ImageWindowMock_Implementation.cpp
// Implementations for ImageWindow stubs to support test image creation
//
// Add these to PCLMockAPI.cpp to enable image window functionality

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QScrollArea>
#include <map>
#include <vector>
#include <string>

view_handle ImageWindowContext::GetPreviewById(const_window_handle, const char*) { return nullptr; }

void ImageWindowContext::EnumeratePreviews(const_window_handle, pcl::view_enumeration_callback, void*) {}

api_bool ImageWindowContext::LoadImageWindows(const char16_type*,
					      const char*,
					      const char*,
					      api_bool,
					      api_bool,
					      pcl::window_enumeration_callback,
					      void*)
{
    return api_false;
}

static bool g_enableDebugLogging = false;

void SetDebugLogging(bool on)
{
   g_enableDebugLogging = on;
}

// =============================================================
// Event Filter for Qt Events
// =============================================================

class MockEventFilter : public QObject
{
public:
    explicit MockEventFilter(MockBase* b)
        : QObject(b->widget), base(b) {}

protected:
    bool eventFilter(QObject* obj, QEvent* ev) override
    {
        if (!base || obj != base->widget)
            return QObject::eventFilter(obj, ev);

        // --- SHOW EVENT ------------------------------------------------------
        if (ev->type() == QEvent::Show)
        {
            if (base->onShow)
                base->onShow(base->pcl_handle, base->pcl_handle);
        }

        // --- MOUSE MOVE ------------------------------------------------------
        if (ev->type() == QEvent::MouseMove)
        {
            if (base->onMouseMove)
            {
                auto* e = static_cast<QMouseEvent*>(ev);
                base->onMouseMove(base->pcl_handle,
                                  base->pcl_handle,
                                  e->x(),
                                  e->y(),
                                  e->buttons(),
                                  QApplication::keyboardModifiers());
                return false;
            }
        }

        // --- MOUSE PRESS -----------------------------------------------------
        if (ev->type() == QEvent::MouseButtonPress)
        {
            if (base->onMousePress)
            {
                auto* e = static_cast<QMouseEvent*>(ev);
                base->onMousePress(base->pcl_handle,
                                   base->pcl_handle,
                                   e->x(),
                                   e->y(),
                                   e->button(),
                                   e->buttons(),
                                   QApplication::keyboardModifiers());
                return false;
            }
        }

        // --- MOUSE RELEASE ---------------------------------------------------
        if (ev->type() == QEvent::MouseButtonRelease)
        {
            if (base->onMouseRelease)
            {
                auto* e = static_cast<QMouseEvent*>(ev);
                base->onMouseRelease(base->pcl_handle,
                                     base->pcl_handle,
                                     e->x(),
                                     e->y(),
                                     e->button(),
                                     e->buttons(),
                                     QApplication::keyboardModifiers());
                return false;
            }
        }

        // --- KEY PRESS -------------------------------------------------------
        if (ev->type() == QEvent::KeyPress)
        {
            if (base->onKeyPress)
            {
                auto* e = static_cast<QKeyEvent*>(ev);
                // Signature commented out in original
                base->onKeyPress(base->pcl_handle,
                                 base->pcl_handle,
                                 e->key(),
                                 QApplication::keyboardModifiers());
                return false;
            }
        }

        return QObject::eventFilter(obj, ev);
    }

private:
    MockBase* base;
};

// =============================================================
// Global Object Registry
// =============================================================

template <typename H>
struct HandleHash
{
   std::size_t operator()(const void *h) const noexcept
   {
      auto p = reinterpret_cast<std::uintptr_t>(h);
      return std::hash<std::uintptr_t>{}(p);
   }
};
 
template <typename H>
struct HandleEqual
{
   bool operator()(const void *a, const void *b) const noexcept
   {
      return a == b;
   }
};

// The global object map: client pointer -> MockBase
static std::unordered_map<const void *, std::unique_ptr<MockBase>, 
                          HandleHash<control_handle>, 
                          HandleEqual<control_handle>> g_objects;

QList<MockBase*> g_topLevelWidgets;

// =============================================================
// Accessor Functions - SIMPLIFIED
// =============================================================

// Generic accessor - returns MockBase for any handle
static inline MockBase* get(const void *h)
{
    if (!h)
        return nullptr;
    auto it = g_objects.find(h);
    return (it == g_objects.end()) ? nullptr : it->second.get();
}

// Widget accessor
static inline QWidget* widgetFromHandle(control_handle h)
{
    MockBase* b = get(h);
    return (b ? b->widget : nullptr);
}

static inline QWidget* widgetFromHandle(const_control_handle h)
{
    return widgetFromHandle(const_cast<control_handle>(h));
}

// Layout accessor - returns QBoxLayout for sizer handles
static inline QBoxLayout* layoutFromSizer(sizer_handle s)
{
    if (!s)
        return nullptr;

    MockBase* b = get(s);
    return (b && b->isSizer) ? b->layout : nullptr;
}

static inline QBoxLayout* layoutFromSizer(const_sizer_handle s)
{
    if (!s)
        return nullptr;
        
    // Cast away const from the void*, then pass to non-const version
    MockBase* b = get(const_cast<void*>(s));
    return (b && b->isSizer) ? b->layout : nullptr;
}

// Tree widget accessor
static inline QTreeWidget* treeFromHandle(control_handle h)
{
    QWidget* w = widgetFromHandle(h);
    return qobject_cast<QTreeWidget*>(w);
}

static inline QTreeWidget* treeFromHandle(const_control_handle h)
{
    return treeFromHandle(const_cast<control_handle>(h));
}

// Tree item accessor
static inline QTreeWidgetItem* treeItemFromHandle(api_handle h)
{
    MockBase* b = get(h);
    return (b && b->isTreeNode) ? b->treeItem : nullptr;
}

static inline QTreeWidgetItem* treeItemFromHandle(const_api_handle h)
{
    return treeItemFromHandle(const_cast<api_handle>(h));
}

// =============================================================
// Utility: Logging
// =============================================================

static inline void logf(const char* fmt, ...)
{
    if (!g_enableDebugLogging)
        return;
        
    va_list ap; 
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap); 
    fprintf(stderr, "\n");
    va_end(ap);
}

// =============================================================
// Helper: Determine Parent Widget for Control Creation
// =============================================================

static QWidget* determineParentWidget(control_handle parent)
{
    if (!parent)
        return nullptr;  // Top-level widget
    
    MockBase* p = get(parent);
    if (!p)
        return nullptr;
    
    // Parent is a widget
    if (!p->isSizer && p->widget)
        return p->widget;
    
    // Parent is a layout - use layout's parent widget
    if (p->isSizer && p->layout)
        return p->layout->parentWidget();
    
    return nullptr;
}

// =============================================================
// CONTROL CREATION TEMPLATE - Consistent Architecture
// =============================================================

template <class T>
control_handle createControl(api_handle module, 
                             control_handle client,      // PCL Control* pointer
                             control_handle parent)
{
    // Determine parent Qt widget
    QWidget* parentWidget = determineParentWidget(parent);
    
    // Create Qt widget
    T* qtWidget = new T(parentWidget);
    
    // Create mock wrapper
    auto* mock = new MockBase();
    mock->isSizer = false;
    mock->isTreeNode = false;
    mock->moduleHandle = module;
    mock->widget = qtWidget;
    mock->layout = nullptr;
    mock->treeItem = nullptr;
    mock->pcl_handle = client;
    
    // Install event filter
    qtWidget->installEventFilter(new MockEventFilter(mock));
    
    // CRITICAL: Use CLIENT (pcl::Control*) as key
    g_objects[client] = std::unique_ptr<MockBase>(mock);
    
    logf("[Mock] createControl<%s> client=%p qtWidget=%p parent=%p",
         typeid(T).name(), client, qtWidget, parentWidget);
    
    // Return the PCL Control pointer
    return client;
}

// =============================================================
// ControlContext API
// =============================================================
control_handle ControlContext::GetControlWindow(const_control_handle handle)
{
    MockBase* b = get(handle);
    if (!b || !b->widget)
        return nullptr;

    QWidget* w = b->widget;
    QWidget* top = w->window();

    for (auto& kv : g_objects)
    {
        MockBase* obj = kv.second.get();
        if (obj->widget == top)
            return reinterpret_cast<control_handle>(const_cast<void*>(kv.first));  // FIX: reinterpret_cast
    }

    return const_cast<control_handle>(handle);
}

control_handle ControlContext::CreateControl(
    api_handle module,
    control_handle pclCtrl,
    control_handle parent,
    uint32 flags)
{
    return createControl<QWidget>(module, pclCtrl, parent);
}

void ControlContext::SetControlVisible(control_handle h, api_bool visible)
{
    if (!h) {
        qWarning() << "[SetControlVisible] NULL handle - ignoring";
        return;
    }

    QWidget* w = widgetFromHandle(h);
    if (!w)
        return;

    if (visible) {
        w->show();
        w->raise();
    } else {
        w->hide();
    }
}

// =============================================================
// Size Sanitization Helpers
// =============================================================

inline int sanitizePCL(int v, QWidget* w, int prev)
{
    if (prev > 800)
      {
	logf("SetControlHeight previous %d > 800", v);
	prev = 800;
      }
    if (prev <= 0) prev = 0;
    
    if (v <= 0 || v > 800) return prev;
    
    return v;
}

void ControlContext::SetControlFixedSize(control_handle h, int32 w, int32 hgt)
{
    QWidget* widget = widgetFromHandle(h);
    if (!widget) return;

    logf("[Mock] SetControlFixedSize: %d x %d", w, hgt);
    
    int pxW = sanitizePCL(w, widget, widget->sizeHint().width());
    int pxH = sanitizePCL(hgt, widget, widget->sizeHint().height());
    
    widget->setFixedSize(pxW, pxH);
}

void ControlContext::SetControlMinSize(control_handle h, int32 w, int32 hgt)
{
    QWidget* widget = widgetFromHandle(h);
    if (!widget) return;
    
    int pxW = sanitizePCL(w, widget, widget->sizeHint().width());
    int pxH = sanitizePCL(hgt, widget, widget->sizeHint().height());
    
    logf("[Mock] SetControlMinSize: %d x %d", pxW, pxH);
    widget->setMinimumSize(pxW, pxH);
}

void ControlContext::SetWindowToolTip(control_handle h, const char16_type* t)
{
    QWidget* w = widgetFromHandle(h);
    if (w)
        w->setToolTip(QString::fromUtf16(t));
}

void ControlContext::SetWindowTitle(control_handle h, const char16_type* t)
{
    QWidget* w = widgetFromHandle(h);
    if (w)
        w->setWindowTitle(QString::fromUtf16(t));
}

void ControlContext::EnsureControlLayoutUpdated(control_handle h)
{
    QWidget* w = widgetFromHandle(h);
    if (!w) return;
    
    if (auto* l = w->layout()) {
        l->invalidate();
        l->activate();
    }
    w->updateGeometry();
    w->update();
}

void ControlContext::SetControlBackgroundColor(control_handle h, uint32 rgba)
{
    // Stub - implement if needed
}

void ControlContext::GetClientRect(const_control_handle h,
                                   int32* x, int32* y,
                                   int32* w, int32* hgt)
{
    if (!w || !hgt)
        return;

    QWidget* widget = widgetFromHandle(h);
    if (!widget)
        return;

    const QRect r = widget->contentsRect();

    if (x)   *x   = r.x();
    if (y)   *y   = r.y();
    *w   = r.width();
    *hgt = r.height();
}

// Event routine setters
api_bool ControlContext::SetKeyPressEventRoutine(
    control_handle h,
    control_handle receiver,
    pcl::keyboard_event_routine r)
{
    MockBase* b = get(h);
    if (b)
      {
	b->pcl_handle = receiver;
	b->onKeyPress = r;
	return api_true;
      }
    else
      return api_false;
}

api_bool ControlContext::SetMousePressEventRoutine(
    control_handle h,
    control_handle receiver,
    pcl::mouse_button_event_routine r)
{
    MockBase* b = get(h);
    if (b)
      {
	b->pcl_handle = receiver;
	b->onMousePress = r;
	return api_true;
      }
    else
      return api_false;
}

api_bool ControlContext::SetMouseReleaseEventRoutine(
    control_handle h,
    control_handle /*client*/,
    pcl::mouse_button_event_routine r)
{
    MockBase* b = get(h);
    if (b) b->onMouseRelease = r;
    return api_true;
}

api_bool ControlContext::SetMouseMoveEventRoutine(
    control_handle h,
    control_handle /*client*/,
    pcl::mouse_event_routine r)
{
    MockBase* b = get(h);
    if (b) b->onMouseMove = r;
    return api_true;
}

api_bool ControlContext::SetShowEventRoutine(
    control_handle h,
    control_handle /*client*/,
    pcl::control_event_routine r)
{
    MockBase* b = get(h);
    if (b) b->onShow = r;
    return api_true;
}

api_bool ControlContext::SetHideEventRoutine(
    control_handle h,
    control_handle /*client*/,
    pcl::control_event_routine r)
{
    MockBase* b = get(h);
    if (b) b->onShow = r;  // Note: Original had this as onShow
    return api_true;
}

// ... Continue with rest of the implementation (sizers, labels, buttons, etc.) ...

// =============================================================
// Sizer Creation
// =============================================================

sizer_handle SizerContext::CreateSizer(api_handle module, api_bool vertical)
{
    auto* b = new MockBase();
    b->isSizer = true;
    b->isTreeNode = false;
    b->vertical = vertical;
    b->moduleHandle = module;
    b->widget = nullptr;
    b->treeItem = nullptr;

    b->layout = vertical
        ? static_cast<QBoxLayout*>(new QVBoxLayout())
        : static_cast<QBoxLayout*>(new QHBoxLayout());

    // Use MockBase pointer as the handle for sizers
    // This is different from controls where we use client pointer
    sizer_handle h = reinterpret_cast<sizer_handle>(b);
    g_objects[h] = std::unique_ptr<MockBase>(b);

    logf("[Mock] CreateSizer vertical=%d handle=%p", vertical, h);
    return h;
}

// =============================================================
// Sizer Insertion
// =============================================================

void SizerContext::InsertSizerControl(
    sizer_handle s, int32 index, control_handle c, int32 stretch, int32 /*flags*/)
{
    logf("[Mock] InsertSizerControl: sizer=%p control=%p", s, c);
    
    QBoxLayout* layout = layoutFromSizer(s);
    QWidget* widget = widgetFromHandle(c);
    
    if (!layout || !widget)
        return;

    if (index < 0 || index >= layout->count())
        layout->addWidget(widget, stretch);
    else
        layout->insertWidget(index, widget, stretch);
}

void SizerContext::InsertSizer(
    sizer_handle s, int32 index, sizer_handle child, int32 stretch)
{
    logf("[Mock] InsertSizer: parent=%p child=%p", s, child);
 
    QBoxLayout* parentLayout = layoutFromSizer(s);
    QBoxLayout* childLayout = layoutFromSizer(child);
    
    if (!parentLayout || !childLayout)
        return;

    if (index < 0 || index >= parentLayout->count())
        parentLayout->addLayout(childLayout, stretch);
    else
        parentLayout->insertLayout(index, childLayout, stretch);
}

void SizerContext::InsertSizerSpacing(sizer_handle s, int32 index, int32 px)
{
    QBoxLayout* layout = layoutFromSizer(s);
    if (!layout) return;
    
    if (index < 0 || index > layout->count())
        index = layout->count();
    layout->insertSpacing(index, px);
}

void SizerContext::InsertSizerStretch(sizer_handle s, int32 index, int32 stretch)
{
    QBoxLayout* layout = layoutFromSizer(s);
    if (!layout) return;
    
    if (index < 0 || index > layout->count())
        index = layout->count();
    layout->insertStretch(index, stretch);
}

void SizerContext::SetSizerMargin(sizer_handle s, int32 px)
{
    QBoxLayout* layout = layoutFromSizer(s);
    if (layout)
        layout->setContentsMargins(px, px, px, px);
}

void SizerContext::SetSizerSpacing(sizer_handle s, int32 px)
{
    QBoxLayout* layout = layoutFromSizer(s);
    if (layout)
        layout->setSpacing(px);
}

api_bool SizerContext::GetSizerDisplayPixelRatio(const_sizer_handle s, double* ratio)
{
    if (!ratio)
        return api_false;

    QBoxLayout* l = layoutFromSizer(s);
    double r = 1.0;

    if (l && l->parentWidget())
        r = l->parentWidget()->devicePixelRatioF();

    *ratio = r;
    return api_true;
}

// =============================================================
// Attach Sizer to Control
// =============================================================

void ControlContext::SetControlSizer(control_handle ctrl, sizer_handle s)
{
    MockBase* C = get(ctrl);
    MockBase* S = get(s);

    if (!S || !S->isSizer || !S->layout) {
        logf("[Mock] SetControlSizer: invalid sizer ctrl=%p sizer=%p", ctrl, s);
        return;
    }

    // If there is no MockBase for ctrl yet, create one
    if (!C) {
        C = new MockBase();
        C->isSizer = false;
        C->isTreeNode = false;
        C->moduleHandle = nullptr;
        C->widget = nullptr;
        C->layout = nullptr;
        C->treeItem = nullptr;
        g_objects[ctrl] = std::unique_ptr<MockBase>(C);
    }

    // If this "control" has no widget, treat it as a top-level window
    if (!C->widget) {
        C->widget = new QWidget(nullptr);
        C->widget->setObjectName("MockTopLevelWindow");
        g_topLevelWidgets.append(C);

        logf("[Mock] SetControlSizer: created top-level container ctrl=%p widget=%p",
             ctrl, C->widget);
    }

    QWidget* container = C->widget;
    QLayout* layout = S->layout;

    logf("[Mock] SetControlSizer: ctrl=%p widget=%p sizer=%p layout=%p",
         ctrl, container, s, layout);

    container->setLayout(layout);
    layout->setParent(container);
}

// =============================================================
// Label Context
// =============================================================

control_handle LabelContext::CreateLabel(
    api_handle m, control_handle c, const char16_type*, control_handle parent, uint32 flags)
{
    return createControl<QLabel>(m, c, parent);
}

void LabelContext::SetLabelText(control_handle h, const char16_type* t)
{
    QLabel* label = qobject_cast<QLabel*>(widgetFromHandle(h));
    if (label)
        label->setText(QString::fromUtf16(t));
}

void LabelContext::SetLabelAlignment(control_handle h, int32 flags)
{
    QLabel* label = qobject_cast<QLabel*>(widgetFromHandle(h));
    if (!label) return;
    
    Qt::Alignment a{};
    uint32 f = uint32(flags);

    if (f & 0x01) a |= Qt::AlignLeft;
    if (f & 0x02) a |= Qt::AlignHCenter;
    if (f & 0x04) a |= Qt::AlignRight;
    if (f & 0x10) a |= Qt::AlignTop;
    if (f & 0x20) a |= Qt::AlignVCenter;
    if (f & 0x40) a |= Qt::AlignBottom;

    if (!a) a = Qt::AlignLeft | Qt::AlignVCenter;

    label->setAlignment(a);
}

// =============================================================
// Edit Context
// =============================================================

control_handle EditContext::CreateEdit(
    api_handle m, control_handle c, const char16_type*, control_handle parent, uint32 flags)
{
    return createControl<QLineEdit>(m, c, parent);
}

void EditContext::SetEditText(control_handle h, const char16_type* t)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(widgetFromHandle(h));
    if (edit)
        edit->setText(QString::fromUtf16(t));
}

api_bool EditContext::SetEditValidatingRegExp(
    control_handle h,
    const char16_type* pattern,
    api_bool caseSensitive)
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(widgetFromHandle(h));
    if (!edit) return api_false;

    QString pat = QString::fromUtf16(pattern);
    QRegularExpression re(pat);

    if (!caseSensitive)
        re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

    edit->setValidator(new QRegularExpressionValidator(re, edit));
    return api_true;
}

// =============================================================
// Slider Context
// =============================================================

control_handle SliderContext::CreateSlider(
    api_handle m, control_handle c, api_bool vertical, control_handle parent, uint32 flags)
{
    return createControl<QSlider>(m, c, parent);
}

void SliderContext::SetSliderValue(control_handle h, int value)
{
    QSlider* slider = qobject_cast<QSlider*>(widgetFromHandle(h));
    if (slider)
        slider->setValue(value);
}

void SliderContext::SetSliderRange(control_handle h, int32 mn, int32 mx)
{
    QSlider* slider = qobject_cast<QSlider*>(widgetFromHandle(h));
    if (slider) {
        slider->setMinimum(mn);
        slider->setMaximum(mx);
    }
}

void SliderContext::SetSliderPageSize(control_handle h, int32 page)
{
    QSlider* slider = qobject_cast<QSlider*>(widgetFromHandle(h));
    if (slider)
        slider->setPageStep(page);
}

void SliderContext::SetSliderTickInterval(control_handle h, int32 interval)
{
    QSlider* slider = qobject_cast<QSlider*>(widgetFromHandle(h));
    if (slider)
        slider->setTickInterval(interval);
}

void SliderContext::SetSliderTickStyle(control_handle h, int32 style)
{
    QSlider* slider = qobject_cast<QSlider*>(widgetFromHandle(h));
    if (!slider) return;
    
    uint32 st = uint32(style);
    QSlider::TickPosition pos = QSlider::NoTicks;

    if (st & 0x01) pos = QSlider::TicksAbove;
    if (st & 0x02) pos = QSlider::TicksBelow;
    if (st & 0x03) pos = QSlider::TicksBothSides;

    slider->setTickPosition(pos);
}

void SliderContext::GetSliderRange(const_control_handle, int32* minValue, int32* maxValue) { *minValue = 0; *maxValue = 100; }
int32 SliderContext::GetSliderValue(const_control_handle) { return 0; }
api_bool SliderContext::SetSliderValueUpdatedEventRoutine(control_handle, api_handle, pcl::value_event_routine) { return api_true; }

// =============================================================
// SpinBox Context
// =============================================================

control_handle SpinBoxContext::CreateSpinBox(api_handle module, control_handle client, control_handle parent, uint32 flags)
{
    return createControl<QSpinBox>(module, client, parent);
}

void SpinBoxContext::SetSpinBoxRange(control_handle h, int minv, int maxv)
{
    QSpinBox* spin = qobject_cast<QSpinBox*>(widgetFromHandle(h));
    if (spin)
        spin->setRange(minv, maxv);
}

void SpinBoxContext::SetSpinBoxValue(control_handle h, int value)
{
    QSpinBox* spin = qobject_cast<QSpinBox*>(widgetFromHandle(h));
    if (spin)
        spin->setValue(value);
}

// =============================================================
// Button Context
// =============================================================

control_handle ButtonContext::CreateCheckBox(
    api_handle m, control_handle c, const char16_type*, control_handle parent, uint32 flags)
{
    return createControl<QCheckBox>(m, c, parent);
}

control_handle ButtonContext::CreatePushButton(
    api_handle m, control_handle c,
    const char16_type* text,
    const_bitmap_handle icon,
    control_handle parent,
    uint32 flags)
{
    control_handle h = createControl<QPushButton>(m, c, parent);
    QPushButton* btn = qobject_cast<QPushButton*>(widgetFromHandle(h));
	
    if (text) {
        if (btn)
            btn->setText(QString::fromUtf16(text));
    }
    
    // Set icon if provided
    if (icon) {
        QPixmap* pixmap = reinterpret_cast<QPixmap*>(const_cast<void*>(icon));
        if (pixmap) {
            btn->setIcon(QIcon(*pixmap));
        }
    }
    
    // Set parent if provided
    if (parent) {
        QWidget* parentWidget = reinterpret_cast<QWidget*>(parent);
        btn->setParent(parentWidget);
    }

    logf("[Mock] CreatePushButton handle=%p", h);
    return h;
}

control_handle ButtonContext::CreateRadioButton(
    api_handle m, control_handle c,
    const char16_type* text,
    control_handle parent,
    uint32 flags)
{
    control_handle h = createControl<QRadioButton>(m, c, parent);
    
    if (text) {
        QRadioButton* btn = qobject_cast<QRadioButton*>(widgetFromHandle(h));
        if (btn)
            btn->setText(QString::fromUtf16(text));
    }
    
    logf("[Mock] CreateRadioButton handle=%p", h);
    return h;
}

control_handle ButtonContext::CreateToolButton(
    api_handle m, control_handle c,
    const char16_type* text,
    const_bitmap_handle icon,
    api_bool checkable,
    control_handle parent,
    uint32 flags)
{
    control_handle h = createControl<QToolButton>(m, c, parent);
    
    QToolButton* btn = qobject_cast<QToolButton*>(widgetFromHandle(h));
    if (!btn) return h;
    
    if (text)
        btn->setText(QString::fromUtf16(text));
    
    btn->setCheckable(checkable);
    
    logf("[Mock] CreateToolButton handle=%p checkable=%d", h, checkable);
    return h;
}

api_bool ButtonContext::GetButtonText(const_control_handle h,
                                  char16_type* text,
                                  size_type* len)
{
    QAbstractButton* btn = qobject_cast<QAbstractButton*>(widgetFromHandle(h));
    if (!btn) return api_false;
    
    QString qtext = btn->text();
    
    if (len) *len = qtext.length();
    
    if (text && len && *len > 0) {
        const char16_type* src = reinterpret_cast<const char16_type*>(qtext.utf16());
        size_t copyLen = std::min(*len, static_cast<size_type>(qtext.length()));
        std::memcpy(text, src, copyLen * sizeof(char16_t));
        if (copyLen < *len) text[copyLen] = 0;
    }
    
    return api_true;
}

void ButtonContext::SetButtonText(control_handle h, const char16_type* text)
{
    QAbstractButton* btn = qobject_cast<QAbstractButton*>(widgetFromHandle(h));
    if (btn && text) {
        QString qtext = QString::fromUtf16(text);
        btn->setText(qtext);
        logf("[Mock] SetButtonText: %s", qtext.toUtf8().constData());
    }
}

bitmap_handle ButtonContext::GetButtonIcon(const_control_handle h)
{
    QAbstractButton* btn = qobject_cast<QAbstractButton*>(widgetFromHandle(h));
    if (!btn) return nullptr;
    
    QIcon icon = btn->icon();
    if (!icon.isNull()) {
        return reinterpret_cast<bitmap_handle>(new QIcon(icon));
    }
    
    return nullptr;
}

void ButtonContext::SetButtonIcon(control_handle h, const_bitmap_handle icon)
{
    QAbstractButton* btn = qobject_cast<QAbstractButton*>(widgetFromHandle(h));
    if (!btn) return;
    
    if (icon && false) {
        QIcon* qicon = reinterpret_cast<QIcon*>(const_cast<void*>(icon));
        btn->setIcon(*qicon);
    } else {
        btn->setIcon(QIcon());
    }
    
    logf("[Mock] SetButtonIcon");
}

void ButtonContext::GetButtonIconSize(const_control_handle h, int32* w, int32* h_out)
{
    QAbstractButton* btn = qobject_cast<QAbstractButton*>(widgetFromHandle(h));
    if (!btn) return;
    
    QSize size = btn->iconSize();
    if (w) *w = size.width();
    if (h_out) *h_out = size.height();
}

void ButtonContext::SetButtonIconSize(control_handle h, int32 w, int32 h_size)
{
    QAbstractButton* btn = qobject_cast<QAbstractButton*>(widgetFromHandle(h));
    if (btn) {
        btn->setIconSize(QSize(w, h_size));
        logf("[Mock] SetButtonIconSize: %d x %d", w, h_size);
    }
}

api_bool ButtonContext::GetButtonPushed(const_control_handle h)
{
    QAbstractButton* btn = qobject_cast<QAbstractButton*>(widgetFromHandle(h));
    return (btn && btn->isDown()) ? api_true : api_false;
}

void ButtonContext::SetButtonPushed(control_handle h, api_bool pushed)
{
    QAbstractButton* btn = qobject_cast<QAbstractButton*>(widgetFromHandle(h));
    if (btn) {
        btn->setDown(pushed);
        logf("[Mock] SetButtonPushed: %d", pushed);
    }
}

uint32 ButtonContext::GetButtonChecked(const_control_handle h)
{
    // Try QCheckBox first (supports tristate)
    if (QCheckBox* checkbox = qobject_cast<QCheckBox*>(widgetFromHandle(h))) {
        Qt::CheckState state = checkbox->checkState();
        return static_cast<uint32>(state); // 0=unchecked, 1=partial, 2=checked
    }
    
    // Fallback to generic QAbstractButton (binary checked state)
    if (QAbstractButton* btn = qobject_cast<QAbstractButton*>(widgetFromHandle(h))) {
        if (btn->isCheckable()) {
            return btn->isChecked() ? 2 : 0;
        }
    }
    
    return 0;
}

void ButtonContext::SetButtonChecked(control_handle h, uint32 state)
{
    // Try QCheckBox first (supports tristate)
    if (QCheckBox* checkbox = qobject_cast<QCheckBox*>(widgetFromHandle(h))) {
        checkbox->setCheckState(static_cast<Qt::CheckState>(state));
        logf("[Mock] SetButtonChecked (QCheckBox): %u", state);
        return;
    }
    
    // Fallback to generic QAbstractButton (binary checked state)
    if (QAbstractButton* btn = qobject_cast<QAbstractButton*>(widgetFromHandle(h))) {
        btn->setChecked(state != 0);
        logf("[Mock] SetButtonChecked (QAbstractButton): %u", state);
    }
}

api_bool ButtonContext::GetButtonDefaultEnabled(const_control_handle h)
{
    QPushButton* btn = qobject_cast<QPushButton*>(widgetFromHandle(h));
    return (btn && btn->isDefault()) ? api_true : api_false;
}

void ButtonContext::SetButtonDefaultEnabled(control_handle h, api_bool enabled)
{
    QPushButton* btn = qobject_cast<QPushButton*>(widgetFromHandle(h));
    if (btn) {
        btn->setDefault(enabled);
        logf("[Mock] SetButtonDefaultEnabled: %d", enabled);
    }
}

api_bool ButtonContext::GetButtonTristateEnabled(const_control_handle h)
{
    QCheckBox* btn = qobject_cast<QCheckBox*>(widgetFromHandle(h));
    return (btn && btn->isTristate()) ? api_true : api_false;
}

void ButtonContext::SetButtonTristateEnabled(control_handle h, api_bool enabled)
{
    QCheckBox* btn = qobject_cast<QCheckBox*>(widgetFromHandle(h));
    if (btn) {
        btn->setTristate(enabled);
        logf("[Mock] SetButtonTristateEnabled: %d", enabled);
    }
}

api_bool ButtonContext::GetToolButtonCheckable(const_control_handle h)
{
    QToolButton* btn = qobject_cast<QToolButton*>(widgetFromHandle(h));
    return (btn && btn->isCheckable()) ? api_true : api_false;
}

void ButtonContext::SetToolButtonCheckable(control_handle h, api_bool checkable)
{
    QToolButton* btn = qobject_cast<QToolButton*>(widgetFromHandle(h));
    if (btn) {
        btn->setCheckable(checkable);
        logf("[Mock] SetToolButtonCheckable: %d", checkable);
    }
}

api_bool ButtonContext::SetButtonPressEventRoutine(control_handle h,
                                               api_handle receiver,
                                               pcl::event_routine routine)
{
    logf("[Mock] SetButtonPressEventRoutine");
    return api_true;
}

api_bool ButtonContext::SetButtonReleaseEventRoutine(control_handle h,
                                                 api_handle receiver, 
                                                 pcl::event_routine routine)
{
    logf("[Mock] SetButtonReleaseEventRoutine");
    return api_true;
}

api_bool ButtonContext::SetButtonClickEventRoutine(
    control_handle h,
    api_handle receiver,
    pcl::button_click_event_routine r)
{
    MockBase* b = get(h);
    if (!b) return api_false;

    b->pcl_handle = reinterpret_cast<control_handle>(receiver);
    b->eventReceiver = reinterpret_cast<control_handle>(receiver);
    b->onButtonClick = r;
    /*    
    if (QAbstractButton* btn = qobject_cast<QAbstractButton*>(widgetFromHandle(h))) {
        QObject::connect(btn, &QAbstractButton::clicked, [b](bool checked){
            if (b->onButtonClick && b->pcl_handle) {
                b->onButtonClick(
                    b->pcl_handle,
                    b->eventReceiver,
                    checked ? api_true : api_false);
            }
        });
    }
    */
    return api_true;
}

api_bool ButtonContext::SetButtonCheckEventRoutine(
    control_handle h,
    api_handle /*client*/,
    pcl::button_check_event_routine r)
{
    MockBase* b = get(h);
    if (!b) return api_false;
    
    b->onButtonCheck = r;
    
    if (QCheckBox* cb = qobject_cast<QCheckBox*>(widgetFromHandle(h))) {
        QObject::connect(cb, &QCheckBox::stateChanged, [b](int st){
            if (b->onButtonCheck && b->pcl_handle) {
                b->onButtonCheck(
                    b->pcl_handle,
                    b->pcl_handle,
                    st);
            }
        });
    }
    
    return api_true;
}

// =============================================================
// ComboBox Context
// =============================================================

control_handle ComboBoxContext::CreateComboBox(
    api_handle m, control_handle c, control_handle parent, uint32 flags)
{
    return createControl<QComboBox>(m, c, parent);
}

void ComboBoxContext::SetComboBoxEditEnabled(control_handle h, api_bool editable)
{
    QComboBox* combo = qobject_cast<QComboBox*>(widgetFromHandle(h));
    if (combo)
        combo->setEditable(editable);
}

int32 ComboBoxContext::GetComboBoxLength(const_control_handle h)
{
    QComboBox* combo = qobject_cast<QComboBox*>(widgetFromHandle(h));
    return combo ? combo->count() : 0;
}

void ComboBoxContext::InsertComboBoxItem(control_handle h, int32 idx, const char16_type* text, const_bitmap_handle icon)
{
    QComboBox* combo = qobject_cast<QComboBox*>(widgetFromHandle(h));
    if (!combo || !text) return;
    
    QString qtext = QString::fromUtf16(text);
    
    if (idx < 0 || idx >= combo->count())
        combo->addItem(qtext);
    else
        combo->insertItem(idx, qtext);
    
    logf("[Mock] InsertComboBoxItem: %s", qtext.toUtf8().constData());
}

void ComboBoxContext::SetComboBoxCurrentItem(control_handle h, int32 idx)
{
    QComboBox* combo = qobject_cast<QComboBox*>(widgetFromHandle(h));
    if (combo)
        combo->setCurrentIndex(idx);
}

api_bool ComboBoxContext::SetComboBoxItemSelectedEventRoutine(control_handle, api_handle, pcl::value_event_routine)
{
    return api_true;
}

// =============================================================
// ScrollBox Context
// =============================================================

control_handle ScrollBoxContext::CreateScrollBox(api_handle module,
                                              control_handle client,
                                              control_handle parent,
                                              uint32 /*flags*/)
{
    control_handle h = createControl<QScrollArea>(module, client, parent);
    
    QScrollArea* scrollArea = qobject_cast<QScrollArea*>(widgetFromHandle(h));
    if (scrollArea) {
        scrollArea->setWidgetResizable(true);
    }
    
    logf("[Mock] CreateScrollBox handle=%p", h);
    return h;
}

control_handle ScrollBoxContext::CreateScrollBoxViewport(control_handle scrollBox,
                                                      control_handle client)
{
    QScrollArea* scrollArea = qobject_cast<QScrollArea*>(widgetFromHandle(scrollBox));
    if (!scrollArea) return nullptr;
    
    MockBase* sb = get(scrollBox);
    if (!sb) return nullptr;
    
    // Create a viewport widget
    auto* b = new MockBase();
    b->moduleHandle = sb->moduleHandle;
    b->isSizer = false;
    b->isTreeNode = false;
    b->widget = new QWidget(scrollArea);
    b->layout = nullptr;
    b->treeItem = nullptr;
    
    // Register using client as key
    g_objects[client] = std::unique_ptr<MockBase>(b);
    
    // Attach to scroll area
    scrollArea->setWidget(b->widget);
    
    logf("[Mock] CreateScrollBoxViewport handle=%p parentScroll=%p widget=%p",
          client, scrollBox, b->widget);
    
    return client;
}

// =============================================================
// TreeBox Context - CORRECTED
// =============================================================

control_handle TreeBoxContext::CreateTreeBox(api_handle module,
                                         control_handle client,
                                         control_handle parent,
                                         uint32 flags)
{
    control_handle h = createControl<QTreeWidget>(module, client, parent);
    
    QTreeWidget* tree = qobject_cast<QTreeWidget*>(widgetFromHandle(h));
    if (tree) {
        tree->setColumnCount(1);
        tree->setHeaderHidden(false);
    }
    
    logf("[Mock] CreateTreeBox handle=%p", h);
    return h;
}

control_handle TreeBoxContext::CreateTreeBoxViewport(control_handle h,
                                                 control_handle client)
{
    QTreeWidget* tree = treeFromHandle(h);
    if (!tree) return nullptr;
    
    QWidget* viewport = tree->viewport();
    
    logf("[Mock] CreateTreeBoxViewport");
    return reinterpret_cast<control_handle>(viewport);
}

api_handle TreeBoxContext::CreateTreeBoxNode(api_handle module,
                                         treebox_handle nodeClient)
{
    // Create mock wrapper
    auto* mock = new MockBase();
    mock->moduleHandle = module;
    mock->isSizer = false;
    mock->isTreeNode = true;  // Mark as tree node
    mock->widget = nullptr;
    mock->layout = nullptr;
    mock->treeItem = new QTreeWidgetItem();
    mock->treeItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    
    // CRITICAL: Use CLIENT as key (consistent with controls)
    g_objects[nodeClient] = std::unique_ptr<MockBase>(mock);
    
    logf("[Mock] CreateTreeBoxNode client=%p item=%p", nodeClient, mock->treeItem);
    
    return nodeClient;
}

void TreeBoxContext::ClearTreeBox(control_handle h)
{
    QTreeWidget* tree = treeFromHandle(h);
    if (tree) {
        tree->clear();
        logf("[Mock] ClearTreeBox");
    }
}

int32 TreeBoxContext::GetTreeBoxChildCount(const_control_handle h)
{
    QTreeWidget* tree = treeFromHandle(h);
    return tree ? tree->topLevelItemCount() : 0;
}

int32 TreeBoxContext::GetTreeBoxChildIndex(const_control_handle h,
                                       const_api_handle node)
{
    QTreeWidget* tree = treeFromHandle(h);
    if (!tree || !node) return -1;
    
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (!item) return -1;
    
    return tree->indexOfTopLevelItem(item);
}

api_handle TreeBoxContext::GetTreeBoxChild(const_control_handle h, int32 idx)
{
    QTreeWidget* tree = treeFromHandle(h);
    if (!tree) return nullptr;
    
    QTreeWidgetItem* item = tree->topLevelItem(idx);
    if (!item) return nullptr;
    
    for (auto& kv : g_objects) {
        MockBase* obj = kv.second.get();
        if (obj->isTreeNode && obj->treeItem == item) {
            return const_cast<api_handle>(kv.first);
        }
    }
    
    return nullptr;
}

void TreeBoxContext::InsertTreeBoxNode(control_handle h,
                                   int32 idx,
                                   api_handle node)
{
    QTreeWidget* tree = treeFromHandle(h);
    if (!tree || !node) return;
    
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (!item) return;
    
    if (idx < 0 || idx >= tree->topLevelItemCount()) {
        tree->addTopLevelItem(item);
    } else {
        tree->insertTopLevelItem(idx, item);
    }
    
    logf("[Mock] InsertTreeBoxNode idx=%d", idx);
}

void TreeBoxContext::RemoveTreeBoxNode(control_handle h, int32 idx)
{
    QTreeWidget* tree = treeFromHandle(h);
    if (!tree) return;
    
    QTreeWidgetItem* item = tree->takeTopLevelItem(idx);
    if (item) {
        logf("[Mock] RemoveTreeBoxNode idx=%d", idx);
    }
}

api_handle TreeBoxContext::GetTreeBoxCurrentNode(const_control_handle h)
{
    QTreeWidget* tree = treeFromHandle(h);
    if (!tree) return nullptr;
    
    QTreeWidgetItem* item = tree->currentItem();
    if (!item) return nullptr;
    
    // Find the handle for this item
    for (auto& kv : g_objects) {
        MockBase* obj = kv.second.get();
        if (obj->isTreeNode && obj->treeItem == item) {
            return const_cast<void *>(kv.first);
        }
    }
    
    return nullptr;
}

void TreeBoxContext::SetTreeBoxCurrentNode(control_handle h, api_handle node)
{
    QTreeWidget* tree = treeFromHandle(h);
    if (!tree) return;
    
    if (!node) {
        tree->setCurrentItem(nullptr);
        return;
    }
    
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (item) {
        tree->setCurrentItem(item);
        logf("[Mock] SetTreeBoxCurrentNode");
    }
}

void TreeBoxContext::SelectAllTreeBoxNodes(control_handle h)
{
    QTreeWidget* tree = treeFromHandle(h);
    if (tree) {
        tree->selectAll();
        logf("[Mock] SelectAllTreeBoxNodes");
    }
}

void TreeBoxContext::SetTreeBoxMultipleNodeSelectionEnabled(control_handle h,
                                                        api_bool enabled)
{
    QTreeWidget* tree = treeFromHandle(h);
    if (tree) {
        tree->setSelectionMode(enabled ?
            QAbstractItemView::ExtendedSelection :
            QAbstractItemView::SingleSelection);
        logf("[Mock] SetTreeBoxMultipleNodeSelectionEnabled: %d", enabled);
    }
}

void TreeBoxContext::SetTreeBoxColumnCount(control_handle h, int32 count)
{
    QTreeWidget* tree = treeFromHandle(h);
    if (tree) {
        tree->setColumnCount(count);
        logf("[Mock] SetTreeBoxColumnCount: %d", count);
    }
}

void TreeBoxContext::AdjustTreeBoxColumnWidthToContents(control_handle h, int32 col)
{
    QTreeWidget* tree = treeFromHandle(h);
    if (tree) {
        tree->resizeColumnToContents(col);
        logf("[Mock] AdjustTreeBoxColumnWidthToContents col=%d", col);
    }
}

void TreeBoxContext::SetTreeBoxHeaderVisible(control_handle h, api_bool visible)
{
    QTreeWidget* tree = treeFromHandle(h);
    if (tree) {
        tree->setHeaderHidden(!visible);
        logf("[Mock] SetTreeBoxHeaderVisible: %d", visible);
    }
}

void TreeBoxContext::SetTreeBoxRootDecorationEnabled(control_handle h,
                                                 api_bool enabled)
{
    QTreeWidget* tree = treeFromHandle(h);
    if (tree) {
        tree->setRootIsDecorated(enabled);
        logf("[Mock] SetTreeBoxRootDecorationEnabled: %d", enabled);
    }
}

void TreeBoxContext::SetTreeBoxAlternateRowColorEnabled(control_handle h,
                                                    api_bool enabled)
{
    QTreeWidget* tree = treeFromHandle(h);
    if (tree) {
        tree->setAlternatingRowColors(enabled);
        logf("[Mock] SetTreeBoxAlternateRowColorEnabled: %d", enabled);
    }
}

// TreeBox Node Functions
control_handle TreeBoxContext::GetTreeBoxNodeParentBox(const_api_handle node)
{
    if (!node) return nullptr;
    
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (!item) return nullptr;
    
    QTreeWidget* tree = item->treeWidget();
    if (!tree) return nullptr;
    
    // Find the control handle for this tree widget
    for (auto& kv : g_objects) {
        MockBase* obj = kv.second.get();
        if (obj->widget == tree) {
            return reinterpret_cast<control_handle>(const_cast<void*>(kv.first));  // FIX
        }
    }
    
    return nullptr;
}

api_handle TreeBoxContext::GetTreeBoxNodeParent(const_api_handle node)
{
    if (!node) return nullptr;
    
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (!item) return nullptr;
    
    QTreeWidgetItem* parent = item->parent();
    if (!parent) return nullptr;
    
    // Find the handle for the parent item
    for (auto& kv : g_objects) {
        MockBase* obj = kv.second.get();
        if (obj->isTreeNode && obj->treeItem == parent) {
            return const_cast<api_handle>(kv.first);
        }
    }
    
    return nullptr;
}

int32 TreeBoxContext::GetTreeBoxNodeChildCount(const_api_handle node)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    return item ? item->childCount() : 0;
}

api_handle TreeBoxContext::GetTreeBoxNodeChild(const_api_handle node, int32 idx)
{
    if (!node) return nullptr;
    
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (!item) return nullptr;
    
    QTreeWidgetItem* child = item->child(idx);
    if (!child) return nullptr;
    
    // Find the handle for the child item
    for (auto& kv : g_objects) {
        MockBase* obj = kv.second.get();
        if (obj->isTreeNode && obj->treeItem == child) {
            return const_cast<api_handle>(kv.first);
        }
    }
    
    return nullptr;
}

void TreeBoxContext::InsertTreeBoxNodeChild(api_handle parentNode,
                                        int32 idx,
                                        api_handle childNode)
{
    if (!parentNode || !childNode) return;
    
    QTreeWidgetItem* parent = treeItemFromHandle(parentNode);
    QTreeWidgetItem* child = treeItemFromHandle(childNode);
    
    if (!parent || !child) return;
    
    if (idx < 0 || idx >= parent->childCount()) {
        parent->addChild(child);
    } else {
        parent->insertChild(idx, child);
    }
    
    logf("[Mock] InsertTreeBoxNodeChild idx=%d", idx);
}

void TreeBoxContext::RemoveTreeBoxNodeChild(api_handle parentNode, int32 idx)
{
    if (!parentNode) return;
    
    QTreeWidgetItem* parent = treeItemFromHandle(parentNode);
    if (!parent) return;
    
    QTreeWidgetItem* child = parent->takeChild(idx);
    if (child) {
        logf("[Mock] RemoveTreeBoxNodeChild idx=%d", idx);
    }
}

// TreeBox Node State Functions

api_bool TreeBoxContext::GetTreeBoxNodeEnabled(const_api_handle node)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    return (item && (item->flags() & Qt::ItemIsEnabled)) ? api_true : api_false;
}

void TreeBoxContext::SetTreeBoxNodeEnabled(api_handle node, api_bool enabled)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (!item) return;
    
    Qt::ItemFlags flags = item->flags();
    
    if (enabled)
        flags |= Qt::ItemIsEnabled;
    else
        flags &= ~Qt::ItemIsEnabled;
    
    item->setFlags(flags);
    logf("[Mock] SetTreeBoxNodeEnabled: %d", enabled);
}

api_bool TreeBoxContext::GetTreeBoxNodeExpanded(const_api_handle node)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    return (item && item->isExpanded()) ? api_true : api_false;
}

void TreeBoxContext::SetTreeBoxNodeExpanded(api_handle node, api_bool expanded)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (item) {
        item->setExpanded(expanded);
        logf("[Mock] SetTreeBoxNodeExpanded: %d", expanded);
    }
}

api_bool TreeBoxContext::GetTreeBoxNodeSelectable(const_api_handle node)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    return (item && (item->flags() & Qt::ItemIsSelectable)) ? api_true : api_false;
}

void TreeBoxContext::SetTreeBoxNodeSelectable(api_handle node, api_bool selectable)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (!item) return;
    
    Qt::ItemFlags flags = item->flags();
    
    if (selectable)
        flags |= Qt::ItemIsSelectable;
    else
        flags &= ~Qt::ItemIsSelectable;
    
    item->setFlags(flags);
    logf("[Mock] SetTreeBoxNodeSelectable: %d", selectable);
}

api_bool TreeBoxContext::GetTreeBoxNodeSelected(const_api_handle node)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    return (item && item->isSelected()) ? api_true : api_false;
}

void TreeBoxContext::SetTreeBoxNodeSelected(api_handle node, api_bool selected)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (item) {
        item->setSelected(selected);
        logf("[Mock] SetTreeBoxNodeSelected: %d", selected);
    }
}

api_bool TreeBoxContext::GetTreeBoxNodeCheckable(const_api_handle node)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    return (item && (item->flags() & Qt::ItemIsUserCheckable)) ? api_true : api_false;
}

void TreeBoxContext::SetTreeBoxNodeCheckable(api_handle node, api_bool checkable)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (!item) return;
    
    Qt::ItemFlags flags = item->flags();
    
    if (checkable) {
        flags |= Qt::ItemIsUserCheckable;
        item->setCheckState(0, Qt::Unchecked);
    } else {
        flags &= ~Qt::ItemIsUserCheckable;
    }
    
    item->setFlags(flags);
    logf("[Mock] SetTreeBoxNodeCheckable: %d", checkable);
}

api_bool TreeBoxContext::GetTreeBoxNodeChecked(const_api_handle node)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    return (item && item->checkState(0) == Qt::Checked) ? api_true : api_false;
}

void TreeBoxContext::SetTreeBoxNodeChecked(api_handle node, api_bool checked)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (item) {
        item->setCheckState(0, checked ? Qt::Checked : Qt::Unchecked);
        logf("[Mock] SetTreeBoxNodeChecked: %d", checked);
    }
}

api_bool TreeBoxContext::GetTreeBoxNodeEditable(const_api_handle node)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    return (item && (item->flags() & Qt::ItemIsEditable)) ? api_true : api_false;
}

void TreeBoxContext::SetTreeBoxNodeEditable(api_handle node, api_bool editable)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (!item) return;
    
    Qt::ItemFlags flags = item->flags();
    
    if (editable)
        flags |= Qt::ItemIsEditable;
    else
        flags &= ~Qt::ItemIsEditable;
    
    item->setFlags(flags);
    logf("[Mock] SetTreeBoxNodeEditable: %d", editable);
}

api_bool TreeBoxContext::GetTreeBoxNodeFirstColumnSpanned(const_api_handle node)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (!item) return api_false;
    
    return item->isFirstColumnSpanned() ? api_true : api_false;
}

void TreeBoxContext::SetTreeBoxNodeFirstColumnSpanned(api_handle node, api_bool spanned)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (!item) return;
    
    item->setFirstColumnSpanned(spanned);
    logf("[Mock] SetTreeBoxNodeFirstColumnSpanned: %d", spanned);
}

// TreeBox Node Column Content Functions

api_bool TreeBoxContext::GetTreeBoxNodeColText(const_api_handle node,
                                           int32 col,
                                           char16_type* text,
                                           size_type* len)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (!item) return api_false;
    
    QString qtext = item->text(col);
    
    if (len) *len = qtext.length();
    
    if (text && len && *len > 0) {
        const char16_type* src = reinterpret_cast<const char16_type*>(qtext.utf16());
        size_t copyLen = std::min(*len, static_cast<size_type>(qtext.length()));
        std::memcpy(text, src, copyLen * sizeof(char16_t));
        if (copyLen < *len) text[copyLen] = 0;
    }
    
    return api_true;
}

void TreeBoxContext::SetTreeBoxNodeColText(api_handle node,
                                       int32 col,
                                       const char16_type* text)
{
    if (!node || !text) return;
    
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (!item) return;
    
    QString qtext = QString::fromUtf16(text);
    item->setText(col, qtext);
    logf("[Mock] SetTreeBoxNodeColText col=%d: %s", col, qtext.toUtf8().constData());
}

bitmap_handle TreeBoxContext::GetTreeBoxNodeColIcon(const_api_handle node, int32 col)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (!item) return nullptr;
    
    QIcon icon = item->icon(col);
    
    if (!icon.isNull()) {
        QPixmap pixmap = icon.pixmap(32, 32);
        return reinterpret_cast<bitmap_handle>(new QPixmap(pixmap));
    }
    
    return nullptr;
}

void TreeBoxContext::SetTreeBoxNodeColIcon(api_handle node,
                                       int32 col,
                                       const_bitmap_handle icon)
{
    if (!node) return;
    
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (!item) return;
    
    if (icon) {
        const QPixmap* pixmap = reinterpret_cast<const QPixmap*>(icon);
        item->setIcon(col, QIcon(*pixmap));
    } else {
        item->setIcon(col, QIcon());
    }
    
    logf("[Mock] SetTreeBoxNodeColIcon col=%d", col);
}

int32 TreeBoxContext::GetTreeBoxNodeColAlignment(const_api_handle node, int32 col)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    return item ? static_cast<int32>(item->textAlignment(col)) : 0;
}

void TreeBoxContext::SetTreeBoxNodeColAlignment(api_handle node,
                                            int32 col,
                                            int32 alignment)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (item) {
        item->setTextAlignment(col, alignment);
        logf("[Mock] SetTreeBoxNodeColAlignment col=%d align=%d", col, alignment);
    }
}

api_bool TreeBoxContext::GetTreeBoxNodeColToolTip(const_api_handle node,
                                              int32 col,
                                              char16_type* text,
                                              size_type* len)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (!item) return api_false;
    
    QString qtext = item->toolTip(col);
    
    if (len) *len = qtext.length();
    
    if (text && len && *len > 0) {
        const char16_type* src = reinterpret_cast<const char16_type*>(qtext.utf16());
        size_t copyLen = std::min(*len, static_cast<size_type>(qtext.length()));
        std::memcpy(text, src, copyLen * sizeof(char16_t));
        if (copyLen < *len) text[copyLen] = 0;
    }
    
    return api_true;
}

void TreeBoxContext::SetTreeBoxNodeColToolTip(api_handle node,
                                          int32 col,
                                          const char16_type* text)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (!item) return;
    
    QString qtext = text ? QString::fromUtf16(text) : QString();
    item->setToolTip(col, qtext);
    logf("[Mock] SetTreeBoxNodeColToolTip col=%d", col);
}

font_handle TreeBoxContext::GetTreeBoxNodeColFont(const_api_handle node, int32 col)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (!item) return nullptr;
    
    QFont font = item->font(col);
    return reinterpret_cast<font_handle>(new QFont(font));
}

void TreeBoxContext::SetTreeBoxNodeColFont(api_handle node,
                                       int32 col,
                                       const_font_handle font)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (!item) return;
    
    if (font) {
        const QFont* qfont = reinterpret_cast<const QFont*>(font);
        item->setFont(col, *qfont);
    }
    
    logf("[Mock] SetTreeBoxNodeColFont col=%d", col);
}

uint32 TreeBoxContext::GetTreeBoxNodeColBackgroundColor(const_api_handle node, int32 col)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (!item) return 0;
    
    QBrush brush = item->background(col);
    return brush.color().rgba();
}

void TreeBoxContext::SetTreeBoxNodeColBackgroundColor(api_handle node,
                                                  int32 col,
                                                  uint32 rgba)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (!item) return;
    
    item->setBackground(col, QBrush(QColor(rgba)));
    logf("[Mock] SetTreeBoxNodeColBackgroundColor col=%d rgba=0x%08x", col, rgba);
}

uint32 TreeBoxContext::GetTreeBoxNodeColTextColor(const_api_handle node, int32 col)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (!item) return 0;
    
    QBrush brush = item->foreground(col);
    return brush.color().rgba();
}

void TreeBoxContext::SetTreeBoxNodeColTextColor(api_handle node,
                                            int32 col,
                                            uint32 rgba)
{
    QTreeWidgetItem* item = treeItemFromHandle(node);
    if (!item) return;
    
    item->setForeground(col, QBrush(QColor(rgba)));
    logf("[Mock] SetTreeBoxNodeColTextColor col=%d rgba=0x%08x", col, rgba);
}

// TreeBox Event Handlers

api_bool TreeBoxContext::SetTreeBoxNodeActivatedEventRoutine(
    control_handle h,
    api_handle /*client*/,
    pcl::item_value_event_routine r)
{
    MockBase* b = get(h);
    if (!b) return api_false;
    
    b->onTreeNodeActivated = r;
    
    if (QTreeWidget* tree = treeFromHandle(h)) {
        QObject::connect(tree, &QTreeWidget::itemActivated,
                         [b](QTreeWidgetItem* item, int col){
            if (b->onTreeNodeActivated && b->pcl_handle) {
                // Find the node handle for this item
                for (auto& kv : g_objects) {
                    MockBase* obj = kv.second.get();
                    if (obj->isTreeNode && obj->treeItem == item) {
                        b->onTreeNodeActivated(
                            b->pcl_handle,
                            b->pcl_handle,
                            const_cast<api_handle>(kv.first),
                            col);
                        break;
                    }
                }
            }
        });
    }
    
    return api_true;
}

api_bool TreeBoxContext::SetTreeBoxCurrentNodeUpdatedEventRoutine(
    control_handle h,
    api_handle /*client*/,
    pcl::item_range_event_routine r)
{
    MockBase* b = get(h);
    if (!b) return api_false;
    
    b->onTreeNodeUpdated = r;
    
    if (QTreeWidget* tree = treeFromHandle(h)) {
        QObject::connect(tree, &QTreeWidget::currentItemChanged,
                         [b](QTreeWidgetItem* item, QTreeWidgetItem*){
            if (b->onTreeNodeUpdated && b->pcl_handle && item) {
                // Find the node handle for this item
                for (auto& kv : g_objects) {
                    MockBase* obj = kv.second.get();
                    if (obj->isTreeNode && obj->treeItem == item) {
                        b->onTreeNodeUpdated(
                            b->pcl_handle,
                            b->pcl_handle,
                            b->pcl_handle,
                            const_cast<api_handle>(kv.first));
                        break;
                    }
                }
            }
        });
    }
    
    return api_true;
}

api_bool TreeBoxContext::SetTreeBoxNodeSelectionUpdatedEventRoutine(
    control_handle h,
    api_handle /*client*/,
    pcl::event_routine r)
{
    MockBase* b = get(h);
    if (!b) return api_false;
    
    b->onTreeSelectionUpdated = r;
    
    if (QTreeWidget* tree = treeFromHandle(h)) {
        QObject::connect(tree, &QTreeWidget::itemSelectionChanged,
                         [b, tree](){
            if (b->onTreeSelectionUpdated && b->pcl_handle) {
                auto items = tree->selectedItems();
                QTreeWidgetItem* first = items.isEmpty() ? nullptr : items.first();
                
                control_handle firstHandle = nullptr;
                if (first) {
                    for (auto& kv : g_objects) {
                        MockBase* obj = kv.second.get();
                        if (obj->isTreeNode && obj->treeItem == first) {
                            firstHandle = reinterpret_cast<control_handle>(const_cast<void*>(kv.first));  // FIX: reinterpret_cast
                            break;
                        }
                    }
                }
                
                b->onTreeSelectionUpdated(b->pcl_handle, firstHandle);
            }
        });
    }
    
    return api_true;
}

typedef meta_format_handle const_meta_format_handle;
  
api_bool TreeBoxContext::SetTreeBoxNodeEnteredEventRoutine(
    control_handle h,
    api_handle,
    pcl::item_value_event_routine routine)
{
    logf("[Mock] SetTreeBoxNodeEnteredEventRoutine");
    return api_true;
}

api_bool TreeBoxContext::SetTreeBoxNodeClickedEventRoutine(
    control_handle h,
    api_handle,
    pcl::item_value_event_routine routine)
{
    logf("[Mock] SetTreeBoxNodeClickedEventRoutine");
    return api_true;
}

api_bool TreeBoxContext::SetTreeBoxNodeDoubleClickedEventRoutine(
    control_handle h,
    api_handle,
    pcl::item_value_event_routine routine)
{
    logf("[Mock] SetTreeBoxNodeDoubleClickedEventRoutine");
    return api_true;
}

api_bool TreeBoxContext::SetTreeBoxNodeExpandedEventRoutine(
    control_handle h,
    api_handle,
    pcl::item_event_routine routine)
{
    logf("[Mock] SetTreeBoxNodeExpandedEventRoutine");
    return api_true;
}

api_bool TreeBoxContext::SetTreeBoxNodeCollapsedEventRoutine(
    control_handle h,
    api_handle,
    pcl::item_event_routine routine)
{
    logf("[Mock] SetTreeBoxNodeCollapsedEventRoutine");
    return api_true;
}

// =============================================================
// Continue with remaining contexts...
// (BitmapBox, Bitmap, UI, Font, Global, etc.)
// These can be added as needed based on what your module uses
// =============================================================
// =============================================================
// BitmapBox Context
// =============================================================

control_handle BitmapBoxContext::CreateBitmapBox(api_handle module,
                                             control_handle client,
                                             const_bitmap_handle bitmap,
                                             control_handle parent,
                                             uint32 flags)
{
    control_handle h = createControl<QLabel>(module, client, parent);
    
    QLabel* label = qobject_cast<QLabel*>(widgetFromHandle(h));
    if (label && bitmap) {
        QPixmap* pixmap = reinterpret_cast<QPixmap*>(const_cast<void*>(bitmap));
        label->setPixmap(*pixmap);
        label->setScaledContents(false);
    }
    
    logf("[Mock] CreateBitmapBox handle=%p", h);
    return h;
}

bitmap_handle BitmapBoxContext::GetBitmapBoxBitmap(const_control_handle h)
{
    QLabel* label = qobject_cast<QLabel*>(widgetFromHandle(h));
    if (!label) return nullptr;
    
    QPixmap pm = label->pixmap(Qt::ReturnByValue);  // FIX: Use new Qt API
    if (pm.isNull())
        return nullptr;
    
    return reinterpret_cast<bitmap_handle>(new QPixmap(pm));
}

void BitmapBoxContext::SetBitmapBoxBitmap(control_handle h,
                                      const_bitmap_handle bitmap)
{
    QLabel* label = qobject_cast<QLabel*>(widgetFromHandle(h));
    if (!label) return;
    
    if (bitmap) {
        const QPixmap* pixmap = reinterpret_cast<const QPixmap*>(bitmap);
        label->setPixmap(*pixmap);
    } else {
        label->clear();
    }
    logf("[Mock] SetBitmapBoxBitmap");
}

int32 BitmapBoxContext::GetBitmapBoxMargin(const_control_handle h)
{
    QLabel* label = qobject_cast<QLabel*>(widgetFromHandle(h));
    return label ? label->margin() : 0;
}

void BitmapBoxContext::SetBitmapBoxMargin(control_handle h, int32 margin)
{
    QLabel* label = qobject_cast<QLabel*>(widgetFromHandle(h));
    if (label) {
        label->setMargin(margin);
        logf("[Mock] SetBitmapBoxMargin: %d", margin);
    }
}

api_bool BitmapBoxContext::GetBitmapBoxAutoFitEnabled(const_control_handle h)
{
    QLabel* label = qobject_cast<QLabel*>(widgetFromHandle(h));
    return (label && label->hasScaledContents()) ? api_true : api_false;
}

void BitmapBoxContext::SetBitmapBoxAutoFitEnabled(control_handle h, api_bool enabled)
{
    QLabel* label = qobject_cast<QLabel*>(widgetFromHandle(h));
    if (label) {
        label->setScaledContents(enabled);
        logf("[Mock] SetBitmapBoxAutoFitEnabled: %d", enabled);
    }
}

// =============================================================
// Bitmap Context
// =============================================================

bitmap_handle BitmapContext::CreateBitmap(api_handle module,
                                      int32 width,
                                      int32 height,
                                      void* data)
{
    QPixmap* pixmap = new QPixmap(width, height);
    
    if (data) {
        QImage img(static_cast<uchar*>(data), width, height,
                   QImage::Format_ARGB32);
        *pixmap = QPixmap::fromImage(img);
    } else {
        pixmap->fill(Qt::transparent);
    }
    
    logf("[Mock] CreateBitmap: %dx%d", width, height);
    return reinterpret_cast<bitmap_handle>(pixmap);
}

bitmap_handle BitmapContext::CreateBitmapXPM(api_handle module, const char** xpm)
{
    if (!xpm) return nullptr;
    
    QPixmap* pixmap = new QPixmap(xpm);
    
    logf("[Mock] CreateBitmapXPM");
    return reinterpret_cast<bitmap_handle>(pixmap);
}

bitmap_handle BitmapContext::CreateBitmapFromFile(api_handle module,
                                              const char16_type* filePath)
{
    if (!filePath) return nullptr;
    
    QString path = QString::fromUtf16(filePath);
    QPixmap* pixmap = new QPixmap(path);
    
    if (pixmap->isNull()) {
        delete pixmap;
        logf("[Mock] CreateBitmapFromFile FAILED: %s", path.toUtf8().constData());
        return BitmapContext::CreateBitmap(module, 100, 100, nullptr);
    }
    
    logf("[Mock] CreateBitmapFromFile: %s", path.toUtf8().constData());
    return reinterpret_cast<bitmap_handle>(pixmap);
}

bitmap_handle BitmapContext::CreateBitmapFromFile8(api_handle module,
                                               const char* filePath)
{
    if (!filePath) return nullptr;
    
    QPixmap* pixmap = new QPixmap(QString::fromUtf8(filePath));
    
    if (pixmap->isNull()) {
        delete pixmap;
        return nullptr;
    }
    
    logf("[Mock] CreateBitmapFromFile8: %s", filePath);
    return reinterpret_cast<bitmap_handle>(pixmap);
}

bitmap_handle BitmapContext::CreateBitmapFromData(api_handle module,
                                              const void* data,
                                              size_type size,
                                              const char* format,
                                              uint32 flags)
{
    if (!data || size == 0) return nullptr;
    
    QPixmap* pixmap = new QPixmap();
    QByteArray bytes(static_cast<const char*>(data), size);
    
    if (!pixmap->loadFromData(bytes, format)) {
        delete pixmap;
        logf("[Mock] CreateBitmapFromData FAILED");
        return nullptr;
    }
    
    logf("[Mock] CreateBitmapFromData: format=%s size=%zu",
         format ? format : "auto", size);
    return reinterpret_cast<bitmap_handle>(pixmap);
}

bitmap_handle BitmapContext::CreateEmptyBitmap(api_handle module)
{
    QPixmap* pixmap = new QPixmap();
    
    logf("[Mock] CreateEmptyBitmap");
    return reinterpret_cast<bitmap_handle>(pixmap);
}

bitmap_handle BitmapContext::CloneBitmap(api_handle module,
                                     const_bitmap_handle source)
{
    if (!source) return nullptr;
    
    const QPixmap* src = reinterpret_cast<const QPixmap*>(source);
    QPixmap* clone = new QPixmap(*src);
    
    logf("[Mock] CloneBitmap");
    return reinterpret_cast<bitmap_handle>(clone);
}

bitmap_handle BitmapContext::CloneBitmapRect(api_handle module,
                                         const_bitmap_handle source,
                                         int32 x, int32 y,
                                         int32 w, int32 h)
{
    if (!source) return nullptr;
    
    const QPixmap* src = reinterpret_cast<const QPixmap*>(source);
    QPixmap* clone = new QPixmap(src->copy(x, y, w, h));
    
    logf("[Mock] CloneBitmapRect: %d,%d %dx%d", x, y, w, h);
    return reinterpret_cast<bitmap_handle>(clone);
}

bitmap_handle BitmapContext::CreateBitmapFromSVG(api_handle module,
                                             const char* svgSource,
                                             int32 width,
                                             int32 height,
                                             uint32 flags)
{
    if (!svgSource) return nullptr;
    
    QByteArray svgData(svgSource);
    QSvgRenderer renderer(svgData);
    
    if (!renderer.isValid()) {
        logf("[Mock] CreateBitmapFromSVG FAILED: invalid SVG");
        return nullptr;
    }
    
    QPixmap* pixmap = new QPixmap(width, height);
    pixmap->fill(Qt::transparent);
    
    QPainter painter(pixmap);
    renderer.render(&painter);
    
    logf("[Mock] CreateBitmapFromSVG: %dx%d", width, height);
    return reinterpret_cast<bitmap_handle>(pixmap);
}

bitmap_handle BitmapContext::CreateBitmapFromSVGFile(api_handle module,
                                                 const char16_type* filePath,
                                                 int32 width,
                                                 int32 height,
                                                 uint32 flags)
{
    if (!filePath) return nullptr;
    
    QString path = QString::fromUtf16(filePath);
    QSvgRenderer renderer(path);
    
    if (!renderer.isValid()) {
        logf("[Mock] CreateBitmapFromSVGFile FAILED: %s",
             path.toUtf8().constData());
        return nullptr;
    }
    
    QPixmap* pixmap = new QPixmap(width, height);
    pixmap->fill(Qt::transparent);
    
    QPainter painter(pixmap);
    renderer.render(&painter);
    
    logf("[Mock] CreateBitmapFromSVGFile: %s", path.toUtf8().constData());
    return reinterpret_cast<bitmap_handle>(pixmap);
}

int32 BitmapContext::GetBitmapFormat(bitmap_handle h)
{
    if (!h) return 0;
    
    const QPixmap* pixmap = reinterpret_cast<const QPixmap*>(h);
    return pixmap->depth();
}

void BitmapContext::SetBitmapFormat(bitmap_handle h, int32 format)
{
    logf("[Mock] SetBitmapFormat: %d (not implemented)", format);
}

unsigned int* BitmapContext::GetBitmapScanLine(bitmap_handle h, int32 y)
{
    if (!h) return nullptr;
    
    const QPixmap* pixmap = reinterpret_cast<const QPixmap*>(h);
    QImage img = pixmap->toImage();
    
    if (y < 0 || y >= img.height()) return nullptr;
    
    return reinterpret_cast<unsigned int*>(img.scanLine(y));
}

api_bool BitmapContext::GetBitmapDimensions(const_bitmap_handle h,
                                        int32* width,
                                        int32* height)
{
    if (!h) return api_false;
    
    const QPixmap* pixmap = reinterpret_cast<const QPixmap*>(h);
    
    if (width) *width = pixmap->width();
    if (height) *height = pixmap->height();
    
    return api_true;
}

api_bool BitmapContext::IsEmptyBitmap(const_bitmap_handle h)
{
    if (!h) return api_true;
    
    const QPixmap* pixmap = reinterpret_cast<const QPixmap*>(h);
    return pixmap->isNull() ? api_true : api_false;
}

uint32 BitmapContext::GetBitmapPixel(const_bitmap_handle h, int32 x, int32 y)
{
    if (!h) return 0;
    
    const QPixmap* pixmap = reinterpret_cast<const QPixmap*>(h);
    QImage img = pixmap->toImage();
    
    if (x < 0 || x >= img.width() || y < 0 || y >= img.height())
        return 0;
    
    return img.pixel(x, y);
}

void BitmapContext::SetBitmapPixel(bitmap_handle h, int32 x, int32 y, uint32 rgba)
{
    if (!h) return;
    
    QPixmap* pixmap = reinterpret_cast<QPixmap*>(h);
    QImage img = pixmap->toImage();
    
    if (x >= 0 && x < img.width() && y >= 0 && y < img.height()) {
        img.setPixel(x, y, rgba);
        *pixmap = QPixmap::fromImage(img);
    }
}

bitmap_handle BitmapContext::MirroredBitmap(const_bitmap_handle h,
                                        api_bool horizontal,
                                        api_bool vertical)
{
    if (!h) return nullptr;
    
    const QPixmap* src = reinterpret_cast<const QPixmap*>(h);
    QTransform transform;
    
    if (horizontal) transform.scale(-1, 1);
    if (vertical) transform.scale(1, -1);
    
    QPixmap* result = new QPixmap(src->transformed(transform));
    
    logf("[Mock] MirroredBitmap: h=%d v=%d", horizontal, vertical);
    return reinterpret_cast<bitmap_handle>(result);
}

bitmap_handle BitmapContext::ScaledBitmap(const_bitmap_handle h,
                                      int32 width,
                                      int32 height,
                                      api_bool smooth)
{
    if (!h) return nullptr;
    
    const QPixmap* src = reinterpret_cast<const QPixmap*>(h);
    Qt::TransformationMode mode = smooth ?
        Qt::SmoothTransformation : Qt::FastTransformation;
    
    QPixmap* result = new QPixmap(src->scaled(width, height,
                                              Qt::IgnoreAspectRatio, mode));
    
    logf("[Mock] ScaledBitmap: %dx%d smooth=%d", width, height, smooth);
    return reinterpret_cast<bitmap_handle>(result);
}

bitmap_handle BitmapContext::RotatedBitmap(const_bitmap_handle h,
                                       double angle,
                                       api_bool smooth)
{
    if (!h) return nullptr;
    
    const QPixmap* src = reinterpret_cast<const QPixmap*>(h);
    QTransform transform;
    transform.rotate(angle);
    
    Qt::TransformationMode mode = smooth ?
        Qt::SmoothTransformation : Qt::FastTransformation;
    
    QPixmap* result = new QPixmap(src->transformed(transform, mode));
    
    logf("[Mock] RotatedBitmap: angle=%.2f smooth=%d", angle, smooth);
    return reinterpret_cast<bitmap_handle>(result);
}

api_bool BitmapContext::LoadBitmap(bitmap_handle h, const char16_type* filePath)
{
    if (!h || !filePath) return api_false;
    
    QPixmap* pixmap = reinterpret_cast<QPixmap*>(h);
    QString path = QString::fromUtf16(filePath);
    
    bool result = pixmap->load(path);
    
    logf("[Mock] LoadBitmap: %s %s",
         path.toUtf8().constData(), result ? "OK" : "FAILED");
    
    return result ? api_true : api_false;
}

api_bool BitmapContext::SaveBitmap(const_bitmap_handle h,
                               const char16_type* filePath,
                               int32 quality)
{
    if (!h || !filePath) return api_false;
    
    const QPixmap* pixmap = reinterpret_cast<const QPixmap*>(h);
    QString path = QString::fromUtf16(filePath);
    
    bool result = pixmap->save(path, nullptr, quality);
    
    logf("[Mock] SaveBitmap: %s quality=%d %s",
         path.toUtf8().constData(), quality, result ? "OK" : "FAILED");
    
    return result ? api_true : api_false;
}

api_bool BitmapContext::LoadBitmapData(bitmap_handle h,
                                   const void* data,
                                   size_type size,
                                   const char* format,
                                   uint32 flags)
{
    if (!h || !data || size == 0) return api_false;
    
    QPixmap* pixmap = reinterpret_cast<QPixmap*>(h);
    QByteArray bytes(static_cast<const char*>(data), size);
    
    bool result = pixmap->loadFromData(bytes, format);
    
    logf("[Mock] LoadBitmapData: format=%s size=%zu %s",
         format ? format : "auto", size, result ? "OK" : "FAILED");
    
    return result ? api_true : api_false;
}

void BitmapContext::CopyBitmap(bitmap_handle dest,
                           int32 xDst, int32 yDst,
                           const_bitmap_handle src,
                           int32 xSrc, int32 ySrc,
                           int32 width, int32 height)
{
    if (!dest || !src) return;
    
    QPixmap* dstPixmap = reinterpret_cast<QPixmap*>(dest);
    const QPixmap* srcPixmap = reinterpret_cast<const QPixmap*>(src);
    
    QPainter painter(dstPixmap);
    painter.drawPixmap(xDst, yDst, *srcPixmap, xSrc, ySrc, width, height);
    
    logf("[Mock] CopyBitmap: dst(%d,%d) <- src(%d,%d) %dx%d",
         xDst, yDst, xSrc, ySrc, width, height);
}

void BitmapContext::FillBitmap(bitmap_handle h,
                           int32 x, int32 y,
                           int32 width, int32 height,
                           uint32 rgba)
{
    if (!h) return;
    
    QPixmap* pixmap = reinterpret_cast<QPixmap*>(h);
    QPainter painter(pixmap);
    painter.fillRect(x, y, width, height, QColor(rgba));
    
    logf("[Mock] FillBitmap: (%d,%d) %dx%d rgba=0x%08x",
         x, y, width, height, rgba);
}

void BitmapContext::GetBitmapDevicePixelRatio(const_bitmap_handle h, double* ratio)
{
    if (!h || !ratio) return;
    
    const QPixmap* pixmap = reinterpret_cast<const QPixmap*>(h);
    *ratio = pixmap->devicePixelRatio();
}

void BitmapContext::SetBitmapDevicePixelRatio(bitmap_handle h, double ratio)
{
    if (!h) return;
    
    QPixmap* pixmap = reinterpret_cast<QPixmap*>(h);
    pixmap->setDevicePixelRatio(ratio);
    
    logf("[Mock] SetBitmapDevicePixelRatio: %.2f", ratio);
}

// Bitmap operation stubs (implement as needed)
void BitmapContext::OrBitmap(bitmap_handle, int32, int32, int32, int32, uint32) {}
void BitmapContext::OrBitmaps(bitmap_handle, int32, int32, const_bitmap_handle, int32, int32, int32, int32) {}
void BitmapContext::AndBitmap(bitmap_handle, int32, int32, int32, int32, uint32) {}
void BitmapContext::AndBitmaps(bitmap_handle, int32, int32, const_bitmap_handle, int32, int32, int32, int32) {}
void BitmapContext::XorBitmap(bitmap_handle, int32, int32, int32, int32, uint32) {}
void BitmapContext::XorBitmaps(bitmap_handle, int32, int32, const_bitmap_handle, int32, int32, int32, int32) {}
void BitmapContext::XorBitmapRect(bitmap_handle, int32, int32, int32, int32, uint32) {}
void BitmapContext::ReplaceBitmapColor(bitmap_handle, int32, int32, int32, int32, uint32, uint32) {}
void BitmapContext::SetBitmapAlpha(bitmap_handle, int32, int32, int32, int32, uint8) {}

// =============================================================
// UI Context
// =============================================================

struct UIObjectInfo
{
    QWidget* widget;
    api_handle client;
    api_handle module;
    size_type refcnt;
};

static std::map<api_handle, UIObjectInfo> g_uiObjects;

api_bool UIContext::AttachToUIObject(api_handle object, api_handle client)
{
    if (!object) 
    {
        logf("[Mock] UIContext::AttachToUIObject: NULL object");
        return api_false;
    }
    
    logf("[Mock] UIContext::AttachToUIObject: object=%p client=%p", object, client);
    
    // The object handle IS the QWidget pointer
    QWidget* widget = reinterpret_cast<QWidget*>(object);
    
    // Store the UI object info
    UIObjectInfo info;
    info.widget = widget;
    info.client = client;
    
    g_uiObjects[object] = info;
    
    logf("[Mock]   Attached QWidget(%p) to client(%p)", widget, client);
    logf("[Mock]   Total UI objects: %zu", g_uiObjects.size());
    
    return api_true;
}

api_bool UIContext::DetachFromUIObject(api_handle object, api_handle client)
{
    if (!object) return api_true;
    
    logf("[Mock] UIContext::DetachFromUIObject: object=%p client=%p", object, client);
    
    auto it = g_uiObjects.find(object);
    if (it != g_uiObjects.end())
    {
        logf("[Mock]   Found and removing UI object");
        g_uiObjects.erase(it);
        logf("[Mock]   Remaining UI objects: %zu", g_uiObjects.size());
    }
    else
    {
        logf("[Mock]   UI object not found (may have been removed already)");
    }
    
    return api_true;
}

api_handle UIContext::GetUIObjectModule(const_api_handle object)
{
    // Try g_objects first (PCL controls)
    MockBase* C = get(object);
    if (C) return C->moduleHandle;
    
    // Try g_uiObjects (raw widgets)
    auto it = g_uiObjects.find(const_cast<api_handle>(object));
    if (it != g_uiObjects.end())
        return it->second.module;
    
    return nullptr;
}

api_bool UIContext::GetUIObjectType(const_api_handle object, 
                                char* type, 
                                size_type* len)
{
    if (!object) return api_false;
    
    MockBase* C = get(object);
    if (!C) return api_false;
    
    const char* typeName = "Control";
    
    if (C->isSizer) {
        typeName = "Sizer";
    } else if (C->isTreeNode) {
        typeName = "TreeNode";
    } else if (C->widget) {
        if (qobject_cast<QLabel*>(C->widget))
            typeName = "Label";
        else if (qobject_cast<QLineEdit*>(C->widget))
            typeName = "Edit";
        else if (qobject_cast<QSlider*>(C->widget))
            typeName = "Slider";
        else if (qobject_cast<QCheckBox*>(C->widget))
            typeName = "CheckBox";
        else if (qobject_cast<QRadioButton*>(C->widget))
            typeName = "RadioButton";
        else if (qobject_cast<QPushButton*>(C->widget))
            typeName = "PushButton";
        else if (qobject_cast<QToolButton*>(C->widget))
            typeName = "ToolButton";
        else if (qobject_cast<QComboBox*>(C->widget))
            typeName = "ComboBox";
        else if (qobject_cast<QSpinBox*>(C->widget))
            typeName = "SpinBox";
        else if (qobject_cast<QTreeWidget*>(C->widget))
            typeName = "TreeBox";
    }
    
    size_t typeLen = std::strlen(typeName);
    
    if (len) *len = typeLen;
    
    if (type && len && *len > 0) {
        size_t copyLen = std::min(*len, typeLen);
        std::memcpy(type, typeName, copyLen);
        if (copyLen < *len) type[copyLen] = '\0';
    }
    
    return api_true;
}

api_bool UIContext::GetUIObjectId(const_api_handle object, 
                              char16_type* id, 
                              size_type* len)
{
    if (!object) return api_false;
    
    MockBase* C = get(object);
    if (!C || !C->widget) return api_false;
    
    QString objectName = C->widget->objectName();
    
    if (len) *len = objectName.length();
    
    if (id && len && *len > 0) {
        const char16_type* src = reinterpret_cast<const char16_type*>(
            objectName.utf16());
        size_t copyLen = std::min(*len, static_cast<size_type>(objectName.length()));
        std::memcpy(id, src, copyLen * sizeof(char16_t));
        if (copyLen < *len) id[copyLen] = 0;
    }
    
    return api_true;
}

api_bool UIContext::SetUIObjectId(api_handle object, const char16_type* id)
{
    if (!object || !id) return api_false;
    
    MockBase* C = get(object);
    if (!C || !C->widget) return api_false;
    
    QString qid = QString::fromUtf16(id);
    C->widget->setObjectName(qid);
    
    logf("[Mock] UIContext::SetUIObjectId: %s", qid.toUtf8().constData());
    
    return api_true;
}

size_type UIContext::GetUIObjectRefCount(const_api_handle object)
{
    // Try g_uiObjects (raw widgets)
    auto it = g_uiObjects.find(const_cast<api_handle>(object));
    if (it != g_uiObjects.end())
        return it->second.refcnt;
    return 1;
 }

api_bool UIContext::SetHandleDestroyedEventRoutine(api_handle object,
                                               pcl::destroy_event_routine routine)
{
    logf("[Mock] UIContext::SetHandleDestroyedEventRoutine: object=%p", object);
    return api_true;
}

// UI Control Object functions (aliases)
api_bool UIContext::AttachToUIControlObject(api_handle object, control_handle client)
{
    return AttachToUIObject(object, client);
}

api_bool UIContext::DetachFromUIControlObject(api_handle object, control_handle client)
{
    return DetachFromUIObject(object, client);
}

api_handle UIContext::GetUIControlObjectModule(const_control_handle object)
{
    return GetUIObjectModule(object);
}

size_type UIContext::GetUIControlObjectRefCount(const_control_handle object)
{
    return GetUIObjectRefCount(object);
}

api_bool UIContext::GetUIControlObjectType(const_control_handle object, 
                                char* type, 
                                size_type* len)
{
    return GetUIObjectType(object, type, len);
}

api_bool UIContext::GetUIControlObjectId(const_api_handle object, 
                              char16_type* id, 
                              size_type* len)
{
    return GetUIObjectId(object, id, len);
}

api_bool UIContext::SetUIControlObjectId(api_handle object, const char16_type* id)
{
    return SetUIObjectId(object, id);
}

// =============================================================
// Font Context
// =============================================================

font_handle FontContext::CloneFont(api_handle, const_font_handle h)
{
    if (!h) return nullptr;
    
    const QFont* src = reinterpret_cast<const QFont*>(h);
    return reinterpret_cast<font_handle>(new QFont(*src));
}

int32 FontContext::GetStringPixelWidth(const_font_handle h, const char16_type* text)
{
    QString s = QString::fromUtf16(text);
    
    QFont f;
    if (h) {
        const QFont* font = reinterpret_cast<const QFont*>(h);
        f = *font;
    }
    
    QFontMetrics fm(f);
    return fm.horizontalAdvance(s);
}

int32 FontContext::GetFontHeight(const_font_handle h)
{
    QFont f;
    if (h) {
        const QFont* font = reinterpret_cast<const QFont*>(h);
        f = *font;
    }
    
    QFontMetrics fm(f);
    return fm.height();
}

// =============================================================
// Global Context
// =============================================================

api_bool GlobalContext::Abort()
{
    logf("[Mock] GlobalContext::Abort called");
    abort();
}

void* GlobalContext::Allocate(size_type size)
{
    return malloc(size);
}

api_bool GlobalContext::Deallocate(void* ptr)
{
    free(ptr);
    return api_true;
}

api_bool GlobalContext::EnableAbort()
{
    return api_true;
}

api_bool GlobalContext::ErrorMessage(uint32, char16_type*, size_type*)
{
    return api_false;
}

console_handle GlobalContext::GetConsole()
{
    return reinterpret_cast<console_handle>(new MockBase());
}

api_bool GlobalContext::GetGlobalString(const char*, char16_type*, size_type*)
{
    return api_false;
}

uint32 GlobalContext::GetKeyboardModifiers()
{
    return QApplication::keyboardModifiers();
}

void GlobalContext::LaunchProcessInstance(meta_process_handle, const_process_handle, int32, uint32) {}
void GlobalContext::LaunchProcessInstanceOnView(meta_process_handle, const_process_handle, view_handle, uint32) {}

uint32 GlobalContext::MessageBox(const char16_type* text, 
                                 const char16_type* caption, 
                                 uint32 button0, 
                                 uint32 button1, 
                                 uint32 button2, 
                                 uint32 defButton, 
                                 uint32 escButton, 
                                 uint32 icon)
{
    QString qtext = QString::fromUtf16(text);
    QString qcaption = QString::fromUtf16(caption);
    
    QMessageBox msgBox;
    msgBox.setText(qtext);
    msgBox.setWindowTitle(qcaption);
    
    return msgBox.exec();
}
 
api_bool GlobalContext::ShowConsole(console_handle, api_bool)
{
    return api_true;
}

api_bool GlobalContext::WriteConsole(console_handle, const char16_type *text, api_bool newline)
{
  // Forward to MockMain's console widget
  MockMainWriteConsole(text, newline != 0);
  return api_true;
}

api_bool GlobalContext::WriteSettingsInteger(api_handle, int32, const char*, api_bool)
{
    return api_true;
}

int32 GlobalContext::MaxProcessorsAllowedForModule(void*, uint32)
{
    return QThread::idealThreadCount();
}

const api_pixtraits_lut* GlobalContext::GetPixelTraitsLUT(uint32)
{
    static api_pixtraits_lut lut = {};
    return &lut;
}

api_bool GlobalContext::BrowseProcessDocumentation(meta_process_handle, uint32)
{
    return api_false;
}

// =============================================================
// Process Context
// =============================================================

process_handle ProcessContext::CloneProcessInstance(api_handle, const_process_handle, uint32)
{
    return nullptr;
}

// =============================================================
// Dialog Context
// =============================================================

api_bool DialogContext::ExecuteOpenFileDialog(char16_type*, const char16_type*, const char16_type*, const char16_type*, const char16_type*)
{
    return api_false;
}

api_bool DialogContext::ExecuteOpenMultipleFilesDialog(char16_type*, uint32(*)(const char16_type*, void*), void*, const char16_type*, const char16_type*, const char16_type*, const char16_type*)
{
    return api_false;
}

// Graphics, SVG, Brush, Pen contexts - all stubs
api_bool GraphicsContext::GetGraphicsStatus(const_graphics_handle) { return api_false; }
void GraphicsContext::EndPaint(graphics_handle) {}

// =============================================================
// Thread Context (uses PCLThreadMock)
// =============================================================

thread_handle ThreadContext::CreateThread(api_handle handle, api_handle client, uint32 flags)
{
    return pcl_mock::CreateThread(handle, client, flags);
}

void ThreadContext::StartThread(thread_handle handle, uint32 priority)
{
    pcl_mock::StartThread(handle, priority);
}

void ThreadContext::KillThread(thread_handle) {}

api_bool ThreadContext::IsThreadActive(const_thread_handle handle)
{
    return pcl_mock::IsThreadActive(handle);
}

uint32 ThreadContext::GetThreadPriority(const_thread_handle) { return 0; }
void ThreadContext::SetThreadPriority(thread_handle, uint32) {}
uint32 ThreadContext::GetThreadStackSize(const_thread_handle) { return 0; }
void ThreadContext::SetThreadStackSize(thread_handle, uint32) {}
api_bool ThreadContext::WaitThread(thread_handle, uint32) { return api_false; }
void ThreadContext::SleepThread(thread_handle, uint32) {}
uint32 ThreadContext::GetThreadStatus(const_thread_handle) { return 0; }
void ThreadContext::SetThreadStatus(thread_handle, uint32) {}
api_bool ThreadContext::GetThreadStatusEx(const_thread_handle, uint32*, uint32) { return api_false; }
api_bool ThreadContext::GetThreadConsoleOutputText(const_thread_handle, char16_type*, size_type*) { return api_false; }
void ThreadContext::AppendThreadConsoleOutputText(thread_handle, const char16_type*, api_bool) {}
void ThreadContext::ClearThreadConsoleOutputText(thread_handle) {}

thread_handle ThreadContext::GetCurrentThread()
{
    return pcl_mock::GetCurrentThread();
}

api_bool ThreadContext::SetThreadExecRoutine(thread_handle handle, pcl::thread_exec_routine dispatcher)
{
    return pcl_mock::SetThreadExecRoutine(handle, dispatcher);
}

int32 ThreadContext::PerformanceAnalysisValue(int32, size_type, int32, api_bool, int32, int32, int32)
{
    return 0;
}

// =============================================================
// Mutex Context - Stubs
// =============================================================

mutex_handle Mutex_CreateMutex(api_handle, uint32) { return nullptr; }
mutex_handle Mutex_CreateReadWriteMutex(api_handle, uint32) { return nullptr; }
api_bool Mutex_GetLockState(const_mutex_handle) { return api_false; }
api_bool Mutex_Lock(mutex_handle, api_bool) { return api_false; }
api_bool Mutex_LockForRead(mutex_handle, api_bool) { return api_false; }
api_bool Mutex_LockForWrite(mutex_handle, api_bool) { return api_false; }
void Mutex_Unlock(mutex_handle) {}

// =============================================================
// ViewList Context - Stubs
// =============================================================

control_handle ViewList_CreateViewList(api_handle, control_handle, uint32) { return nullptr; }
void ViewList_RegenerateViewList(control_handle, api_bool, api_bool, api_bool) {}
void ViewList_GetViewListContents(const_control_handle, api_bool*, api_bool*, api_bool*) {}
const_view_handle ViewList_GetViewListExcludedView(const_control_handle) { return nullptr; }
void ViewList_SetViewListExcludedView(control_handle, const_view_handle) {}
view_handle ViewList_GetViewListCurrentView(const_control_handle) { return nullptr; }
void ViewList_SetViewListCurrentView(control_handle, view_handle) {}
api_bool ViewList_FindViewListView(const_control_handle, const_view_handle) { return api_false; }
void ViewList_RemoveViewListView(control_handle, const_view_handle) {}
api_bool ViewList_SetViewListViewSelectedEventRoutine(control_handle, api_handle, pcl::view_event_routine) { return api_true; }
api_bool ViewList_SetViewListCurrentViewUpdatedEventRoutine(control_handle, api_handle, pcl::view_event_routine) { return api_true; }

// =============================================================
// Numerical Context - Stubs
// =============================================================


void LogDebug(const std::string& message) {
  qDebug() << "[PCLMockAPI] " << message.c_str() << "\n";    
}

struct FFTTransform {
    int size;
    bool isReal;
    bool isDouble;
    void* buffer;  // To simulate memory allocation
    
    FFTTransform(int n, bool real, bool dbl) : 
        size(n), isReal(real), isDouble(dbl) {
        // Allocate some memory to simulate FFT buffer
        size_t bufferSize = n * (isReal ? 1 : 2) * (isDouble ? sizeof(double) : sizeof(float));
        buffer = malloc(bufferSize);
    }
    
    ~FFTTransform() {
        if (buffer) {
            free(buffer);
            buffer = nullptr;
        }
    }
};

// Map to keep track of created FFT transforms
static std::map<void*, FFTTransform*> g_fft_transforms;
static std::mutex g_fft_mutex;

// Typedef for the FFT transform function signature
typedef api_bool (*fft_real_transform_d_func)(void* handle, dcomplex* y, const double* x);

// Structure to store FFTW plan information
struct FFTWPlanWrapper {
    int size;
    bool isReal;
    bool forward;
    
    // FFTW plans
    fftw_plan forwardPlan;
    fftw_plan inversePlan;
    
    // Buffers for real transforms
    double* realIn;
    double* realOut;
    fftw_complex* complexIn;
    fftw_complex* complexOut;
    
    FFTWPlanWrapper(int n, bool real) : 
        size(n), isReal(real), forward(true) {
        
        if (isReal) {
            // Real-to-complex and complex-to-real transforms
            realIn = (double*)fftw_malloc(sizeof(double) * n);
            realOut = (double*)fftw_malloc(sizeof(double) * n);
            complexOut = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (n/2 + 1));
            complexIn = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (n/2 + 1));
            
            // Create plans
            forwardPlan = fftw_plan_dft_r2c_1d(n, realIn, complexOut, FFTW_MEASURE);
            inversePlan = fftw_plan_dft_c2r_1d(n, complexIn, realOut, FFTW_MEASURE);
        } else {
            // Complex-to-complex transforms
            complexIn = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * n);
            complexOut = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * n);
            realIn = nullptr;
            realOut = nullptr;
            
            // Create plans
            forwardPlan = fftw_plan_dft_1d(n, complexIn, complexOut, FFTW_FORWARD, FFTW_MEASURE);
            inversePlan = fftw_plan_dft_1d(n, complexIn, complexOut, FFTW_BACKWARD, FFTW_MEASURE);
        }
    }
    ~FFTWPlanWrapper() {
        // Destroy plans
        fftw_destroy_plan(forwardPlan);
        fftw_destroy_plan(inversePlan);
        
        // Free buffers
        if (realIn) fftw_free(realIn);
        if (realOut) fftw_free(realOut);
        if (complexIn) fftw_free(complexIn);
        if (complexOut) fftw_free(complexOut);
    }
};

// Map to keep track of created FFT transforms
static std::map<void*, FFTWPlanWrapper*> g_fftw_plans;
static std::mutex g_fftw_mutex;

// Initialize FFTW library
void InitializeFFTW() {
    static bool initialized = false;
    if (!initialized) {
        fftw_init_threads();
        fftw_plan_with_nthreads(4); // Use multiple threads for better performance
        initialized = true;
    }
}

// Clean up FFTW resources
void CleanupFFTW() {
    std::lock_guard<std::mutex> lock(g_fftw_mutex);
    for (auto& pair : g_fftw_plans) {
        delete pair.second;
    }
    g_fftw_plans.clear();
    fftw_cleanup_threads();
    fftw_cleanup();
}

//======================= PCL API FUNCTIONS =======================

// Returns the next power of 2 greater than or equal to n
size_type NumericalContext::FFTRealOptimizedLengthF(size_type n) {
    LogDebug("FFTRealOptimizedLengthF called with n=" + std::to_string(n));
    
    // Ensure we never return a value smaller than the input
    if (n <= 0) return 1;
    
    // FFTW works best with sizes that are products of small primes
    // For simplicity, we'll use powers of 2, but FFTW actually works well with many sizes
    int result = 1;
    while (result < n) {
        result *= 2;
    }
    
    LogDebug("FFTRealOptimizedLengthF returning " + std::to_string(result));
    return result;
}

// Complex FFT optimized length (similar implementation)
size_type NumericalContext::FFTComplexOptimizedLengthF(size_type n) {
  return NumericalContext::FFTRealOptimizedLengthF(n);
}

// Create a real transform
void* NumericalContext::FFTCreateRealTransformD(size_type n) {
    LogDebug("FFTCreateRealTransformD called with n=" + std::to_string(n));
    
    InitializeFFTW();
    
    // Create a new transform wrapper
    FFTWPlanWrapper* wrapper = new FFTWPlanWrapper(n, true);
    void* handle = wrapper;  // Use the pointer as the handle
    
    // Store in our map
    std::lock_guard<std::mutex> lock(g_fftw_mutex);
    g_fftw_plans[handle] = wrapper;
    
    LogDebug("FFTCreateRealTransformD returning handle " + std::to_string((uintptr_t)handle));
    return handle;
}

api_bool NumericalContext::GaussJordanInPlaceF(float**, float**, int32, int32) { return api_false; }
api_bool NumericalContext::GaussJordanInPlaceD(double**, double**, int32, int32) { return api_false; }
api_bool NumericalContext::SVDInPlaceF(float**, float*, float**, int32, int32) { return api_false; }
api_bool NumericalContext::SVDInPlaceD(double**, double*, double**, int32, int32) { return api_false; }
api_enum NumericalContext::LinearFitF(double*, double*, double*, const float*, const float*, size_type, api_bool(*)(void*), void*) { return 0; }
api_enum NumericalContext::LinearFitD(double*, double*, double*, const double*, const double*, size_type, api_bool(*)(void*), void*) { return 0; }
api_bool NumericalContext::FFTDestroyTransform(fft_handle) { return api_false; }
fft_handle NumericalContext::FFTCreateComplexTransformD(size_type) { return nullptr; }
api_bool NumericalContext::FFTRealTransformD(fft_handle, void*, const double*) { return api_false; }
fft_handle NumericalContext::FFTCreateComplexInverseTransformD(size_type) { return nullptr; }

// =============================================================
// FileFormat Context - Stubs
// =============================================================

api_bool FileFormatContext::GetFileFormatName(meta_format_handle, char*, size_type*) { return api_false; }
file_format_handle FileFormatContext::CreateFileFormatInstance(api_handle, meta_format_handle) { return nullptr; }
api_bool FileFormatContext::ReadImage(file_format_handle, image_handle) { return api_false; }
api_bool FileFormatContext::GetImageId(const_file_format_handle, char*, size_type*, uint32) { return api_false; }
api_bool FileFormatContext::SelectImage(file_format_handle, uint32) { return api_false; }
api_bool FileFormatContext::GetImageCount(const_file_format_handle) { return api_false; }
api_bool FileFormatContext::OpenImageFileEx(file_format_handle, const char16_type*, const char*, uint32) { return api_false; }
bitmap_handle FileFormatContext::GetFileFormatIcon(const_meta_format_handle) { return nullptr; }
meta_format_handle FileFormatContext::GetFileFormatByName(api_handle, const char*) { return nullptr; }
api_bool FileFormatContext::GetFileFormatStatus(const_file_format_handle, char16_type*, size_type*, void*) { return api_false; }
api_bool FileFormatContext::GetImageDescription(const_file_format_handle, api_image_info*, api_image_options*, uint32) { return api_false; }
uint32 FileFormatContext::GetFileFormatVersion(const_meta_format_handle) { return 0; }
api_bool FileFormatContext::GetFileFormatMimeTypes(const_meta_format_handle, char**, size_type*, size_type*) { return api_false; }
bitmap_handle FileFormatContext::GetFileFormatSmallIcon(const_meta_format_handle) { return nullptr; }
meta_format_handle FileFormatContext::GetFileFormatByMimeType(api_handle, const char*, uint32, uint32) { return nullptr; }
api_bool FileFormatContext::GetFileFormatDescription(const_meta_format_handle, char16_type*, size_type*) { return api_false; }
void FileFormatContext::DisposeFormatSpecificData(const_meta_format_handle, const void*) {}
api_bool FileFormatContext::EditFileFormatPreferences(const_meta_format_handle) { return api_false; }
api_bool FileFormatContext::GetFileFormatCapabilities(const_meta_format_handle, api_format_capabilities*) { return api_false; }
api_bool FileFormatContext::ValidateFormatSpecificData(const_meta_format_handle, const void*) { return api_false; }
api_bool FileFormatContext::GetFileFormatFileExtensions(const_meta_format_handle, char16_type**, size_type*, size_type*) { return api_false; }
api_bool FileFormatContext::GetFileFormatImplementation(const_meta_format_handle, char16_type*, size_type*) { return api_false; }
meta_format_handle FileFormatContext::GetFileFormatByFileExtension(api_handle, const char16_type*, uint32, uint32) { return nullptr; }

// =============================================================
// Module/Process/Interface Definition - Stubs
// =============================================================

void ModuleDefinitionContext::SetModuleOnLoadRoutine(pcl::module_on_load_routine) {}
void ModuleDefinitionContext::SetModuleOnUnloadRoutine(pcl::module_on_unload_routine) {}
void ModuleDefinitionContext::SetModuleAllocationRoutine(pcl::module_allocation_routine) {}
void ModuleDefinitionContext::SetModuleDeallocationRoutine(pcl::module_deallocation_routine) {}
void ModuleDefinitionContext::ExitModuleDefinitionContext() {}
void ModuleDefinitionContext::EnterModuleDefinitionContext() {}

// ProcessDefinition - large stub section
void ProcessDefinitionContext::EnterProcessDefinitionContext() {}
api_bool ProcessDefinitionContext::IsProcessDefinitionContextActive() { return api_false; }
void ProcessDefinitionContext::BeginProcessDefinition(meta_process_handle, const char*) {}
api_bool ProcessDefinitionContext::GetProcessBeingDefined(char*, size_type*) { return api_false; }
void ProcessDefinitionContext::SetProcessCategory(const char*) {}
void ProcessDefinitionContext::SetProcessVersion(uint32) {}
void ProcessDefinitionContext::SetProcessAliasIdentifiers(const char*) {}
void ProcessDefinitionContext::SetProcessDescription(const char16_type*) {}
void ProcessDefinitionContext::SetProcessScriptComment(const char16_type*) {}
void ProcessDefinitionContext::SetProcessIconSVG(const char*) {}
void ProcessDefinitionContext::SetProcessIconSVGFile(const char16_type*) {}
void ProcessDefinitionContext::SetProcessIconImage(const char**) {}
void ProcessDefinitionContext::SetProcessIconImageFile(const char16_type*) {}
void ProcessDefinitionContext::SetProcessIconSmallImage(const char**) {}
void ProcessDefinitionContext::SetProcessIconSmallImageFile(const char16_type*) {}
void ProcessDefinitionContext::SetProcessClassInitializationRoutine(pcl::process_class_initialization_routine) {}
void ProcessDefinitionContext::SetProcessCreationRoutine(pcl::process_creation_routine) {}
void ProcessDefinitionContext::SetProcessDestructionRoutine(pcl::process_destruction_routine) {}
void ProcessDefinitionContext::SetProcessClonationRoutine(pcl::process_clonation_routine) {}
void ProcessDefinitionContext::SetProcessTestClonationRoutine(pcl::process_test_clonation_routine) {}
void ProcessDefinitionContext::SetProcessSetServerHandleRoutine(pcl::process_set_handle_routine) {}
void ProcessDefinitionContext::SetProcessAssignmentRoutine(pcl::process_assignment_routine) {}
void ProcessDefinitionContext::SetProcessInitializationRoutine(pcl::process_initialization_routine) {}
void ProcessDefinitionContext::SetProcessValidationRoutine(pcl::process_validation_routine) {}
void ProcessDefinitionContext::SetProcessCommandLineProcessingRoutine(pcl::process_command_line_processing_routine, uint32) {}
void ProcessDefinitionContext::SetProcessEditPreferencesRoutine(pcl::process_edit_preferences_routine) {}
void ProcessDefinitionContext::SetProcessBrowseDocumentationRoutine(pcl::process_browse_documentation_routine) {}
void ProcessDefinitionContext::SetProcessExecutionPreferencesRoutine(pcl::process_execution_preferences_routine) {}
void ProcessDefinitionContext::SetProcessExecutionValidationRoutine(pcl::process_execution_validation_routine) {}
void ProcessDefinitionContext::SetProcessMaskValidationRoutine(pcl::process_mask_validation_routine) {}
void ProcessDefinitionContext::SetProcessHistoryUpdateValidationRoutine(pcl::process_history_update_validation_routine) {}
void ProcessDefinitionContext::SetProcessUndoModeRoutine(pcl::process_undo_mode_routine) {}
void ProcessDefinitionContext::SetProcessPreExecutionRoutine(pcl::process_pre_execution_routine) {}
void ProcessDefinitionContext::SetProcessExecutionRoutine(pcl::process_execution_routine) {}
void ProcessDefinitionContext::SetProcessPostExecutionRoutine(pcl::process_post_execution_routine) {}
void ProcessDefinitionContext::SetProcessGlobalExecutionValidationRoutine(pcl::process_global_execution_validation_routine) {}
void ProcessDefinitionContext::SetProcessPreGlobalExecutionRoutine(pcl::process_pre_global_execution_routine) {}
void ProcessDefinitionContext::SetProcessGlobalExecutionRoutine(pcl::process_global_execution_routine) {}
void ProcessDefinitionContext::SetProcessPostGlobalExecutionRoutine(pcl::process_post_global_execution_routine) {}
void ProcessDefinitionContext::SetProcessImageExecutionValidationRoutine(pcl::process_image_execution_validation_routine) {}
void ProcessDefinitionContext::SetProcessImageExecutionRoutine(pcl::process_image_execution_routine) {}
void ProcessDefinitionContext::SetProcessDefaultInterfaceSelectionRoutine(pcl::process_default_interface_selection_routine) {}
void ProcessDefinitionContext::SetProcessInterfaceSelectionRoutine(pcl::process_interface_selection_routine) {}
void ProcessDefinitionContext::SetProcessInterfaceValidationRoutine(pcl::process_interface_validation_routine) {}
void ProcessDefinitionContext::SetProcessPreReadingRoutine(pcl::process_pre_reading_routine) {}
void ProcessDefinitionContext::SetProcessPostReadingRoutine(pcl::process_post_reading_routine) {}
void ProcessDefinitionContext::SetProcessPreWritingRoutine(pcl::process_pre_writing_routine) {}
void ProcessDefinitionContext::SetProcessPostWritingRoutine(pcl::process_post_writing_routine) {}
void ProcessDefinitionContext::SetProcessIPCStartRoutine(pcl::process_ipc_notification_routine) {}
void ProcessDefinitionContext::SetProcessIPCStopRoutine(pcl::process_ipc_notification_routine) {}
void ProcessDefinitionContext::SetProcessIPCSetParametersRoutine(pcl::process_ipc_notification_routine) {}
void ProcessDefinitionContext::SetProcessIPCGetStatusRoutine(pcl::process_ipc_status_routine) {}
void ProcessDefinitionContext::BeginParameterDefinition(meta_parameter_handle, const char*, uint32) {}
api_bool ProcessDefinitionContext::GetParameterBeingDefined(char*, size_type*) { return api_false; }
void ProcessDefinitionContext::SetParameterProcessVersionRange(uint32, uint32) {}
void ProcessDefinitionContext::SetParameterRequired(api_bool) {}
void ProcessDefinitionContext::SetParameterReadOnly(api_bool) {}
void ProcessDefinitionContext::SetParameterAliasIdentifiers(const char*) {}
void ProcessDefinitionContext::SetParameterDescription(const char16_type*) {}
void ProcessDefinitionContext::SetParameterScriptComment(const char16_type*) {}
void ProcessDefinitionContext::SetParameterLockRoutine(pcl::parameter_lock_routine) {}
void ProcessDefinitionContext::SetParameterUnlockRoutine(pcl::parameter_unlock_routine) {}
void ProcessDefinitionContext::SetParameterValidationRoutine(pcl::parameter_validation_routine) {}
void ProcessDefinitionContext::SetParameterAllocationRoutine(pcl::parameter_allocation_routine) {}
void ProcessDefinitionContext::SetParameterLengthQueryRoutine(pcl::parameter_length_query_routine) {}
void ProcessDefinitionContext::SetDefaultNumericValue(double) {}
void ProcessDefinitionContext::SetValidNumericRange(double, double) {}
void ProcessDefinitionContext::SetPrecision(int32) {}
void ProcessDefinitionContext::SetScientificNotation(api_bool) {}
void ProcessDefinitionContext::SetDefaultBooleanValue(api_bool) {}
void ProcessDefinitionContext::DefineEnumerationElement(const char*, api_enum) {}
void ProcessDefinitionContext::DefineEnumerationAlias(const char*, const char*) {}
void ProcessDefinitionContext::SetDefaultEnumerationValueIndex(uint32) {}
void ProcessDefinitionContext::SetDefaultStringValue(const char16_type*) {}
void ProcessDefinitionContext::SetStringAllowedCharacters(const char16_type*) {}
void ProcessDefinitionContext::SetStringLengthLimits(size_type, size_type) {}
void ProcessDefinitionContext::BeginTableColumnDefinition(meta_parameter_handle, const char*, uint32) {}
void ProcessDefinitionContext::EndTableColumnDefinition() {}
void ProcessDefinitionContext::SetTableRowLimits(size_type, size_type) {}
void ProcessDefinitionContext::SetBlockSizeLimits(size_type, size_type) {}
void ProcessDefinitionContext::EndParameterDefinition() {}
void ProcessDefinitionContext::EndProcessDefinition() {}
void ProcessDefinitionContext::ExitProcessDefinitionContext() {}

// InterfaceDefinition - stubs
void InterfaceDefinitionContext::EnterInterfaceDefinitionContext() {}
api_bool InterfaceDefinitionContext::IsInterfaceDefinitionContextActive() { return api_false; }
void InterfaceDefinitionContext::BeginInterfaceDefinition(meta_interface_handle, const char*, uint32) {}
api_bool InterfaceDefinitionContext::GetInterfaceBeingDefined(char*, size_type*) { return api_false; }
void InterfaceDefinitionContext::SetInterfaceVersion(uint32) {}
void InterfaceDefinitionContext::SetInterfaceAliasIdentifiers(const char*) {}
void InterfaceDefinitionContext::SetInterfaceDescription(const char16_type*) {}
void InterfaceDefinitionContext::SetInterfaceIconSVG(const char*) {}
void InterfaceDefinitionContext::SetInterfaceIconSVGFile(const char16_type*) {}
void InterfaceDefinitionContext::SetInterfaceIconImage(const char**) {}
void InterfaceDefinitionContext::SetInterfaceIconImageFile(const char16_type*) {}
void InterfaceDefinitionContext::SetInterfaceIconSmallImage(const char**) {}
void InterfaceDefinitionContext::SetInterfaceIconSmallImageFile(const char16_type*) {}
void InterfaceDefinitionContext::SetInterfaceFeatures(uint32, uint32) {}
// ... (many more interface definition stubs - add as needed)
void InterfaceDefinitionContext::EndInterfaceDefinition() {}
void InterfaceDefinitionContext::ExitInterfaceDefinitionContext() {}

// =============================================================
// External Process, Network Transfer, Graphics - All Stubs
// =============================================================

// ExternalProcess stubs
int32 ExternalProcess_ExecuteProgram(const char16_type*, const char16_type**, size_type) { return 0; }
api_bool ExternalProcess_StartProgram(const char16_type*, const char16_type**, size_type, const char16_type*, uint64*) { return api_false; }
external_process_handle ExternalProcess_CreateExternalProcess(api_handle, api_handle) { return nullptr; }

// NetworkTransfer stubs
network_transfer_handle NetworkTransfer_CreateNetworkTransfer(api_handle, api_handle) { return nullptr; }

// Graphics stubs
graphics_handle Graphics_CreateGraphics(api_handle) { return nullptr; }
api_bool Graphics_BeginControlPaint(graphics_handle, control_handle) { return api_false; }
api_bool Graphics_BeginBitmapPaint(graphics_handle, bitmap_handle) { return api_false; }

// SVG stubs
svg_handle SVG_CreateSVGFile(api_handle, const char16_type*, int32, int32, uint32) { return nullptr; }
svg_handle SVG_CreateSVGBuffer(api_handle, int32, int32, uint32) { return nullptr; }

// Brush stubs
brush_handle Brush_CreateBrush(api_handle, uint32, int32) { return nullptr; }
brush_handle Brush_CreateBitmapBrush(api_handle, const_bitmap_handle) { return nullptr; }

// Pen stubs
pen_handle Pen_CreatePen(api_handle, uint32, float, int32, int32, int32) { return nullptr; }
pen_handle Pen_ClonePen(api_handle, const_pen_handle) { return nullptr; }

// CodeEditor, WebView, ImageView stubs - add as needed
control_handle CodeEditor_CreateCodeEditor(api_handle, api_handle, control_handle, uint32) { return nullptr; }
control_handle WebViewContext::CreateWebView(api_handle, api_handle, control_handle, uint32) { return nullptr; }
control_handle ImageViewContext::CreateImageView(api_handle, api_handle, control_handle, uint32, int32, int32, int32, int32, api_bool, api_bool) { return nullptr; }

// =============================================================
// InterfaceDefinitionContext - Complete Stubs
// =============================================================

void InterfaceDefinitionContext::SetInterfaceInitializationRoutine(pcl::interface_initialization_routine) {}
void InterfaceDefinitionContext::SetInterfaceLaunchRoutine(pcl::interface_launch_routine) {}
void InterfaceDefinitionContext::SetInterfaceProcessInstantiationRoutine(pcl::interface_process_instantiation_routine) {}
void InterfaceDefinitionContext::SetInterfaceProcessTestInstantiationRoutine(pcl::interface_process_instantiation_routine) {}
void InterfaceDefinitionContext::SetInterfaceProcessValidationRoutine(pcl::interface_process_validation_routine) {}
void InterfaceDefinitionContext::SetInterfaceProcessImportRoutine(pcl::interface_process_import_routine) {}

void InterfaceDefinitionContext::SetInterfaceApplyRoutine(pcl::interface_control_routine) {}
void InterfaceDefinitionContext::SetInterfaceApplyGlobalRoutine(pcl::interface_control_routine) {}
void InterfaceDefinitionContext::SetInterfaceRealTimePreviewUpdatedRoutine(pcl::interface_control_state_routine) {}
void InterfaceDefinitionContext::SetInterfaceExecuteRoutine(pcl::interface_control_routine) {}
void InterfaceDefinitionContext::SetInterfaceCancelRoutine(pcl::interface_control_routine) {}
void InterfaceDefinitionContext::SetInterfaceBrowseDocumentationRoutine(pcl::interface_control_routine) {}
void InterfaceDefinitionContext::SetInterfaceTrackViewUpdatedRoutine(pcl::interface_control_state_routine) {}
void InterfaceDefinitionContext::SetInterfaceEditPreferencesRoutine(pcl::interface_control_routine) {}
void InterfaceDefinitionContext::SetInterfaceResetRoutine(pcl::interface_control_routine) {}

void InterfaceDefinitionContext::SetInterfaceRealTimeUpdateQueryRoutine(pcl::interface_real_time_update_query_routine) {}
void InterfaceDefinitionContext::SetInterfaceRealTimeGenerationFlagsRoutine(pcl::interface_real_time_generation_flags_routine) {}
void InterfaceDefinitionContext::SetInterfaceRealTimeGenerationRoutine(pcl::interface_real_time_generation_routine) {}
void InterfaceDefinitionContext::SetInterfaceRealTimeCancelRoutine(pcl::interface_real_time_cancel_routine) {}

void InterfaceDefinitionContext::SetInterfaceDynamicModeEnterRoutine(pcl::interface_dynamic_mode_enter_routine) {}
void InterfaceDefinitionContext::SetInterfaceDynamicModeExitRoutine(pcl::interface_dynamic_mode_exit_routine) {}
void InterfaceDefinitionContext::SetInterfaceDynamicMouseEnterRoutine(pcl::interface_dynamic_view_event_routine) {}
void InterfaceDefinitionContext::SetInterfaceDynamicMouseLeaveRoutine(pcl::interface_dynamic_view_event_routine) {}
void InterfaceDefinitionContext::SetInterfaceDynamicMouseMoveRoutine(pcl::interface_dynamic_mouse_routine) {}
void InterfaceDefinitionContext::SetInterfaceDynamicMousePressRoutine(pcl::interface_dynamic_mouse_button_routine) {}
void InterfaceDefinitionContext::SetInterfaceDynamicMouseReleaseRoutine(pcl::interface_dynamic_mouse_button_routine) {}
void InterfaceDefinitionContext::SetInterfaceDynamicMouseDoubleClickRoutine(pcl::interface_dynamic_mouse_routine) {}
void InterfaceDefinitionContext::SetInterfaceDynamicKeyPressRoutine(pcl::interface_dynamic_keyboard_routine) {}
void InterfaceDefinitionContext::SetInterfaceDynamicKeyReleaseRoutine(pcl::interface_dynamic_keyboard_routine) {}
void InterfaceDefinitionContext::SetInterfaceDynamicMouseWheelRoutine(pcl::interface_dynamic_wheel_routine) {}
void InterfaceDefinitionContext::SetInterfaceDynamicUpdateQueryRoutine(pcl::interface_dynamic_update_query_routine) {}
void InterfaceDefinitionContext::SetInterfaceDynamicPaintRoutine(pcl::interface_dynamic_paint_routine) {}

void InterfaceDefinitionContext::SetImageCreatedNotificationRoutine(pcl::image_notification_routine) {}
void InterfaceDefinitionContext::SetImageUpdatedNotificationRoutine(pcl::image_notification_routine) {}
void InterfaceDefinitionContext::SetImageRenamedNotificationRoutine(pcl::image_notification_routine) {}
void InterfaceDefinitionContext::SetImageDeletedNotificationRoutine(pcl::image_notification_routine) {}
void InterfaceDefinitionContext::SetImageFocusedNotificationRoutine(pcl::image_notification_routine) {}
void InterfaceDefinitionContext::SetImageLockedNotificationRoutine(pcl::image_notification_routine) {}
void InterfaceDefinitionContext::SetImageUnlockedNotificationRoutine(pcl::image_notification_routine) {}
void InterfaceDefinitionContext::SetImageSTFEnabledNotificationRoutine(pcl::image_notification_routine) {}
void InterfaceDefinitionContext::SetImageSTFDisabledNotificationRoutine(pcl::image_notification_routine) {}
void InterfaceDefinitionContext::SetImageSTFUpdatedNotificationRoutine(pcl::image_notification_routine) {}
void InterfaceDefinitionContext::SetImageRGBWSUpdatedNotificationRoutine(pcl::image_notification_routine) {}
void InterfaceDefinitionContext::SetImageCMEnabledNotificationRoutine(pcl::image_notification_routine) {}
void InterfaceDefinitionContext::SetImageCMDisabledNotificationRoutine(pcl::image_notification_routine) {}
void InterfaceDefinitionContext::SetImageCMUpdatedNotificationRoutine(pcl::image_notification_routine) {}
void InterfaceDefinitionContext::SetImageSavedNotificationRoutine(pcl::image_notification_routine) {}

void InterfaceDefinitionContext::SetMaskUpdatedNotificationRoutine(pcl::image_notification_routine) {}
void InterfaceDefinitionContext::SetMaskEnabledNotificationRoutine(pcl::image_notification_routine) {}
void InterfaceDefinitionContext::SetMaskDisabledNotificationRoutine(pcl::image_notification_routine) {}
void InterfaceDefinitionContext::SetMaskShownNotificationRoutine(pcl::image_notification_routine) {}
void InterfaceDefinitionContext::SetMaskHiddenNotificationRoutine(pcl::image_notification_routine) {}

void InterfaceDefinitionContext::SetTransparencyHiddenNotificationRoutine(pcl::image_notification_routine) {}
void InterfaceDefinitionContext::SetTransparencyModeUpdatedNotificationRoutine(pcl::image_notification_routine) {}

void InterfaceDefinitionContext::SetViewPropertyUpdatedNotificationRoutine(pcl::view_property_notification_routine) {}
void InterfaceDefinitionContext::SetViewPropertyDeletedNotificationRoutine(pcl::view_property_notification_routine) {}

void InterfaceDefinitionContext::SetBeginReadoutNotificationRoutine(pcl::image_notification_routine) {}
void InterfaceDefinitionContext::SetUpdateReadoutNotificationRoutine(pcl::readout_notification_routine) {}
void InterfaceDefinitionContext::SetEndReadoutNotificationRoutine(pcl::image_notification_routine) {}

void InterfaceDefinitionContext::SetProcessCreatedNotificationRoutine(pcl::process_notification_routine) {}
void InterfaceDefinitionContext::SetProcessUpdatedNotificationRoutine(pcl::process_notification_routine) {}
void InterfaceDefinitionContext::SetProcessDeletedNotificationRoutine(pcl::process_notification_routine) {}
void InterfaceDefinitionContext::SetProcessSavedNotificationRoutine(pcl::process_notification_routine) {}

void InterfaceDefinitionContext::SetRealTimePreviewOwnerChangeNotificationRoutine(pcl::interface_notification_routine) {}
void InterfaceDefinitionContext::SetRealTimePreviewLUTUpdatedNotificationRoutine(pcl::lut_notification_routine) {}
void InterfaceDefinitionContext::SetRealTimePreviewGenerationStartNotificationRoutine(pcl::global_notification_routine) {}
void InterfaceDefinitionContext::SetRealTimePreviewGenerationFinishNotificationRoutine(pcl::global_notification_routine) {}

void InterfaceDefinitionContext::SetGlobalRGBWSUpdatedNotificationRoutine(pcl::global_notification_routine) {}
void InterfaceDefinitionContext::SetGlobalCMEnabledNotificationRoutine(pcl::global_notification_routine) {}
void InterfaceDefinitionContext::SetGlobalCMDisabledNotificationRoutine(pcl::global_notification_routine) {}
void InterfaceDefinitionContext::SetGlobalCMUpdatedNotificationRoutine(pcl::global_notification_routine) {}
void InterfaceDefinitionContext::SetReadoutOptionsUpdatedNotificationRoutine(pcl::global_notification_routine) {}
void InterfaceDefinitionContext::SetGlobalPreferencesUpdatedNotificationRoutine(pcl::global_notification_routine) {}
void InterfaceDefinitionContext::SetGlobalFiltersUpdatedNotificationRoutine(pcl::global_notification_routine) {}

// =============================================================
// ImageWindowContext - Missing Stub
// =============================================================

api_bool ImageWindowContext::TerminateDynamicSession(uint32 flags)
{
    logf("[Mock] ImageWindowContext::TerminateDynamicSession");
    return api_true;
}

static std::map<std::string, void*> g_function_map;

static void* unimplemented_function(void) {
        logf("Called unimplemented function");
        abort();
        return nullptr;
}

void* mock_function_resolver(const char* name) {
    if (!name) return nullptr;
    
    std::string func_name = name;
    logf("Resolving function: %s", name);
    
    auto it = g_function_map.find(func_name);
    if (it != g_function_map.end()) {
      logf("Found implementation for: %s", name);
        return it->second;
    }
    
    // Create a default handler for missing functions
    logf("No implementation found for: %s", name);
    
    // Store this handler so we don't create a new one each time
    g_function_map[func_name] = (void*)unimplemented_function;
    
    return g_function_map[func_name];
}

function_resolver GetMockFunctionResolver() {
    return mock_function_resolver;
}

// =============================================================
// CursorContext
// =============================================================

void* CursorContext::CloneCursor(void* handle, const void* cursor)
{
    // Return the same cursor (no actual cloning in mock)
    return const_cast<void*>(cursor);
}

// =============================================================
// DialogContext
// =============================================================

control_handle DialogContext::CreateDialog(void* handle, void* parent, 
                                          control_handle control, unsigned int flags)
{
    // Return a dummy dialog handle
    static int dummyDialog = 0;
    return (control_handle)&dummyDialog;
}

void DialogContext::ReturnDialog(control_handle control, int result)
{
    // No-op in mock
}

int32 DialogContext::ExecuteDialog(control_handle control)
{
    // Always return "accepted" (1) in mock
    return 1;
}

// =============================================================
// ControlContext
// =============================================================

void ControlContext::SetControlCursor(control_handle control, const void* cursor)
{
    // No-op in mock
}

api_bool ControlContext::SetViewDragEventRoutine(control_handle control, control_handle receiver,
                                                 pcl::view_drag_event_handler handler)
{
    // No-op in mock - drag/drop not supported
    return api_true;
}

api_bool ControlContext::SetViewDropEventRoutine(control_handle control, control_handle receiver,
                                                 pcl::view_drag_event_handler handler)
{
    // No-op in mock - drag/drop not supported
    return api_true;
}

// =============================================================
// ViewListContext
// =============================================================

control_handle ViewListContext::CreateViewList(void* handle, void* parent, 
                                               control_handle control, unsigned int flags)
{
    // Return a dummy view list handle
    static int dummyViewList = 0;
    return (control_handle)&dummyViewList;
}

void ViewListContext::RegenerateViewList(control_handle control, 
                                         unsigned int currentView,
                                         unsigned int mainViews, 
                                         unsigned int previews)
{
    // No-op in mock
}

void ViewListContext::SetViewListCurrentView(control_handle control, void* view)
{
    // No-op in mock
}

api_bool ViewListContext::SetViewListViewSelectedEventRoutine(
    control_handle control, 
    control_handle receiver,
    pcl::view_event_routine handler)
{
    // No-op in mock
    return api_true;
}

// =============================================================
// NumericalContext
// =============================================================

void* NumericalContext::FFTCreateComplexTransformF(size_t length)
{
    // Return a dummy FFT handle
    static int dummyFFT = 0;
    return &dummyFFT;
}

void* NumericalContext::FFTCreateComplexInverseTransformF(size_t length)
{
    // Return a dummy inverse FFT handle
    static int dummyIFFT = 0;
    return &dummyIFFT;
}

api_bool NumericalContext::FFTComplexTransformF(void* handle, void* out, const void* in)
{
    // No-op in mock - FFT not actually performed
    return api_true;
}

api_bool NumericalContext::FFTComplexInverseTransformF(void* handle, void* out, const void* in)
{
    // No-op in mock - inverse FFT not actually performed
    return api_true;
}

size_t NumericalContext::FFTComplexOptimizedLengthD(size_t length)
{
    // Same as float version
    return FFTComplexOptimizedLengthF(length);
}

// =============================================================
// ImageWindowContext
// =============================================================

void ImageWindowContext::AddImageWindowKeyword(window_handle handle, const char* name, 
                                               const char* value, const char* comment)
{
    // No-op in mock
}

void ImageWindowContext::ResetImageWindowKeywords(window_handle handle)
{
    // No-op in mock
}

int32 ImageWindowContext::GetImageWindowKeywordCount(const_window_handle handle)
{
    // No keywords in mock
    return 0;
}

void ImageWindowContext::GetImageWindowKeyword(const_window_handle handle, int index,
                                               char* name, size_t nameSize,
                                               char* value, size_t valueSize,
                                               char* comment, size_t commentSize)
{
    // No keywords to retrieve - just return (void return type)
}

// PCLMockAPI_Gaia.cpp
// Additional mock implementations for Gaia module
// Add these to the END of your existing PCLMockAPI.cpp

// Note: Make sure PCLMockAPI.cpp includes:
// #include <QTextEdit>
// #include <QComboBox>

// =============================================================
// FontContext
// =============================================================

int32 FontContext::GetCharPixelWidth(const void* handle, int ch)
{
    // Return approximate width for any character
    return 8; // Average character width in pixels
}

// =============================================================
// DialogContext - Additional
// =============================================================

api_bool DialogContext::ExecuteSaveFileDialog(
    char16_type* path,
    const char16_type* initialPath,
    const char16_type* caption,
    const char16_type* filters,
    const char16_type* defaultExt,
    uint32 flags)
{
    // Mock: return false (user cancelled)
    return api_false;
}

api_bool DialogContext::SetReturnDialogEventRoutine(
    control_handle control,
    control_handle receiver,
    void (*handler)(control_handle, control_handle, int))
{
    // No-op in mock
  return api_true;
}

// =============================================================
// GlobalContext - Settings
// =============================================================

api_bool GlobalContext::DeleteSettingsItem(void* handle, const char* key, uint32 flags)
{
    // No-op in mock - settings not persisted
  return api_true;
}

api_bool GlobalContext::ReadSettingsString(
    void* handle,
    char16_type** value,
    const char* key,
    uint32 flags)
{
    // Mock: no settings available
    *value = nullptr;
    return api_false;
}

api_bool GlobalContext::WriteSettingsString(
    void* handle,
    const char16_type* value,
    const char* key,
    uint32 flags)
{
    // No-op in mock - settings not persisted
    return api_true;
}

// =============================================================
// ControlContext - Additional
// =============================================================

void ControlContext::RestyleControl(control_handle control)
{
    // No-op in mock
}

void ControlContext::SetControlStyleSheet(control_handle control, const char16_type* css)
{
    // No-op in mock - stylesheets not supported
}

// =============================================================
// TextBoxContext
// =============================================================

control_handle TextBoxContext::CreateTextBox(
    void* handle,
    void* parent,
    const char16_type* text,
    control_handle control,
    uint32 flags)
{
    // Create a mock QTextEdit
    qDebug() << "[Mock] createControl<QTextEdit>";
    
    MockBase* base = new MockBase();
    base->isSizer = false;
    base->widget = new QTextEdit(nullptr);
    
    if (text)
        base->widget->setProperty("text", QString::fromUtf16(text));
    
    g_topLevelWidgets.append(base);
    return (control_handle)base;
}

void TextBoxContext::SetTextBoxText(control_handle control, const char16_type* text)
{
    MockBase* base = reinterpret_cast<MockBase*>(control);
    if (base && base->widget)
    {
        QTextEdit* textBox = qobject_cast<QTextEdit*>(base->widget);
        if (textBox && text)
            textBox->setPlainText(QString::fromUtf16(text));
    }
}

void TextBoxContext::InsertTextBoxText(control_handle control, const char16_type* text)
{
    MockBase* base = reinterpret_cast<MockBase*>(control);
    if (base && base->widget)
    {
        QTextEdit* textBox = qobject_cast<QTextEdit*>(base->widget);
        if (textBox && text)
            textBox->insertPlainText(QString::fromUtf16(text));
    }
}

void TextBoxContext::SetTextBoxReadOnly(control_handle control, uint32 readOnly)
{
    MockBase* base = reinterpret_cast<MockBase*>(control);
    if (base && base->widget)
    {
        QTextEdit* textBox = qobject_cast<QTextEdit*>(base->widget);
        if (textBox)
            textBox->setReadOnly(readOnly != 0);
    }
}

// =============================================================
// ComboBoxContext - Additional
// =============================================================

int32 ComboBoxContext::GetComboBoxCurrentItem(const_control_handle control)
{
    const MockBase* base = reinterpret_cast<const MockBase*>(control);
    if (base && base->widget)
    {
        const QComboBox* combo = qobject_cast<const QComboBox*>(base->widget);
        if (combo)
            return combo->currentIndex();
    }
    return -1;
}

// =============================================================
// NetworkTransferContext
// =============================================================

void* NetworkTransferContext::CreateNetworkTransfer(void* handle, void* parent)
{
    // Return a dummy network transfer handle
    static int dummyTransfer = 0;
    return &dummyTransfer;
}

api_bool NetworkTransferContext::SetNetworkTransferURL(
    void* handle,
    const char16_type* url,
    const char16_type* username,
    const char16_type* password)
{
    // No-op in mock - network transfers not supported
    return api_true;
}

void NetworkTransferContext::GetNetworkTransferTotalSpeed(const void* handle, double* speed)
{
    // Mock: no transfer happening
    if (speed)
        *speed = 0.0;
}

api_bool NetworkTransferContext::PerformNetworkTransferDownload(void* handle)
{
    // Mock: download always fails
    return api_false;
}

fsize_type NetworkTransferContext::GetNetworkTransferBytesTransferred(const void* handle)
{
    // Mock: no bytes transferred
    return 0;
}

api_bool NetworkTransferContext::GetNetworkTransferErrorInformation(
    const void* handle,
    char16_type* errorText,
    size_type* errorTextSize)
{
    // Mock: return generic error
    if (errorText && errorTextSize)
    {
        QString error = "Network transfers not supported in mock environment";
        memcpy(errorText, error.utf16(), *errorTextSize * sizeof(char16_type));
        *errorTextSize = error.length();
    }
    return api_true;
}

api_bool NetworkTransferContext::SetNetworkTransferDownloadEventRoutine(
    void* handle,
    void* receiver,
    uint32 (*handler)(void*, control_handle, const void*, int64))
{
    // No-op in mock - events not supported
    return api_false;
}

api_bool NetworkTransferContext::SetNetworkTransferProgressEventRoutine(
    void* handle,
    void* receiver,
    uint32 (*handler)(void*, control_handle, int64, int64, int64, int64))
{
    // No-op in mock - events not supported
    return api_false;
}

// Image stubs
// PCLMockAPI_Image.cpp
// Additional mock implementations for Image module
// Add these to the END of your existing PCLMockAPI.cpp

// Note: This module uses advanced graphics features - all stubbed for UI export

// =============================================================
// PenContext
// =============================================================

void* PenContext::ClonePen(void* handle, const void* pen)
{
    return const_cast<void*>(pen);
}

void* PenContext::CreatePen(void* handle, uint32 color, float width, 
                            int style, int cap, int join)
{
    static int dummyPen = 0;
    return &dummyPen;
}

// =============================================================
// BrushContext
// =============================================================

void* BrushContext::CloneBrush(void* handle, const void* brush)
{
    return const_cast<void*>(brush);
}

void* BrushContext::CreateBrush(void* handle, uint32 color, int style)
{
    static int dummyBrush = 0;
    return &dummyBrush;
}

void* BrushContext::CreateBitmapBrush(void* handle, const void* bitmap)
{
    static int dummyBitmapBrush = 0;
    return &dummyBitmapBrush;
}

// =============================================================
// FontContext - Additional
// =============================================================

int32 FontContext::GetFontAscent(const void* handle)
{
    return 12; // Mock ascent value
}

void FontContext::SetFontWeight(void* handle, int weight)
{
    // No-op
}

// =============================================================
// FrameContext
// =============================================================

void FrameContext::SetFrameStyle(control_handle control, int style)
{
    // No-op - frame styling not supported in mock
}

void FrameContext::SetFrameLineWidth(control_handle control, int width)
{
    // No-op
}

// =============================================================
// LabelContext - Additional
// =============================================================

void LabelContext::SetLabelWordWrappingEnabled(control_handle control, uint32 enabled)
{
    MockBase* base = reinterpret_cast<MockBase*>(control);
    if (base && base->widget)
    {
        QLabel* label = qobject_cast<QLabel*>(base->widget);
        if (label)
            label->setWordWrap(enabled != 0);
    }
}

// =============================================================
// ActionContext
// =============================================================

void ActionContext::SetActionToolTip(void* handle, const char16_type* tooltip)
{
    // No-op
}

void* ActionContext::CreateActionSVGFile(void* handle, void* parent,
                                         const char16_type* menuText,
                                         const char16_type* iconSVGFile,
                                         const char16_type* toolBar,
                                         uint32 flags)
{
    static int dummyAction = 0;
    return &dummyAction;
}

void ActionContext::SetActionAccelerator(void* handle, int keyCode, int modifiers)
{
    // No-op
}

api_bool ActionContext::SetActionExecutionRoutine(void* handle, void (*callback)(void*))
{
    // No-op
    return api_true;
}

api_bool ActionContext::SetActionStateQueryRoutine(void* handle,
                                                   uint32 (*callback)(const void*, const void*))
{
    // No-op
    return api_true;
}

// =============================================================
// DialogContext - Additional
// =============================================================

api_bool DialogContext::SetExecuteDialogEventRoutine(control_handle control, control_handle receiver,
                                                     void (*handler)(control_handle, control_handle))
{
    // No-op
    return api_true;
}

// =============================================================
// GlobalContext - Additional
// =============================================================

api_bool GlobalContext::GetGlobalFont(const char* id, char16_type* face,
                                     size_type* faceLen, int* size)
{
    if (face && faceLen && *faceLen > 0)
    {
        QString defaultFont = "Helvetica";
        // Convert QString to char16_type*
        for (int i = 0; i < defaultFont.length() && i < (int)*faceLen - 1; i++)
            face[i] = defaultFont[i].unicode();
        face[defaultFont.length()] = 0;
        *faceLen = defaultFont.length();
    }
    if (size) *size = 12;
    return api_true;
}

api_bool GlobalContext::ReadSettingsFlag(void* handle, uint32* value,
                                        const char* key, uint32 flags)
{
    if (value) *value = 0;
    return api_false;
}

api_bool GlobalContext::WriteSettingsFlag(void* handle, uint32 value,
                                         const char* key, uint32 flags)
{
    // No-op
    return api_true;
}

// =============================================================
// ControlContext - Additional
// =============================================================

void ControlContext::UpdateControl(control_handle control)
{
    MockBase* base = reinterpret_cast<MockBase*>(control);
    if (base && base->widget)
        base->widget->update();
}

void ControlContext::ActivateWindow(control_handle control)
{
    MockBase* base = reinterpret_cast<MockBase*>(control);
    if (base && base->widget)
        base->widget->activateWindow();
}

api_bool ControlContext::GetTrackViewActive(const_control_handle control)
{
    return api_false;
}

void ControlContext::SetTrackViewActive(control_handle control, uint32 active)
{
    // No-op
}

api_bool ControlContext::SetPaintEventRoutine(control_handle control, control_handle receiver,
                                              uint32 (*handler)(control_handle, control_handle,
                                                               int, int, int, int))
{
    // No-op - painting not supported in mock
    return api_true;
}

// =============================================================
// SpinBoxContext - Additional
// =============================================================

void SpinBoxContext::GetSpinBoxRange(const_control_handle control, int* min, int* max)
{
    const MockBase* base = reinterpret_cast<const MockBase*>(control);
    if (base && base->widget)
    {
        const QSpinBox* spin = qobject_cast<const QSpinBox*>(base->widget);
        if (spin)
        {
            if (min) *min = spin->minimum();
            if (max) *max = spin->maximum();
        }
    }
}

// =============================================================
// TreeBoxContext
// =============================================================

void TreeBoxContext::SetTreeBoxIconSize(control_handle control, int width, int height)
{
    // No-op
}

api_bool TreeBoxContext::GetTreeBoxHeaderText(const_control_handle control, int section,
                                              char16_type* text, size_type* len)
{
    if (len) *len = 0;
    return api_false;
}

void TreeBoxContext::SetTreeBoxHeaderIcon(control_handle control, int section,
                                         const void* icon)
{
    // No-op
}

void TreeBoxContext::SetTreeBoxHeaderText(control_handle control, int section,
                                         const char16_type* text)
{
    // No-op
}

int32 TreeBoxContext::GetTreeBoxColumnCount(const_control_handle control)
{
    return 1; // Mock: one column
}

void TreeBoxContext::SetTreeBoxNodeIntoView(control_handle control, void* node)
{
    // No-op
}

api_bool TreeBoxContext::GetTreeBoxHeaderVisible(const_control_handle control)
{
    return api_true;
}

api_bool TreeBoxContext::GetTreeBoxSelectedNodes(const_control_handle control,
                                                 void** nodes, size_type* count)
{
    if (count) *count = 0;
    return api_false;
}

void TreeBoxContext::SetTreeBoxHeaderAlignment(control_handle control,
                                               int section, int alignment)
{
    // No-op
}

// =============================================================
// GraphicsContext - Drawing Operations
// =============================================================

void GraphicsContext::StrokeRectD(void* handle, double x, double y,
                                 double w, double h, const void* pen)
{
    // No-op - graphics not rendered in mock
}

void* GraphicsContext::CreateGraphics(void* handle)
{
    static int dummyGraphics = 0;
    return &dummyGraphics;
}

void GraphicsContext::SetGraphicsPen(void* handle, const void* pen)
{
    // No-op
}

void GraphicsContext::StrokeEllipseD(void* handle, double x, double y,
                                    double w, double h, const void* pen)
{
    // No-op
}

void GraphicsContext::SetGraphicsBrush(void* handle, const void* brush)
{
    // No-op
}

api_bool GraphicsContext::BeginControlPaint(void* handle, control_handle control)
{
    // No-op
    return api_true;
}

void GraphicsContext::EnableGraphicsAntialiasing(void* handle, uint32 enabled)
{
    // No-op
}

void GraphicsContext::ResetGraphicsTransformation(void* handle)
{
    // No-op
}

void GraphicsContext::RotateGraphicsTransformation(void* handle, double angle)
{
    // No-op
}

void GraphicsContext::SetGraphicsCompositionOperator(void* handle, int op)
{
    // No-op
}

void GraphicsContext::TranslateGraphicsTransformation(void* handle, double dx, double dy)
{
    // No-op
}

void GraphicsContext::DrawRect(void* handle, int x, int y, int w, int h)
{
    // No-op
}

void GraphicsContext::DrawLineD(void* handle, double x1, double y1, double x2, double y2)
{
    // No-op
}

void GraphicsContext::FillRectD(void* handle, double x, double y, double w, double h,
                               const void* brush)
{
    // No-op
}

// =============================================================
// GroupBoxContext
// =============================================================

control_handle GroupBoxContext::CreateGroupBox(void* handle, void* parent,
                                               const char16_type* title,
                                               control_handle control, uint32 flags)
{
    qDebug() << "[Mock] createControl<QGroupBox>";
    
    MockBase* base = new MockBase();
    base->isSizer = false;
    base->widget = new QGroupBox(nullptr);
    
    if (title)
        static_cast<QGroupBox*>(base->widget)->setTitle(QString::fromUtf16(title));
    
    g_topLevelWidgets.append(base);
    return (control_handle)base;
}

void GroupBoxContext::SetGroupBoxTitle(control_handle control, const char16_type* title)
{
    MockBase* base = reinterpret_cast<MockBase*>(control);
    if (base && base->widget)
    {
        QGroupBox* group = qobject_cast<QGroupBox*>(base->widget);
        if (group && title)
            group->setTitle(QString::fromUtf16(title));
    }
}

void GroupBoxContext::SetGroupBoxChecked(control_handle control, uint32 checked)
{
    MockBase* base = reinterpret_cast<MockBase*>(control);
    if (base && base->widget)
    {
        QGroupBox* group = qobject_cast<QGroupBox*>(base->widget);
        if (group)
            group->setChecked(checked != 0);
    }
}

void GroupBoxContext::SetGroupBoxCheckable(control_handle control, uint32 checkable)
{
    MockBase* base = reinterpret_cast<MockBase*>(control);
    if (base && base->widget)
    {
        QGroupBox* group = qobject_cast<QGroupBox*>(base->widget);
        if (group)
            group->setCheckable(checkable != 0);
    }
}

api_bool GroupBoxContext::SetGroupBoxCheckEventRoutine(control_handle control, void* receiver,
                                                       void (*handler)(control_handle, control_handle, int))
{
    // No-op
    return api_true;
}

// =============================================================
// ViewListContext - Additional
// =============================================================

void* ViewListContext::GetViewListCurrentView(const_control_handle control)
{
    return nullptr; // No views in mock
}

// =============================================================
// NumericalContext - Surface Spline
// =============================================================

api_bool NumericalContext::SurfaceSplineCreateD(void** handle, int type, double smoothness,
                                               uint32 flags, const double* x, const double* y,
                                               const double* z, int count, int order,
                                               float tolerance, const float* weights)
{
    static int dummySpline = 0;
    if (handle) *handle = &dummySpline;
    return api_true;
}

api_bool NumericalContext::SurfaceSplineDestroy(void* handle)
{
    // No-op
    return api_true;
}

api_bool NumericalContext::SurfaceSplineEvaluate(const void* handle, double* result,
                                                  double x, double y)
{
    if (result) *result = 0.0; // Mock: return zero
    return api_true;
}

api_bool NumericalContext::SurfaceSplineDuplicate(void** clone, const void* handle)
{
    static int dummyClone = 0;
    if (clone) *clone = &dummyClone;
    return api_true;
}

api_bool NumericalContext::SurfaceSplineDeserialize(void** handle, const char* data,
                                                   size_type size, uint32 flags)
{
    static int dummySpline = 0;
    if (handle) *handle = &dummySpline;
    return api_true;
}

// =============================================================
// ImageWindowContext - Image Window Operations
// =============================================================

void ImageWindowContext::ImageToViewportD(const_window_handle handle, double* x, double* y)
{
    // No-op - coordinates unchanged in mock
}

void ImageWindowContext::UpdateViewportRect(window_handle handle, int x, int y, int w, int h)
{
    // No-op
}

void ImageWindowContext::EndViewportSelection(window_handle handle)
{
    // No-op
}

api_bool ImageWindowContext::GetViewportSelection(const_window_handle handle, int* x0, int* y0,
                                                  int* x1, int* y1, uint32* flags)
{
    if (x0) *x0 = 0;
    if (y0) *y0 = 0;
    if (x1) *x1 = 0;
    if (y1) *y1 = 0;
    if (flags) *flags = 0;
    return api_false;
}

void ImageWindowContext::BeginViewportSelection(window_handle handle, int x, int y, uint32 flags)
{
    // No-op
}

void ImageWindowContext::ImageScalarToViewportD(const_window_handle handle, double* value)
{
    // No-op - value unchanged
}

void ImageWindowContext::SetImageWindowViewport(window_handle handle, double cx, double cy, int zoom)
{
    // No-op
}

void ImageWindowContext::BringImageWindowToFront(window_handle handle)
{
    // No-op
}

void ImageWindowContext::ModifyViewportSelection(window_handle handle, int x, int y, uint32 flags)
{
    // No-op
}

int32 ImageWindowContext::GetImageWindowDisplayChannel(const_window_handle handle)
{
    return 0; // Red channel
}

api_bool ImageWindowContext::GetImageWindowDisplayPixelRatio(const_window_handle handle, double* ratio)
{
    if (ratio) *ratio = 1.0;
    return api_true;
}

void ImageWindowContext::GetImageWindowVisibleViewportRect(const_window_handle handle,
                                                           int* x0, int* y0, int* x1, int* y1)
{
    if (x0) *x0 = 0;
    if (y0) *y0 = 0;
    if (x1) *x1 = 1024;
    if (y1) *y1 = 768;
}

api_bool ImageWindowContext::GetImageWindowHasAstrometricSolution(const_window_handle handle)
{
    return api_false; // No astrometry in mock
}

// =============================================================
// DialogContext - Additional Stubs
// =============================================================

api_bool DialogContext::ExecuteGetDirectoryDialog(char16_type* output_path,
                                                   const char16_type* initial_path,
                                                   const char16_type* caption)
{
    logf("[Mock] DialogContext::ExecuteGetDirectoryDialog");
    
    // Mock implementation: return empty path or copy initial path
    if (output_path)
    {
        if (initial_path)
        {
            // Copy initial path to output
            size_t len = 0;
            while (initial_path[len] != 0 && len < 1024)
            {
                output_path[len] = initial_path[len];
                len++;
            }
            output_path[len] = 0;
        }
        else
        {
            output_path[0] = 0;  // Empty string
        }
    }
    return api_true;
}

// =============================================================
// GlobalContext - Additional Stubs
// =============================================================

api_bool GlobalContext::ExecuteCommand(api_handle handle, console_handle console,
                                       const char16_type* command_string)
{
    logf("[Mock] GlobalContext::ExecuteCommand");
    // Mock implementation: command executes silently with no result
    return api_true;
}

// =============================================================
// ModuleContext - Additional Stubs
// =============================================================

api_bool ModuleContext::EvaluateScript(api_handle handle, api_property_value* result,
                                       const char16_type* script_text,
                                       const char* language)
{
    logf("[Mock] ModuleContext::EvaluateScript (language=%s)", language ? language : "null");
    // Mock implementation: script evaluates to null/empty
    if (result)
    {
        result->type = 0;  // Assuming 0 = undefined/null
        // Don't try to access .ptr member - it may not exist in the union
    }
    return api_true;
}

// =============================================================
// ComboBoxContext - Additional Stubs
// =============================================================

void ComboBoxContext::RemoveComboBoxItem(control_handle control, int item_index)
{
    logf("[Mock] ComboBoxContext::RemoveComboBoxItem (index=%d)", item_index);
    
    QWidget* w = widgetFromHandle(control);
    QComboBox* combo = qobject_cast<QComboBox*>(w);
    if (combo && item_index >= 0 && item_index < combo->count())
    {
        combo->removeItem(item_index);
    }
}

void ComboBoxContext::SetComboBoxIconSize(control_handle control, int width, int height)
{
    logf("[Mock] ComboBoxContext::SetComboBoxIconSize (%d x %d)", width, height);
    
    QWidget* w = widgetFromHandle(control);
    QComboBox* combo = qobject_cast<QComboBox*>(w);
    if (combo)
    {
        combo->setIconSize(QSize(width, height));
    }
}

api_bool ComboBoxContext::SetComboBoxItemHighlightedEventRoutine(
    control_handle control,
    api_handle receiver,
    pcl::value_event_routine callback)
{
    logf("[Mock] ComboBoxContext::SetComboBoxItemHighlightedEventRoutine");
    
    MockBase* base = get(control);
    if (base && base->widget)
    {
        QComboBox* combo = qobject_cast<QComboBox*>(base->widget);
        if (combo)
        {
            // Connect Qt signal to invoke the callback
            QObject::connect(combo, QOverload<int>::of(&QComboBox::highlighted),
                [base, callback](int index)
                {
                    if (callback)
                        callback(base->pcl_handle, base->pcl_handle, index);
                });
        }
    }
    return api_true;
}

// =============================================================
// GraphicsContext - Additional Drawing Stubs
// =============================================================

void GraphicsContext::FillEllipseD(void* handle, double x, double y,
                                   double width, double height,
                                   const void* brush)
{
    logf("[Mock] GraphicsContext::FillEllipseD (%.1f, %.1f, %.1f x %.1f)",
         x, y, width, height);
    // No-op - graphics not rendered in mock
}

void GraphicsContext::FillPolygonD(void* handle, const double* points_array,
                                   size_type num_points, int fill_rule,
                                   const void* brush)
{
    logf("[Mock] GraphicsContext::FillPolygonD (%zu points, rule=%d)",
         (size_t)num_points, fill_rule);
    // No-op - graphics not rendered in mock
}

void GraphicsContext::StrokePolygonD(void* handle, const double* points_array,
                                     size_type num_points, int fill_rule,
                                     const void* pen)
{
    logf("[Mock] GraphicsContext::StrokePolygonD (%zu points, rule=%d)",
         (size_t)num_points, fill_rule);
    // No-op - graphics not rendered in mock
}

void GraphicsContext::DrawTiledBitmap(void* handle, int x, int y,
                                      int width, int height,
                                      const void* bitmap,
                                      int tile_x, int tile_y)
{
    logf("[Mock] GraphicsContext::DrawTiledBitmap (%d, %d, %d x %d, tile: %d, %d)",
         x, y, width, height, tile_x, tile_y);
    // No-op - graphics not rendered in mock
}

api_bool GraphicsContext::BeginBitmapPaint(graphics_handle handle, bitmap_handle bitmap)
{
    logf("[Mock] GraphicsContext::BeginBitmapPaint");
    // No-op - initialize graphics context for bitmap painting
    return api_true;
}

void GraphicsContext::DrawLine(void* handle, int x1, int y1, int x2, int y2)
{
    logf("[Mock] GraphicsContext::DrawLine (%d, %d) -> (%d, %d)",
         x1, y1, x2, y2);
    // No-op - graphics not rendered in mock
}

// =============================================================
// ImageWindowContext - Additional Stubs
// =============================================================

api_bool ImageWindowContext::CelestialToImage(const_window_handle window_context,
                                              double* ra, double* dec)
{
    logf("[Mock] ImageWindowContext::CelestialToImage (%.6f, %.6f)",
         ra ? *ra : 0.0, dec ? *dec : 0.0);
    
    // Mock implementation: leave coordinates unchanged
    // In a real implementation, this would transform celestial coordinates
    // (RA/Dec) to image pixel coordinates using astrometric solution
    return api_true;
}

// =============================================================
// END OF APPENDED STUBS
// =============================================================

// =============================================================
// TimerContext - Additional Stubs
// =============================================================

timer_handle TimerContext::CreateTimer(api_handle module, api_handle receiver,
                                       uint32 flags)
{
    logf("[Mock] TimerContext::CreateTimer");
    
    // Create a QTimer as the underlying implementation
    QTimer* qtTimer = new QTimer();
    
    // Create mock wrapper
    auto base = std::make_unique<MockBase>();
    base->timer_api_handle = receiver;  // Store the receiver control
    base->isSizer = false;
    base->isTreeNode = false;
    base->qtTimer = qtTimer;  // Store QTimer pointer
    
    timer_handle handle = reinterpret_cast<timer_handle>(base.get());
    g_objects[handle] = std::move(base);
    
    return handle;
}

void TimerContext::SetTimerInterval(timer_handle handle, uint32 milliseconds)
{
    logf("[Mock] TimerContext::SetTimerInterval (%u ms)", milliseconds);
    
    MockBase* base = get(handle);
    if (base && base->qtTimer)
    {
        base->qtTimer->setInterval(milliseconds);
    }
}

void TimerContext::SetTimerSingleShot(timer_handle handle, uint32 singleShot)
{
    logf("[Mock] TimerContext::SetTimerSingleShot (%s)", singleShot ? "true" : "false");
    
    MockBase* base = get(handle);
    if (base && base->qtTimer)
    {
        base->qtTimer->setSingleShot(singleShot != 0);
    }
}

api_bool TimerContext::SetTimerNotifyEventRoutine(timer_handle handle, api_handle receiver,
                                              void (*callback)(timer_handle, pcl::Control*))
{
    logf("[Mock] TimerContext::SetTimerNotifyEventRoutine");
    
    MockBase* base = get(handle);
    if (base && base->qtTimer)
    {
        // Store the callback and receiver
        base->onTimerNotify = callback;
        base->timerReceiver = receiver;
        
        // Connect QTimer timeout signal to invoke the callback
        QObject::connect(base->qtTimer, &QTimer::timeout,
            [base, callback, receiver]()
            {
                if (callback)
                    callback(reinterpret_cast<timer_handle>(base), 
                           reinterpret_cast<pcl::Control*>(receiver));
            });
    }
    return api_true;
}

// =============================================================
// TimerContext - Additional Stub
// =============================================================

api_bool TimerContext::IsTimerActive(const_timer_handle handle)
{
    logf("[Mock] TimerContext::IsTimerActive");
    
    MockBase* base = get(const_cast<timer_handle>(handle));
    if (base && base->qtTimer)
    {
        return base->qtTimer->isActive() ? api_true : api_false;
    }
    
    return api_false;
}

// =============================================================
// END OF TIMER STUB
// =============================================================

// =============================================================
// GlobalContext - Additional Stubs
// =============================================================

void GlobalContext::BroadcastImageUpdated(const_view_handle view, const_bitmap_handle bitmap)
{
    logf("[Mock] GlobalContext::BroadcastImageUpdated");
    // Mock implementation: broadcast image update notification
    // In real implementation, this would notify all interested listeners
}

// =============================================================
// GraphicsContext - Additional Drawing Stubs
// =============================================================

void GraphicsContext::DrawEllipseD(void* handle, double x, double y,
                                   double width, double height)
{
    logf("[Mock] GraphicsContext::DrawEllipseD (%.1f, %.1f, %.1f x %.1f)",
         x, y, width, height);
    // No-op - graphics not rendered in mock
}

void GraphicsContext::DrawScaledBitmap(void* handle, int x, int y,
                                       int width, int height,
                                       const void* bitmap)
{
    logf("[Mock] GraphicsContext::DrawScaledBitmap (%d, %d, %d x %d)",
         x, y, width, height);
    // No-op - graphics not rendered in mock
}

void GraphicsContext::EnableGraphicsSmoothInterpolation(void* handle, uint32 enabled)
{
    logf("[Mock] GraphicsContext::EnableGraphicsSmoothInterpolation (%s)",
         enabled ? "enabled" : "disabled");
    // No-op - graphics not rendered in mock
}

void GraphicsContext::DrawRectD(void* handle, double x, double y,
                                double width, double height)
{
    logf("[Mock] GraphicsContext::DrawRectD (%.1f, %.1f, %.1f x %.1f)",
         x, y, width, height);
    // No-op - graphics not rendered in mock
}

// =============================================================
// ImageWindowContext - Additional Stubs
// =============================================================

void ImageWindowContext::ImageToViewport(const_window_handle window_context,
                                         int* x, int* y)
{
    logf("[Mock] ImageWindowContext::ImageToViewport (%d, %d)",
         x ? *x : 0, y ? *y : 0);
    // Mock implementation: coordinates unchanged (1:1 mapping)
    // In real implementation, this would transform image coordinates to viewport
}

void ImageWindowContext::UpdateImageRect(window_handle window_context,
                                         double x, double y,
                                         double width, double height)
{
    logf("[Mock] ImageWindowContext::UpdateImageRect (%.1f, %.1f, %.1f x %.1f)",
         x, y, width, height);
    // Mock implementation: request viewport update for image region
}

void ImageWindowContext::RegenerateImageRect(window_handle window_context,
                                             double x, double y,
                                             double width, double height)
{
    logf("[Mock] ImageWindowContext::RegenerateImageRect (%.1f, %.1f, %.1f x %.1f)",
         x, y, width, height);
    // Mock implementation: regenerate rendering for image region
}

void ImageWindowContext::ViewportScalarToImageD(const_window_handle window_context,
                                                double* value)
{
    logf("[Mock] ImageWindowContext::ViewportScalarToImageD (%.6f)",
         value ? *value : 0.0);
    // Mock implementation: value unchanged (1:1 scale)
    // In real implementation, this would transform viewport scalar to image scale
}

// =============================================================
// END OF ADDITIONAL STUBS
// =============================================================

// =============================================================
// EditContext - Additional Stubs
// =============================================================

void EditContext::SetEditReadOnly(control_handle control, uint32 readOnly)
{
    logf("[Mock] EditContext::SetEditReadOnly (%s)", readOnly ? "true" : "false");
    
    QWidget* w = widgetFromHandle(control);
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(w);
    if (lineEdit)
    {
        lineEdit->setReadOnly(readOnly != 0);
        return;
    }
    
    QTextEdit* textEdit = qobject_cast<QTextEdit*>(w);
    if (textEdit)
    {
        textEdit->setReadOnly(readOnly != 0);
    }
}

// =============================================================
// GlobalContext - Color Profile Support
// =============================================================

api_bool GlobalContext::GetProfilesDirectory(int32 index, char16_type* path,
                                             size_type* length)
{
    logf("[Mock] GlobalContext::GetProfilesDirectory (index=%d)", index);
    
    // Mock implementation: return standard macOS ICC profile directories
    static const char* profileDirs[] = {
        "/System/Library/ColorSync/Profiles",
        "/Library/ColorSync/Profiles",
        "~/Library/ColorSync/Profiles"
    };
    
    if (index < 0 || index >= 3)
        return api_false;
    
    QString dirPath = QString::fromUtf8(profileDirs[index]);
    if (dirPath.startsWith("~/"))
    {
        dirPath.replace(0, 1, QDir::homePath());
    }
    
    if (path && length)
    {
        // Convert to UTF-16 and copy
        std::u16string u16str = dirPath.toStdU16String();
        size_type copyLen = std::min(*length - 1, u16str.length());
        std::memcpy(path, u16str.c_str(), copyLen * sizeof(char16_type));
        path[copyLen] = 0;
        *length = copyLen;
    }
    else if (length)
    {
        // Just return the required length
        *length = dirPath.length() + 1;
    }
    
    return api_true;
}

// =============================================================
// ComboBoxContext - Additional Stubs
// =============================================================

void ComboBoxContext::ClearComboBox(control_handle control)
{
    logf("[Mock] ComboBoxContext::ClearComboBox");
    
    QWidget* w = widgetFromHandle(control);
    QComboBox* combo = qobject_cast<QComboBox*>(w);
    if (combo)
    {
        combo->clear();
    }
}

// =============================================================
// ImageWindowContext - ICC Profile Support
// =============================================================

void ImageWindowContext::GetImageWindowICCProfile(const_window_handle window_context,
                                                      void* profile_handle)
{
    logf("[Mock] ImageWindowContext::GetImageWindowICCProfile");
    
    // Mock implementation: no ICC profile attached to mock image windows
    // In real implementation, this would copy the ICC profile data
}

void ImageWindowContext::SetImageWindowICCProfile(window_handle window_context,
                                                  const void* profile_handle)
{
    logf("[Mock] ImageWindowContext::SetImageWindowICCProfile");
    
    // Mock implementation: store ICC profile with image window
    // In real implementation, this would embed the profile in the image
}

void ImageWindowContext::DeleteImageWindowICCProfile(window_handle window_context)
{
    logf("[Mock] ImageWindowContext::DeleteImageWindowICCProfile");
    
    // Mock implementation: remove ICC profile from image window
    // In real implementation, this would strip the embedded profile
}

uint32 ImageWindowContext::GetImageWindowICCProfileLength(const_window_handle window_context)
{
    logf("[Mock] ImageWindowContext::GetImageWindowICCProfileLength");
    
    // Mock implementation: no ICC profile, return 0
    // In real implementation, this would return the profile data size in bytes
    return 0;
}

// =============================================================
// END OF COLOR MANAGEMENT STUBS
// =============================================================

// =============================================================
// ComboBoxContext - Text Retrieval Stub
// =============================================================

api_bool ComboBoxContext::GetComboBoxItemText(const_control_handle control, int index,
                                              char16_type* text, size_type* length)
{
    logf("[Mock] ComboBoxContext::GetComboBoxItemText (index=%d)", index);
    
    QWidget* w = widgetFromHandle(const_cast<control_handle>(control));
    QComboBox* combo = qobject_cast<QComboBox*>(w);
    
    if (!combo || index < 0 || index >= combo->count())
        return api_false;
    
    QString itemText = combo->itemText(index);
    
    if (text && length)
    {
        // Convert to UTF-16 and copy
        std::u16string u16str = itemText.toStdU16String();
        size_type copyLen = std::min(*length - 1, u16str.length());
        std::memcpy(text, u16str.c_str(), copyLen * sizeof(char16_type));
        text[copyLen] = 0;
        *length = copyLen;
    }
    else if (length)
    {
        // Just return the required length
        *length = itemText.length() + 1;
    }
    
    return api_true;
}

// =============================================================
// ImageWindowContext - Resolution and Astrometry Stubs
// =============================================================

void ImageWindowContext::GetImageWindowResolution(const_window_handle window_context,
                                                     double* xRes, double* yRes,
                                                     uint32* metric)
{
    logf("[Mock] ImageWindowContext::GetImageWindowResolution");
    
    // Mock implementation: return default 72 DPI resolution
    if (xRes)
        *xRes = 72.0;
    if (yRes)
        *yRes = 72.0;
    if (metric)
        *metric = 0;  // 0 = inches, 1 = centimeters    
}

void ImageWindowContext::SetImageWindowResolution(window_handle window_context,
                                                  double xRes, double yRes,
                                                  uint32 metric)
{
    logf("[Mock] ImageWindowContext::SetImageWindowResolution (%.2f, %.2f, %s)",
         xRes, yRes, metric ? "cm" : "inches");
    
    // Mock implementation: store resolution metadata
    // In real implementation, this would set the image resolution
}

api_bool ImageWindowContext::CopyImageWindowAstrometricSolution(window_handle target,
                                                            const_window_handle source,
                                                            uint32 notify)
{
    logf("[Mock] ImageWindowContext::CopyImageWindowAstrometricSolution (notify=%u)",
         notify);

    return api_true;
    
    // Mock implementation: copy astrometric solution from source to target
    // In real implementation, this would copy WCS (World Coordinate System) data
}

// =============================================================
// END OF VIEW PROPERTIES AND IMAGE WINDOW STUBS
// =============================================================

// =============================================================
// TimerContext - Additional Stubs
// =============================================================

api_bool TimerContext::StartTimer(timer_handle handle)
{
    logf("[Mock] TimerContext::StartTimer");
    
    MockBase* base = get(handle);
    if (base && base->qtTimer)
    {
        base->qtTimer->start();
        return api_true;
    }
    return api_false;
}

// =============================================================
// DialogContext - Additional Stubs
// =============================================================

void DialogContext::SetDialogResizable(control_handle control, uint32 resizable)
{
    logf("[Mock] DialogContext::SetDialogResizable (%s)", resizable ? "true" : "false");
    
    QWidget* w = widgetFromHandle(control);
    QDialog* dialog = qobject_cast<QDialog*>(w);
    if (dialog)
    {
        if (resizable)
        {
            dialog->setSizeGripEnabled(true);
            dialog->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
            dialog->setMinimumSize(0, 0);
        }
        else
        {
            dialog->setSizeGripEnabled(false);
            dialog->setFixedSize(dialog->size());
        }
    }
}

// =============================================================
// TabBoxContext - Tab Widget Stubs
// =============================================================

control_handle TabBoxContext::CreateTabBox(api_handle module, control_handle client,
                                           control_handle parent, uint32 flags)
{
    logf("[Mock] TabBoxContext::CreateTabBox");
    
    // Determine parent Qt widget
    QWidget* parentWidget = determineParentWidget(parent);
    
    // Create Qt tab widget
    QTabWidget* qtTabWidget = new QTabWidget(parentWidget);
    
    // Create mock wrapper
    auto base = std::make_unique<MockBase>();
    base->pcl_handle = client;
    base->isSizer = false;
    base->isTreeNode = false;
    base->widget = qtTabWidget;
    
    control_handle handle = client;
    g_objects[handle] = std::move(base);
    
    return handle;
}

int32 TabBoxContext::GetTabBoxLength(const_control_handle control)
{
    logf("[Mock] TabBoxContext::GetTabBoxLength");
    
    QWidget* w = widgetFromHandle(const_cast<control_handle>(control));
    QTabWidget* tabWidget = qobject_cast<QTabWidget*>(w);
    if (tabWidget)
    {
        return tabWidget->count();
    }
    return 0;
}

void TabBoxContext::InsertTabBoxPage(control_handle control, int32 index,
                                     control_handle page, const char16_type* title,
                                     const void* icon)
{
    logf("[Mock] TabBoxContext::InsertTabBoxPage (index=%d)", index);
    
    QWidget* w = widgetFromHandle(control);
    QTabWidget* tabWidget = qobject_cast<QTabWidget*>(w);
    
    QWidget* pageWidget = widgetFromHandle(page);
    
    if (tabWidget && pageWidget)
    {
        QString tabTitle = title ? QString::fromUtf16(title) : QString();
        
        if (index < 0 || index >= tabWidget->count())
        {
            tabWidget->addTab(pageWidget, tabTitle);
        }
        else
        {
            tabWidget->insertTab(index, pageWidget, tabTitle);
        }
        
        // Set icon if provided
        if (icon)
        {
            // Icon handling would go here
            // For now, just add without icon
        }
    }
}

void TabBoxContext::SetTabBoxCurrentPageIndex(control_handle control, int32 index)
{
    logf("[Mock] TabBoxContext::SetTabBoxCurrentPageIndex (index=%d)", index);
    
    QWidget* w = widgetFromHandle(control);
    QTabWidget* tabWidget = qobject_cast<QTabWidget*>(w);
    if (tabWidget && index >= 0 && index < tabWidget->count())
    {
        tabWidget->setCurrentIndex(index);
    }
}

api_bool TabBoxContext::SetTabBoxPageSelectedEventRoutine(control_handle control,
                                                         api_handle receiver,
                                                         pcl::value_event_routine callback)
{
    logf("[Mock] TabBoxContext::SetTabBoxPageSelectedEventRoutine");
    
    MockBase* base = get(control);
    if (base && base->widget)
    {
        QTabWidget* tabWidget = qobject_cast<QTabWidget*>(base->widget);
        if (tabWidget)
        {
            // Connect currentChanged signal to invoke the callback
            QObject::connect(tabWidget, &QTabWidget::currentChanged,
                [base, callback](int index)
                {
                    if (callback)
                        callback(base->pcl_handle, base->pcl_handle, index);
                });
            return api_true;
        }
    }
    return api_false;
}

// =============================================================
// ControlContext - Additional Stubs
// =============================================================

uint32 ControlContext::GetControlCanvasColor(const_control_handle control)
{
    logf("[Mock] ControlContext::GetControlCanvasColor");
    
    QWidget* w = widgetFromHandle(const_cast<control_handle>(control));
    if (w)
    {
        QPalette palette = w->palette();
        QColor color = palette.color(QPalette::Window);
        // Return RGBA format: 0xAARRGGBB
        return (0xFF000000 | (color.red() << 16) | (color.green() << 8) | color.blue());
    }
    
    // Default: white
    return 0xFFFFFFFF;
}

api_bool ControlContext::GetRealTimePreviewActive(const_control_handle control)
{
    logf("[Mock] ControlContext::GetRealTimePreviewActive");
    
    // Mock implementation: no real-time preview active
    return api_false;
}

// =============================================================
// TextBoxContext - Text Box Stubs
// =============================================================

api_bool TextBoxContext::GetTextBoxText(const_control_handle control,
                                       char16_type* text, size_type* length)
{
    logf("[Mock] TextBoxContext::GetTextBoxText");
    
    QWidget* w = widgetFromHandle(const_cast<control_handle>(control));
    QTextEdit* textEdit = qobject_cast<QTextEdit*>(w);
    if (!textEdit)
        return api_false;
    
    QString plainText = textEdit->toPlainText();
    
    if (text && length)
    {
        // Convert to UTF-16 and copy
        std::u16string u16str = plainText.toStdU16String();
        size_type copyLen = std::min(*length - 1, u16str.length());
        std::memcpy(text, u16str.c_str(), copyLen * sizeof(char16_type));
        text[copyLen] = 0;
        *length = copyLen;
    }
    else if (length)
    {
        // Just return the required length
        *length = plainText.length() + 1;
    }
    
    return api_true;
}

api_bool TextBoxContext::SetTextBoxCaretPositionUpdatedEventRoutine(
    control_handle control,
    control_handle receiver,
    pcl::range_event_routine callback)
{
    logf("[Mock] TextBoxContext::SetTextBoxCaretPositionUpdatedEventRoutine");
    
    MockBase* base = get(control);
    if (base && base->widget)
    {
        QTextEdit* textEdit = qobject_cast<QTextEdit*>(base->widget);
        if (textEdit)
        {
            // Connect cursorPositionChanged signal to invoke the callback
            QObject::connect(textEdit, &QTextEdit::cursorPositionChanged,
                [base, callback, textEdit]()
                {
                    if (callback)
                    {
                        QTextCursor cursor = textEdit->textCursor();
                        int line = cursor.blockNumber();
                        int column = cursor.columnNumber();
                        callback(base->pcl_handle, base->pcl_handle, line, column);
                    }
                });
            return api_true;
        }
    }
    return api_false;
}

// =============================================================
// ComboBoxContext - Additional Stubs
// =============================================================

int32 ComboBoxContext::FindComboBoxItem(const_control_handle control,
                                       const char16_type* text,
                                       int32 startIndex,
                                       uint32 exactMatch,
                                       uint32 caseSensitive)
{
    logf("[Mock] ComboBoxContext::FindComboBoxItem (startIndex=%d, exact=%u, case=%u)",
         startIndex, exactMatch, caseSensitive);
    
    QWidget* w = widgetFromHandle(const_cast<control_handle>(control));
    QComboBox* combo = qobject_cast<QComboBox*>(w);
    if (!combo || !text)
        return -1;
    
    QString searchText = QString::fromUtf16(text);
    Qt::MatchFlags flags = Qt::MatchFlags();
    
    if (exactMatch)
        flags |= Qt::MatchExactly;
    else
        flags |= Qt::MatchContains;
    
    if (caseSensitive)
        flags |= Qt::MatchCaseSensitive;
    
    // Find item starting from startIndex
    for (int i = startIndex; i < combo->count(); ++i)
    {
        QString itemText = combo->itemText(i);
        
        if (exactMatch)
        {
            if (caseSensitive)
            {
                if (itemText == searchText)
                    return i;
            }
            else
            {
                if (itemText.compare(searchText, Qt::CaseInsensitive) == 0)
                    return i;
            }
        }
        else
        {
            if (caseSensitive)
            {
                if (itemText.contains(searchText))
                    return i;
            }
            else
            {
                if (itemText.contains(searchText, Qt::CaseInsensitive))
                    return i;
            }
        }
    }
    
    return -1;  // Not found
}

// =============================================================
// GraphicsContext - Additional Stubs
// =============================================================

void GraphicsContext::FillRect(void* handle, int x, int y, int w, int h,
                               const void* brush)
{
    logf("[Mock] GraphicsContext::FillRect (%d, %d, %d x %d)", x, y, w, h);
    // No-op - graphics not rendered in mock
}

// =============================================================
// RealTimePreviewContext - Real-Time Preview Stubs
// =============================================================

void RealTimePreviewContext::UpdateRealTimePreview()
{
    logf("[Mock] RealTimePreviewContext::UpdateRealTimePreview");
    // Mock implementation: trigger preview update
    // In real implementation, this would refresh the real-time preview
}

api_bool RealTimePreviewContext::SetRealTimePreviewOwner(interface_handle owner, uint32 flags)
{
    logf("[Mock] RealTimePreviewContext::SetRealTimePreviewOwner (flags=%u)", flags);
    // Mock implementation: set the process interface that owns this preview
    // In real implementation, this would link the preview to its owner interface
    return api_true;
}

// =============================================================
// END OF TIMER, DIALOG, TAB, TEXT, AND PREVIEW STUBS
// =============================================================

// =============================================================
// GlobalContext - Version Information Stubs
// =============================================================

void GlobalContext::GetPixInsightVersion(uint32* major, uint32* minor, uint32* release,
                                         uint32* revision, uint32* betaRelease,
                                         uint32* confidentialRelease, uint32* leVersion,
                                         char* langCode)
{
    logf("[Mock] GlobalContext::GetPixInsightVersion");
    
    // Mock version: 1.8.9
    if (major)
        *major = 1;
    if (minor)
        *minor = 8;
    if (release)
        *release = 9;
    if (revision)
        *revision = 2;
    if (betaRelease)
        *betaRelease = 0;
    if (confidentialRelease)
        *confidentialRelease = 0;
    if (leVersion)
        *leVersion = 0;
    if (langCode)
    {
        // Return "eng" for English
        langCode[0] = 'e';
        langCode[1] = 'n';
        langCode[2] = 'g';
        langCode[3] = '\0';
    }
}

char16_type* GlobalContext::GetPixInsightCodename(api_handle hModule)
{
    logf("[Mock] GlobalContext::GetPixInsightCodename");
    
    // Return a mock codename
    // The caller module is responsible for allocating and freeing this string
    const char16_t* codename = u"Ripley";
    size_t len = std::char_traits<char16_t>::length(codename);
    
    // Allocate using the module's allocator
    char16_type* result = reinterpret_cast<char16_type*>(
        API->Global->Allocate((len + 1) * sizeof(char16_type))
    );
    
    if (result)
    {
        std::memcpy(result, codename, (len + 1) * sizeof(char16_type));
    }
    
    return result;
}

// =============================================================
// TextBoxContext - Event Handler Stubs
// =============================================================

api_bool TextBoxContext::SetTextBoxUpdatedEventRoutine(control_handle control,
                                                       control_handle receiver,
                                                       pcl::unicode_event_routine callback)
{
    logf("[Mock] TextBoxContext::SetTextBoxUpdatedEventRoutine");
    
    MockBase* base = get(control);
    if (base && base->widget)
    {
        QTextEdit* textEdit = qobject_cast<QTextEdit*>(base->widget);
        if (textEdit)
        {
            // Connect textChanged signal to invoke the callback
            QObject::connect(textEdit, &QTextEdit::textChanged,
                [base, callback, textEdit]()
                {
                    if (callback)
                    {
                        QString text = textEdit->toPlainText();
                        std::u16string u16str = text.toStdU16String();
                        callback(base->pcl_handle, base->pcl_handle,
                                reinterpret_cast<const char16_type*>(u16str.c_str()));
                    }
                });
            return api_true;
        }
    }
    return api_false;
}

// =============================================================
// CodeEditorContext - Code Editor Stubs
// =============================================================

control_handle CodeEditorContext::CreateCodeEditor(api_handle module, control_handle client,
                                                   control_handle parent, uint32 flags)
{
    logf("[Mock] CodeEditorContext::CreateCodeEditor");
    
    // Determine parent Qt widget
    QWidget* parentWidget = determineParentWidget(parent);
    
    // Create Qt text edit as a code editor (could be QPlainTextEdit for better performance)
    QTextEdit* qtCodeEditor = new QTextEdit(parentWidget);
    
    // Set monospace font for code editing
    QFont monoFont("Courier New", 10);
    monoFont.setStyleHint(QFont::Monospace);
    qtCodeEditor->setFont(monoFont);
    
    // Enable tab/space handling for code
    qtCodeEditor->setTabStopDistance(40); // 4 spaces worth
    qtCodeEditor->setAcceptRichText(false);
    
    // Create mock wrapper
    auto base = std::make_unique<MockBase>();
    base->pcl_handle = client;
    base->isSizer = false;
    base->isTreeNode = false;
    base->widget = qtCodeEditor;
    
    control_handle handle = client;
    g_objects[handle] = std::move(base);
    
    return handle;
}

void CodeEditorContext::SetEditorText(control_handle control, const char16_type* text)
{
    logf("[Mock] CodeEditorContext::SetEditorText");
    
    QWidget* w = widgetFromHandle(control);
    QTextEdit* editor = qobject_cast<QTextEdit*>(w);
    if (editor && text)
    {
        QString qtext = QString::fromUtf16(text);
        editor->setPlainText(qtext);
    }
}

void CodeEditorContext::SetEditorReadOnly(control_handle control, uint32 readOnly)
{
    logf("[Mock] CodeEditorContext::SetEditorReadOnly (%s)", readOnly ? "true" : "false");
    
    QWidget* w = widgetFromHandle(control);
    QTextEdit* editor = qobject_cast<QTextEdit*>(w);
    if (editor)
    {
        editor->setReadOnly(readOnly != 0);
    }
}

// =============================================================
// END OF VERSION, TEXTBOX, AND CODEEDITOR STUBS
// =============================================================

// =============================================================
// EditContext - Additional Stubs
// =============================================================

api_bool EditContext::GetEditModified(const_control_handle control)
{
    logf("[Mock] EditContext::GetEditModified");
    
    QWidget* w = widgetFromHandle(const_cast<control_handle>(control));
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(w);
    if (lineEdit)
    {
        return lineEdit->isModified() ? api_true : api_false;
    }
    
    return api_false;
}

void EditContext::SetEditModified(control_handle control, uint32 modified)
{
    logf("[Mock] EditContext::SetEditModified (%s)", modified ? "true" : "false");
    
    QWidget* w = widgetFromHandle(control);
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(w);
    if (lineEdit)
    {
        lineEdit->setModified(modified != 0);
    }
}

// =============================================================
// FontContext - Additional Stubs
// =============================================================

void FontContext::GetStringPixelRect(const_font_handle font, const char16_type* text,
                                     int32* x, int32* y, int32* w, int32* h,
                                     uint32 flags)
{
    logf("[Mock] FontContext::GetStringPixelRect");
    
    // Create a QFont from the handle (simplified - assumes font handle has basic info)
    QFont qfont("Arial", 10);
    QFontMetrics fm(qfont);
    QString qtext = text ? QString::fromUtf16(text) : QString();
    
    QRect rect = fm.tightBoundingRect(qtext);
    
    if (x) *x = rect.x();
    if (y) *y = rect.y();
    if (w) *w = rect.width();
    if (h) *h = rect.height();
}

// =============================================================
// LabelContext - Additional Stubs
// =============================================================

api_bool LabelContext::GetLabelText(const_control_handle control,
                                    char16_type* text, size_type* length)
{
    logf("[Mock] LabelContext::GetLabelText");
    
    QWidget* w = widgetFromHandle(const_cast<control_handle>(control));
    QLabel* label = qobject_cast<QLabel*>(w);
    if (!label)
        return api_false;
    
    QString labelText = label->text();
    
    if (text && length)
    {
        // Convert to UTF-16 and copy
        std::u16string u16str = labelText.toStdU16String();
        size_type copyLen = std::min(*length - 1, u16str.length());
        std::memcpy(text, u16str.c_str(), copyLen * sizeof(char16_type));
        text[copyLen] = 0;
        *length = copyLen;
    }
    else if (length)
    {
        // Just return the required length
        *length = labelText.length() + 1;
    }
    
    return api_true;
}

// =============================================================
// DialogContext - Additional Stubs
// =============================================================

void DialogContext::OpenDialog(control_handle control)
{
    logf("[Mock] DialogContext::OpenDialog");
    
    QWidget* w = widgetFromHandle(control);
    QDialog* dialog = qobject_cast<QDialog*>(w);
    if (dialog)
    {
        dialog->show();
    }
}

// =============================================================
// GlobalContext - Additional Stubs
// =============================================================

api_bool GlobalContext::ResetProcessStatus()
{
    logf("[Mock] GlobalContext::ResetProcessStatus");
    // Mock implementation: reset any process status flags
    return api_true;
}

// =============================================================
// ControlContext - Additional Stubs
// =============================================================

api_bool ControlContext::SetCloseEventRoutine(control_handle control, control_handle receiver,
                                              pcl::control_event_routine callback)
{
    logf("[Mock] ControlContext::SetCloseEventRoutine");
    
    MockBase* base = get(control);
    if (base && base->widget)
    {
      //        QWidget* w = base->widget;
        
        // Store the callback
        base->onClose = callback;
        
        // Install event filter to catch close events
        // Note: This is a simplified implementation
        // Full implementation would need proper QEvent::Close handling
        
        return api_true;
    }
    return api_false;
}

// =============================================================
// ProcessContext - Additional Stubs
// =============================================================

char16_type* ProcessContext::GetProcessInstanceSourceCode(api_handle hModule,
                                                          const_process_handle process,
                                                          const char* language,
                                                          const char* varId,
                                                          uint32 indent)
{
    logf("[Mock] ProcessContext::GetProcessInstanceSourceCode");
    
    // Return a mock source code string
    const char16_t* mockCode = u"// Mock process instance source code\n";
    size_t len = std::char_traits<char16_t>::length(mockCode);
    
    // Allocate using the module's allocator
    char16_type* result = reinterpret_cast<char16_type*>(
        API->Global->Allocate((len + 1) * sizeof(char16_type))
    );
    
    if (result)
    {
        std::memcpy(result, mockCode, (len + 1) * sizeof(char16_type));
    }
    
    return result;
}

// =============================================================
// GraphicsContext - Additional Drawing Stubs
// =============================================================

void GraphicsContext::DrawTextRect(graphics_handle handle, int32 x, int32 y,
                                   int32 w, int32 h, const char16_type* text,
                                   int32 align)
{
    logf("[Mock] GraphicsContext::DrawTextRect (%d, %d, %d x %d, align=%d)",
         x, y, w, h, align);
    // No-op - graphics not rendered in mock
}

void GraphicsContext::EnableGraphicsTextAntialiasing(graphics_handle handle, uint32 enabled)
{
    logf("[Mock] GraphicsContext::EnableGraphicsTextAntialiasing (%s)",
         enabled ? "enabled" : "disabled");
    // No-op - graphics not rendered in mock
}

void GraphicsContext::SetGraphicsTransparentBackground(graphics_handle handle, uint32 transparent)
{
    logf("[Mock] GraphicsContext::SetGraphicsTransparentBackground (%s)",
         transparent ? "true" : "false");
    // No-op - graphics not rendered in mock
}

// =============================================================
// NumericalContext - Additional Stubs
// =============================================================

char* NumericalContext::SurfaceSplineSerialize(api_handle hModule,
                                               const_sspline_handle hSS,
                                               uint32 flags)
{
    logf("[Mock] NumericalContext::SurfaceSplineSerialize");
    
    // Return a mock serialized string
    const char* mockData = "MOCK_SURFACE_SPLINE_DATA";
    size_t len = std::strlen(mockData);
    
    // Allocate using the module's allocator
    char* result = reinterpret_cast<char*>(
        API->Global->Allocate((len + 1) * sizeof(char))
    );
    
    if (result)
    {
        std::memcpy(result, mockData, len + 1);
    }
    
    return result;
}

// =============================================================
// FileFormatContext - Comprehensive File Format Stubs
// =============================================================

api_bool FileFormatContext::AddKeyword(file_format_handle handle,
                                       const char* name, const char* value,
                                       const char* comment)
{
    logf("[Mock] FileFormatContext::AddKeyword (name=%s)", name ? name : "null");
    // Mock implementation: accept keyword but don't store
    return api_true;
}

const void* FileFormatContext::GetICCProfile(file_format_handle handle)
{
    logf("[Mock] FileFormatContext::GetICCProfile");
    // Mock implementation: no ICC profile
    return nullptr;
}

api_bool FileFormatContext::SetICCProfile(file_format_handle handle, const void* profile)
{
    logf("[Mock] FileFormatContext::SetICCProfile");
    // Mock implementation: accept profile but don't store
    return api_true;
}

api_bool FileFormatContext::CloseImageFile(file_format_handle handle)
{
    logf("[Mock] FileFormatContext::CloseImageFile");
    // Mock implementation: close file handle
    return api_true;
}

api_bool FileFormatContext::GetNextKeyword(file_format_handle handle,
                                           char* name, char* value, char* comment,
                                           uint32 maxLen)
{
    logf("[Mock] FileFormatContext::GetNextKeyword");
    // Mock implementation: no more keywords
    return api_false;
}

size_type FileFormatContext::GetKeywordCount(file_format_handle handle)
{
    logf("[Mock] FileFormatContext::GetKeywordCount");
    // Mock implementation: no keywords
    return 0;
}

api_bool FileFormatContext::SetImageOptions(file_format_handle handle,
                                            const api_image_options* options)
{
    logf("[Mock] FileFormatContext::SetImageOptions");
    // Mock implementation: accept options
    return api_true;
}

api_bool FileFormatContext::GetImageProperty(file_format_handle handle,
                                             const char* id, api_property_value* value)
{
    logf("[Mock] FileFormatContext::GetImageProperty (id=%s)", id ? id : "null");
    // Mock implementation: property not found
    return api_false;
}

api_bool FileFormatContext::SetImageProperty(file_format_handle handle,
                                             const char* id,
                                             const api_property_value* value)
{
    logf("[Mock] FileFormatContext::SetImageProperty (id=%s)", id ? id : "null");
    // Mock implementation: accept property
    return api_true;
}

void FileFormatContext::EndKeywordEmbedding(file_format_handle handle)
{
    logf("[Mock] FileFormatContext::EndKeywordEmbedding");
    // Mock implementation: finalize keyword embedding
}

void FileFormatContext::EndKeywordExtraction(file_format_handle handle)
{
    logf("[Mock] FileFormatContext::EndKeywordExtraction");
    // Mock implementation: finalize keyword extraction
}

api_bool FileFormatContext::BeginKeywordEmbedding(file_format_handle handle)
{
    logf("[Mock] FileFormatContext::BeginKeywordEmbedding");
    // Mock implementation: begin keyword embedding
    return api_true;
}

const void* FileFormatContext::GetFormatSpecificData(const_file_format_handle handle)
{
    logf("[Mock] FileFormatContext::GetFormatSpecificData");
    // Mock implementation: no format-specific data
    return nullptr;
}

api_bool FileFormatContext::SetFormatSpecificData(file_format_handle handle,
                                                  const void* data)
{
    logf("[Mock] FileFormatContext::SetFormatSpecificData");
    // Mock implementation: accept data
    return api_true;
}

api_bool FileFormatContext::BeginKeywordExtraction(file_format_handle handle)
{
    logf("[Mock] FileFormatContext::BeginKeywordExtraction");
    // Mock implementation: begin keyword extraction
    return api_true;
}

void FileFormatContext::EndICCProfileEmbedding(file_format_handle handle)
{
    logf("[Mock] FileFormatContext::EndICCProfileEmbedding");
    // Mock implementation: finalize ICC profile embedding
}

void FileFormatContext::EndICCProfileExtraction(file_format_handle handle)
{
    logf("[Mock] FileFormatContext::EndICCProfileExtraction");
    // Mock implementation: finalize ICC profile extraction
}

api_bool FileFormatContext::BeginICCProfileEmbedding(file_format_handle handle)
{
    logf("[Mock] FileFormatContext::BeginICCProfileEmbedding");
    // Mock implementation: begin ICC profile embedding
    return api_true;
}

api_bool FileFormatContext::EnumerateImageProperties(file_format_handle handle,
                                                     pcl::property_enumeration_callback callback,
                                                     char* id, size_type* length, void* data)
{
    logf("[Mock] FileFormatContext::EnumerateImageProperties");
    // Mock implementation: no properties to enumerate
    return api_true;
}

api_bool FileFormatContext::BeginICCProfileExtraction(file_format_handle handle)
{
    logf("[Mock] FileFormatContext::BeginICCProfileExtraction");
    // Mock implementation: begin ICC profile extraction
    return api_true;
}

void FileFormatContext::EndImagePropertyEmbedding(file_format_handle handle)
{
    logf("[Mock] FileFormatContext::EndImagePropertyEmbedding");
    // Mock implementation: finalize image property embedding
}

void FileFormatContext::EndImagePropertyExtraction(file_format_handle handle)
{
    logf("[Mock] FileFormatContext::EndImagePropertyExtraction");
    // Mock implementation: finalize image property extraction
}

api_bool FileFormatContext::BeginImagePropertyEmbedding(file_format_handle handle)
{
    logf("[Mock] FileFormatContext::BeginImagePropertyEmbedding");
    // Mock implementation: begin image property embedding
    return api_true;
}

meta_format_handle FileFormatContext::GetFileFormatInstanceFormat(const_file_format_handle handle)
{
    logf("[Mock] FileFormatContext::GetFileFormatInstanceFormat");
    // Mock implementation: return a dummy meta format handle
    static int dummyMetaFormat = 0;
    return &dummyMetaFormat;
}

api_bool FileFormatContext::BeginImagePropertyExtraction(file_format_handle handle)
{
    logf("[Mock] FileFormatContext::BeginImagePropertyExtraction");
    // Mock implementation: begin image property extraction
    return api_true;
}

// =============================================================
// ImageWindowContext - Additional Stubs
// =============================================================

void ImageWindowContext::ImageScalarToViewport(const_window_handle window_context, int32* value)
{
    logf("[Mock] ImageWindowContext::ImageScalarToViewport (%d)", value ? *value : 0);
    // Mock implementation: value unchanged (1:1 scale)
    // In real implementation, this would scale from image to viewport units
}

void ImageWindowContext::UpdateImageWindowViewport(window_handle window_context)
{
    logf("[Mock] ImageWindowContext::UpdateImageWindowViewport");
    // Mock implementation: update viewport display
    // In real implementation, this would refresh the viewport
}

// =============================================================
// END OF COMPREHENSIVE STUBS
// =============================================================
// =============================================================
// ComboBoxContext - Additional Stub
// =============================================================

void ComboBoxContext::SetComboBoxMaxVisibleItemCount(control_handle control, int32 count)
{
    logf("[Mock] ComboBoxContext::SetComboBoxMaxVisibleItemCount (count=%d)", count);
    
    QWidget* w = widgetFromHandle(control);
    QComboBox* combo = qobject_cast<QComboBox*>(w);
    if (combo)
    {
        combo->setMaxVisibleItems(count);
    }
}

// =============================================================
// END OF COMBOBOX STUB
// =============================================================

// =============================================================
// ViewContext - Screen Transfer Functions Stubs
// =============================================================

api_bool ViewContext::GetViewScreenTransferFunctions(const_view_handle view,
                                                     double* m, double* c0, double* c1,
                                                     double* r0, double* r1)
{
    logf("[Mock] ViewContext::GetViewScreenTransferFunctions");
    
    // Mock implementation: return identity STF (no transformation)
    if (m) *m = 0.5;    // midtones
    if (c0) *c0 = 0.0;  // shadows clipping
    if (c1) *c1 = 1.0;  // highlights clipping
    if (r0) *r0 = 0.0;  // shadows dynamic range
    if (r1) *r1 = 1.0;  // highlights dynamic range
    
    return api_false;  // No STF currently set
}

api_bool ViewContext::SetViewScreenTransferFunctions(view_handle view,
                                                     const double* m, const double* c0,
                                                     const double* c1, const double* r0,
                                                     const double* r1, uint32 notify)
{
    logf("[Mock] ViewContext::SetViewScreenTransferFunctions");
    
    // Mock implementation: accept STF but don't apply
    return api_true;
}

api_bool ViewContext::GetViewScreenTransferFunctionsEnabled(view_handle view)
{
    logf("[Mock] ViewContext::GetViewScreenTransferFunctionsEnabled");
    
    // Mock implementation: STF not enabled
    return api_false;
}

void ViewContext::SetViewScreenTransferFunctionsEnabled(view_handle view,
                                                        uint32 enabled, uint32 notify)
{
    logf("[Mock] ViewContext::SetViewScreenTransferFunctionsEnabled (%s)",
         enabled ? "enabled" : "disabled");
    
    // Mock implementation: accept setting but don't apply
}

// =============================================================
// BrushContext - Gradient Brush Stubs
// =============================================================

brush_handle BrushContext::CreateLinearGradientBrush(api_handle module,
                                                     double x1, double y1,
                                                     double x2, double y2,
                                                     int32 spread,
                                                     const api_gradient_stop* stops,
                                                     size_type count)
{
    logf("[Mock] BrushContext::CreateLinearGradientBrush");
    
    // Mock implementation: return a dummy brush handle
    static int dummyBrush = 0;
    return &dummyBrush;
}

brush_handle BrushContext::CreateRadialGradientBrush(api_handle module,
                                                     double cx, double cy, double r,
                                                     double fx, double fy,
                                                     int32 spread,
                                                     const api_gradient_stop* stops,
                                                     size_type count)
{
    logf("[Mock] BrushContext::CreateRadialGradientBrush");
    
    // Mock implementation: return a dummy brush handle
    static int dummyBrush = 0;
    return &dummyBrush;
}

// =============================================================
// TimerContext - Additional Stubs
// =============================================================

void TimerContext::StopTimer(timer_handle handle)
{
    logf("[Mock] TimerContext::StopTimer");
    
    MockBase* base = get(handle);
    if (base && base->qtTimer)
    {
        base->qtTimer->stop();
    }
}

// =============================================================
// ControlContext - Additional Event Stubs
// =============================================================

api_bool ControlContext::SetWheelEventRoutine(control_handle control, control_handle receiver,
                                              pcl::wheel_event_routine callback)
{
    logf("[Mock] ControlContext::SetWheelEventRoutine");
    
    MockBase* base = get(control);
    if (base && base->widget)
    {
        // Store callback
        base->onMouseWheel = callback;
        
        // Install event filter to catch wheel events
        // This would require proper QEvent::Wheel handling
        
        return api_true;
    }
    return api_false;
}

api_bool ControlContext::SetMouseDoubleClickEventRoutine(control_handle control,
                                                         control_handle receiver,
                                                         pcl::mouse_event_routine callback)
{
    logf("[Mock] ControlContext::SetMouseDoubleClickEventRoutine");
    
    MockBase* base = get(control);
    if (base && base->widget)
    {
        // Store callback
        base->onMouseDoubleClick = callback;
        
        // Connect to Qt double click events
        // This would be handled by the event filter
        
        return api_true;
    }
    return api_false;
}

// =============================================================
// SpinBoxContext - Additional Stubs
// =============================================================

int32 SpinBoxContext::GetSpinBoxValue(const_control_handle control)
{
    logf("[Mock] SpinBoxContext::GetSpinBoxValue");
    
    QWidget* w = widgetFromHandle(const_cast<control_handle>(control));
    QSpinBox* spinBox = qobject_cast<QSpinBox*>(w);
    if (spinBox)
    {
        return spinBox->value();
    }
    
    return 0;
}

// =============================================================
// TreeBoxContext - Additional Stubs
// =============================================================

void TreeBoxContext::SortTreeBox(control_handle control, int32 col, uint32 ascending)
{
    logf("[Mock] TreeBoxContext::SortTreeBox (col=%d, ascending=%s)",
         col, ascending ? "true" : "false");
    
    QWidget* w = widgetFromHandle(control);
    QTreeWidget* tree = qobject_cast<QTreeWidget*>(w);
    if (tree)
    {
        tree->sortItems(col, ascending ? Qt::AscendingOrder : Qt::DescendingOrder);
    }
}

void TreeBoxContext::SetTreeBoxColumnWidth(control_handle control, int32 col, int32 width)
{
    logf("[Mock] TreeBoxContext::SetTreeBoxColumnWidth (col=%d, width=%d)", col, width);
    
    QWidget* w = widgetFromHandle(control);
    QTreeWidget* tree = qobject_cast<QTreeWidget*>(w);
    if (tree && col >= 0 && col < tree->columnCount())
    {
        tree->setColumnWidth(col, width);
    }
}

void TreeBoxContext::SetTreeBoxColumnVisible(control_handle control, int32 col, uint32 visible)
{
    logf("[Mock] TreeBoxContext::SetTreeBoxColumnVisible (col=%d, visible=%s)",
         col, visible ? "true" : "false");
    
    QWidget* w = widgetFromHandle(control);
    QTreeWidget* tree = qobject_cast<QTreeWidget*>(w);
    if (tree && col >= 0 && col < tree->columnCount())
    {
        tree->setColumnHidden(col, !visible);
    }
}

void TreeBoxContext::SetTreeBoxHeaderSortingEnabled(control_handle control, uint32 enabled)
{
    logf("[Mock] TreeBoxContext::SetTreeBoxHeaderSortingEnabled (%s)",
         enabled ? "enabled" : "disabled");
    
    QWidget* w = widgetFromHandle(control);
    QTreeWidget* tree = qobject_cast<QTreeWidget*>(w);
    if (tree)
    {
        tree->setSortingEnabled(enabled != 0);
    }
}

api_bool TreeBoxContext::SetTreeBoxNodeUpdatedEventRoutine(control_handle control,
                                                           api_handle receiver,
                                                           pcl::item_value_event_routine callback)
{
    logf("[Mock] TreeBoxContext::SetTreeBoxNodeUpdatedEventRoutine");
    
    MockBase* base = get(control);
    if (base && base->widget)
    {
        QTreeWidget* tree = qobject_cast<QTreeWidget*>(base->widget);
        if (tree)
        {
            // Connect to itemChanged signal
            QObject::connect(tree, &QTreeWidget::itemChanged,
                [base, callback](QTreeWidgetItem* item, int column)
                {
                    if (callback)
                    {
                        // Get the node handle for this item
                        // This would require tracking QTreeWidgetItem* -> api_handle mapping
                        api_handle nodeHandle = nullptr; // Would need proper mapping
                        callback(base->pcl_handle, base->pcl_handle, nodeHandle, column);
                    }
                });
            return api_true;
        }
    }
    return api_false;
}

// =============================================================
// GraphicsContext - Additional Drawing Stubs
// =============================================================

void GraphicsContext::StrokeRect(graphics_handle handle, int32 x, int32 y,
                                 int32 w, int32 h, const_pen_handle pen)
{
    logf("[Mock] GraphicsContext::StrokeRect (%d, %d, %d x %d)", x, y, w, h);
    // No-op - graphics not rendered in mock
}

void GraphicsContext::DrawEllipse(graphics_handle handle, int32 x, int32 y,
                                  int32 w, int32 h)
{
    logf("[Mock] GraphicsContext::DrawEllipse (%d, %d, %d x %d)", x, y, w, h);
    // No-op - graphics not rendered in mock
}

void GraphicsContext::DrawBitmapRect(graphics_handle handle, int32 x, int32 y,
                                     const_bitmap_handle bitmap,
                                     int32 sx, int32 sy, int32 sw, int32 sh)
{
    logf("[Mock] GraphicsContext::DrawBitmapRect (%d, %d, src: %d, %d, %d x %d)",
         x, y, sx, sy, sw, sh);
    // No-op - graphics not rendered in mock
}

// =============================================================
// FileFormatContext - Additional Stubs
// =============================================================

api_bool FileFormatContext::IsImageFileOpen(const_file_format_handle handle)
{
    logf("[Mock] FileFormatContext::IsImageFileOpen");
    
    // Mock implementation: pretend file is not open
    return api_false;
}

// =============================================================
// ImageWindowContext - Additional Stubs
// =============================================================

void ImageWindowContext::ViewportToImageD(const_window_handle window_context,
                                          double* x, double* y)
{
    logf("[Mock] ImageWindowContext::ViewportToImageD (%.2f, %.2f)",
         x ? *x : 0.0, y ? *y : 0.0);
    // Mock implementation: coordinates unchanged (1:1 mapping)
}

void ImageWindowContext::RegenerateImageWindowViewport(window_handle window_context)
{
    logf("[Mock] ImageWindowContext::RegenerateImageWindowViewport");
    // Mock implementation: regenerate viewport rendering
}

// =============================================================
// ExternalProcessContext - Process Management Stubs
// =============================================================

external_process_handle ExternalProcessContext::CreateExternalProcess(api_handle hModule,
                                                                      api_handle hClient)
{
    logf("[Mock] ExternalProcessContext::CreateExternalProcess");
    
    // Create mock wrapper for external process
    auto base = std::make_unique<MockBase>();
    base->process_handle = hClient;
    base->isSizer = false;
    base->isTreeNode = false;
    
    external_process_handle handle = reinterpret_cast<external_process_handle>(base.get());
    g_objects[handle] = std::move(base);
    
    return handle;
}

api_bool ExternalProcessContext::StartExternalProcess(external_process_handle handle,
                                                      const char16_type* program,
                                                      const char16_type** argv,
                                                      size_type argc)
{
    logf("[Mock] ExternalProcessContext::StartExternalProcess");
    
    // Mock implementation: pretend to start process
    return api_true;
}

api_bool ExternalProcessContext::KillExternalProcess(external_process_handle handle)
{
    logf("[Mock] ExternalProcessContext::KillExternalProcess");
    
    // Mock implementation: kill process
    return api_true;
}

api_bool ExternalProcessContext::TerminateExternalProcess(external_process_handle handle)
{
    logf("[Mock] ExternalProcessContext::TerminateExternalProcess");
    
    // Mock implementation: terminate process gracefully
    return api_true;
}

api_bool ExternalProcessContext::WriteToExternalProcess(external_process_handle handle,
                                                        const void* data, size_type count)
{
    logf("[Mock] ExternalProcessContext::WriteToExternalProcess (count=%zu)", (size_t)count);
    
    // Mock implementation: pretend to write to process stdin
    return api_true;
}

api_bool ExternalProcessContext::ReadFromExternalProcess(api_handle hModule,
                                                         external_process_handle handle,
                                                         int32 stream,
                                                         void** data, size_type* count)
{
    logf("[Mock] ExternalProcessContext::ReadFromExternalProcess (stream=%d)", stream);
    
    // Mock implementation: no data available
    if (data) *data = nullptr;
    if (count) *count = 0;
    
    return api_true;
}

api_bool ExternalProcessContext::CloseExternalProcessStream(external_process_handle handle,
                                                            int32 stream)
{
    logf("[Mock] ExternalProcessContext::CloseExternalProcessStream (stream=%d)", stream);
    
    // Mock implementation: close stream
    return api_true;
}

api_bool ExternalProcessContext::GetExternalProcessIsRunning(const_external_process_handle handle)
{
    logf("[Mock] ExternalProcessContext::GetExternalProcessIsRunning");
    
    // Mock implementation: process is not running
    return api_false;
}

api_bool ExternalProcessContext::WaitForExternalProcessFinished(external_process_handle handle,
                                                                int32 ms)
{
    logf("[Mock] ExternalProcessContext::WaitForExternalProcessFinished (ms=%d)", ms);
    
    // Mock implementation: process already finished
    return api_true;
}

api_bool ExternalProcessContext::SetExternalProcessWorkingDirectory(external_process_handle handle,
                                                                    const char16_type* dir)
{
    logf("[Mock] ExternalProcessContext::SetExternalProcessWorkingDirectory");
    
    // Mock implementation: set working directory
    return api_true;
}

api_bool ExternalProcessContext::SetExternalProcessStartedEventRoutine(
    external_process_handle handle,
    api_handle receiver,
    pcl::external_process_event_routine callback)
{
    logf("[Mock] ExternalProcessContext::SetExternalProcessStartedEventRoutine");
    
    // Mock implementation: store callback
    return api_true;
}

api_bool ExternalProcessContext::SetExternalProcessFinishedEventRoutine(
    external_process_handle handle,
    api_handle receiver,
    pcl::external_process_exit_status_event_routine callback)
{
    logf("[Mock] ExternalProcessContext::SetExternalProcessFinishedEventRoutine");
    
    // Mock implementation: store callback
    return api_true;
}

api_bool ExternalProcessContext::SetExternalProcessErrorEventRoutine(
    external_process_handle handle,
    api_handle receiver,
    pcl::external_process_status_event_routine callback)
{
    logf("[Mock] ExternalProcessContext::SetExternalProcessErrorEventRoutine");
    
    // Mock implementation: store callback
    return api_true;
}

api_bool ExternalProcessContext::SetExternalProcessStandardOutputDataAvailableEventRoutine(
    external_process_handle handle,
    api_handle receiver,
    pcl::external_process_event_routine callback)
{
    logf("[Mock] ExternalProcessContext::SetExternalProcessStandardOutputDataAvailableEventRoutine");
    
    // Mock implementation: store callback
    return api_true;
}

api_bool ExternalProcessContext::SetExternalProcessStandardErrorDataAvailableEventRoutine(
    external_process_handle handle,
    api_handle receiver,
    pcl::external_process_event_routine callback)
{
    logf("[Mock] ExternalProcessContext::SetExternalProcessStandardErrorDataAvailableEventRoutine");
    
    // Mock implementation: store callback
    return api_true;
}

// =============================================================
// END OF VIEW, BRUSH, TIMER, CONTROL, SPINBOX, TREEBOX, GRAPHICS, AND EXTERNALPROCESS STUBS
// =============================================================

// =============================================================
// ViewContext - Additional Stubs
// =============================================================

api_bool ViewContext::DeleteViewProperty(api_handle hModule, view_handle view,
                                         const char* id, uint32 notify)
{
    logf("[Mock] ViewContext::DeleteViewProperty (id=%s, notify=%s)",
         id ? id : "null", notify ? "true" : "false");
    
    // Mock implementation: property doesn't exist
    return api_false;
}

// =============================================================
// GlobalContext - Settings Stubs (Unsigned Integer)
// =============================================================

api_bool GlobalContext::ReadSettingsUnsignedInteger(api_handle hModule,
                                                    uint32* value,
                                                    const char* key,
                                                    uint32 global)
{
    logf("[Mock] GlobalContext::ReadSettingsUnsignedInteger (key=%s, global=%s)",
         key ? key : "null", global ? "true" : "false");
    
    // Mock implementation: setting not found
    if (value)
        *value = 0;
    
    return api_false;
}

api_bool GlobalContext::WriteSettingsUnsignedInteger(api_handle hModule,
                                                     uint32 value,
                                                     const char* key,
                                                     uint32 global)
{
    logf("[Mock] GlobalContext::WriteSettingsUnsignedInteger (key=%s, value=%u, global=%s)",
         key ? key : "null", value, global ? "true" : "false");
    
    // Mock implementation: accept write
    return api_true;
}

// =============================================================
// ModuleContext - Resource Loading Stub
// =============================================================

api_bool ModuleContext::LoadResource(api_handle hModule,
                                     const char16_type* resourceType,
                                     const char16_type* resourceName)
{
    logf("[Mock] ModuleContext::LoadResource");
    
    // Mock implementation: resource not found
    return api_false;
}

// =============================================================
// SliderContext - Additional Stub
// =============================================================

void SliderContext::SetSliderStepSize(control_handle control, int32 step)
{
    logf("[Mock] SliderContext::SetSliderStepSize (step=%d)", step);
    
    QWidget* w = widgetFromHandle(control);
    QSlider* slider = qobject_cast<QSlider*>(w);
    if (slider)
    {
        slider->setSingleStep(step);
    }
}

// =============================================================
// GraphicsContext - Additional Drawing Stub
// =============================================================

void GraphicsContext::DrawPolygonD(graphics_handle handle, const double* points,
                                   size_type count, int32 fillRule)
{
    logf("[Mock] GraphicsContext::DrawPolygonD (count=%zu, fillRule=%d)",
         (size_t)count, fillRule);
    // No-op - graphics not rendered in mock
}

// =============================================================
// ImageWindowContext - Additional Stubs
// =============================================================

void ImageWindowContext::SetDynamicCursor(window_handle window_context,
                                          const_bitmap_handle bitmap,
                                          int32 hx, int32 hy)
{
    logf("[Mock] ImageWindowContext::SetDynamicCursor (hotspot: %d, %d)", hx, hy);
    
    // Mock implementation: set cursor bitmap
    // In real implementation, this would set the cursor for dynamic interface
}

int32 ImageWindowContext::GetCursorTolerance()
{
    logf("[Mock] ImageWindowContext::GetCursorTolerance");
    
    // Mock implementation: return default cursor tolerance in pixels
    return 4;  // Typical tolerance for cursor hit-testing
}

void ImageWindowContext::ClearImageWindowAstrometricSolution(window_handle window_context,
                                                             uint32 notify)
{
    logf("[Mock] ImageWindowContext::ClearImageWindowAstrometricSolution (notify=%s)",
         notify ? "true" : "false");
    
    // Mock implementation: clear astrometric solution
    // In real implementation, this would remove WCS metadata
}

// =============================================================
// END OF GEOMETRY PROJECT STUBS
// =============================================================

// =============================================================
// FontContext - Font Enumeration Stub
// =============================================================

api_bool FontContext::EnumerateFonts(font_enumeration_callback callback,
                                     char16_type* fontFace, size_type* len,
                                     void* data, const char* writingSystem)
{
    logf("[Mock] FontContext::EnumerateFonts (writingSystem=%s)",
         writingSystem ? writingSystem : "null");
    
    // Mock implementation: enumerate a few common fonts
    if (callback && fontFace && len)
    {
        const char16_t* fonts[] = {
            u"Arial",
            u"Courier New",
            u"Helvetica",
            u"Times New Roman",
            u"Verdana"
        };
        
        for (size_t i = 0; i < 5; ++i)
        {
            size_t fontLen = std::char_traits<char16_t>::length(fonts[i]);
            if (*len > fontLen)
            {
                std::memcpy(fontFace, fonts[i], (fontLen + 1) * sizeof(char16_type));
                if (!callback(fontFace, data))
                    break;  // Callback returned false, stop enumeration
            }
        }
    }
    
    return api_true;
}

// =============================================================
// GlobalContext - Global Settings Stubs (Real, Flag, Color, String, Integer)
// =============================================================

api_bool GlobalContext::GetGlobalReal(const char* key, double* value)
{
    logf("[Mock] GlobalContext::GetGlobalReal (key=%s)", key ? key : "null");
    
    // Mock implementation: return default value
    if (value)
        *value = 0.0;
    
    return api_false;  // Setting not found
}

api_bool GlobalContext::SetGlobalFlag(const char* key, uint32 value)
{
    logf("[Mock] GlobalContext::SetGlobalFlag (key=%s, value=%s)",
         key ? key : "null", value ? "true" : "false");
    
    // Mock implementation: accept write
    return api_true;
}

api_bool GlobalContext::SetGlobalReal(const char* key, double value)
{
    logf("[Mock] GlobalContext::SetGlobalReal (key=%s, value=%.6f)",
         key ? key : "null", value);
    
    // Mock implementation: accept write
    return api_true;
}

api_bool GlobalContext::GetGlobalColor(const char* key, uint32* value)
{
    logf("[Mock] GlobalContext::GetGlobalColor (key=%s)", key ? key : "null");
    
    // Mock implementation: return default color (white)
    if (value)
        *value = 0xFFFFFFFF;
    
    return api_false;  // Setting not found
}

api_bool GlobalContext::SetGlobalColor(const char* key, uint32 value)
{
    logf("[Mock] GlobalContext::SetGlobalColor (key=%s, value=0x%08X)",
         key ? key : "null", value);
    
    // Mock implementation: accept write
    return api_true;
}

api_bool GlobalContext::SetGlobalString(const char* key, const char16_type* value)
{
    logf("[Mock] GlobalContext::SetGlobalString (key=%s)", key ? key : "null");
    
    // Mock implementation: accept write
    return api_true;
}

api_bool GlobalContext::SetGlobalInteger(const char* key, uint32 value, uint32 isSigned)
{
    logf("[Mock] GlobalContext::SetGlobalInteger (key=%s, value=%u, signed=%s)",
         key ? key : "null", value, isSigned ? "true" : "false");
    
    // Mock implementation: accept write
    return api_true;
}

// =============================================================
// GlobalContext - Readout Options Stubs
// =============================================================

void GlobalContext::GetReadoutOptions(api_readout_options* options)
{
    logf("[Mock] GlobalContext::GetReadoutOptions");
    
    if (options)
    {
        // Mock implementation: return default readout options
        std::memset(options, 0, sizeof(api_readout_options));
        // Set some reasonable defaults
        options->precision = 6;
        options->range = 0;  // Normalized range [0,1]
    }
}

void GlobalContext::SetReadoutOptions(const api_readout_options* options)
{
    logf("[Mock] GlobalContext::SetReadoutOptions");
    
    // Mock implementation: accept options
}

// =============================================================
// GlobalContext - Global Settings Update Context Stubs
// =============================================================

api_bool GlobalContext::EnterGlobalSettingsUpdateContext()
{
    logf("[Mock] GlobalContext::EnterGlobalSettingsUpdateContext");
    
    // Mock implementation: enter update context
    return api_true;
}

api_bool GlobalContext::IsGlobalSettingsUpdateContextActive()
{
    logf("[Mock] GlobalContext::IsGlobalSettingsUpdateContextActive");
    
    // Mock implementation: no update context active
    return api_false;
}

api_bool GlobalContext::ExitGlobalSettingsUpdateContext()
{
    logf("[Mock] GlobalContext::ExitGlobalSettingsUpdateContext");
    
    // Mock implementation: exit update context
    return api_true;
}

api_bool GlobalContext::CancelGlobalSettingsUpdate(api_handle hModule, uint32 reserved)
{
    logf("[Mock] GlobalContext::CancelGlobalSettingsUpdate");
    
    // Mock implementation: cancel update
    return api_true;
}

void GlobalContext::BroadcastGlobalFiltersUpdated(const void* reserved)
{
    logf("[Mock] GlobalContext::BroadcastGlobalFiltersUpdated");
    
    // Mock implementation: broadcast notification
}

// =============================================================
// ControlContext - Additional Control Property Stubs
// =============================================================

void ControlContext::SetControlFont(control_handle control, const_font_handle font)
{
    logf("[Mock] ControlContext::SetControlFont");
    
    QWidget* w = widgetFromHandle(control);
    if (w && font)
    {
        // In a full implementation, we'd convert the font_handle to QFont
        // For now, just set a default monospace font
        QFont qfont("Courier New", 10);
        w->setFont(qfont);
    }
}

void ControlContext::SetControlButtonColor(control_handle control, uint32 color)
{
    logf("[Mock] ControlContext::SetControlButtonColor (0x%08X)", color);
    
    QWidget* w = widgetFromHandle(control);
    if (w)
    {
        // Extract RGB from RGBA color
        int r = (color >> 16) & 0xFF;
        int g = (color >> 8) & 0xFF;
        int b = color & 0xFF;
        
        QPalette palette = w->palette();
        palette.setColor(QPalette::Button, QColor(r, g, b));
        w->setPalette(palette);
    }
}

uint32 ControlContext::GetControlBackgroundColor(const_control_handle control)
{
    logf("[Mock] ControlContext::GetControlBackgroundColor");
    
    QWidget* w = widgetFromHandle(const_cast<control_handle>(control));
    if (w)
    {
        QColor color = w->palette().color(QPalette::Window);
        // Convert to RGBA format
        return (color.red() << 16) | (color.green() << 8) | color.blue() | 0xFF000000;
    }
    
    return 0xFFFFFFFF;  // Default white
}

void ControlContext::SetControlMouseTrackingEnabled(control_handle control, api_bool enabled)
{
    logf("[Mock] ControlContext::SetControlMouseTrackingEnabled (%s)",
         enabled ? "enabled" : "disabled");
    
    QWidget* w = widgetFromHandle(control);
    if (w)
    {
        w->setMouseTracking(enabled != 0);
    }
}

// =============================================================
// SpinBoxContext - Additional Stubs
// =============================================================

void SpinBoxContext::SetSpinBoxStepSize(control_handle control, int32 step)
{
    logf("[Mock] SpinBoxContext::SetSpinBoxStepSize (step=%d)", step);
    
    QWidget* w = widgetFromHandle(control);
    QSpinBox* spinBox = qobject_cast<QSpinBox*>(w);
    if (spinBox)
    {
        spinBox->setSingleStep(step);
    }
}

void SpinBoxContext::SetSpinBoxMinimumValueText(control_handle control,
                                                const char16_type* text)
{
    logf("[Mock] SpinBoxContext::SetSpinBoxMinimumValueText");
    
    QWidget* w = widgetFromHandle(control);
    QSpinBox* spinBox = qobject_cast<QSpinBox*>(w);
    if (spinBox && text)
    {
        QString qtext = QString::fromUtf16(text);
        spinBox->setSpecialValueText(qtext);
    }
}

// =============================================================
// WebViewContext - WebView Operation Stubs
// =============================================================

api_bool WebViewContext::SaveWebViewAsPDF(control_handle control,
                                          const char16_type* filePath,
                                          const double* pageWidth,
                                          const double* pageHeight,
                                          const double* marginLeft,
                                          const double* marginTop,
                                          const double* marginRight,
                                          const double* marginBottom,
                                          int32 orientation)
{
    logf("[Mock] WebViewContext::SaveWebViewAsPDF (orientation=%d)", orientation);
    
    // Mock implementation: pretend to save PDF
    return api_true;
}

api_bool WebViewContext::LoadWebViewContent(control_handle control,
                                            const char16_type* URI)
{
    logf("[Mock] WebViewContext::LoadWebViewContent");
    
    // Mock implementation: pretend to load content
    return api_true;
}

api_bool WebViewContext::SetWebViewZoomFactor(control_handle control,
                                              const double* zoomFactor)
{
    logf("[Mock] WebViewContext::SetWebViewZoomFactor (zoom=%.2f)",
         zoomFactor ? *zoomFactor : 1.0);
    
    // Mock implementation: accept zoom factor
    return api_true;
}

api_bool WebViewContext::SetWebViewLoadFinishedEventRoutine(control_handle control,
                                                            api_handle receiver,
                                                            pcl::state_event_routine callback)
{
    logf("[Mock] WebViewContext::SetWebViewLoadFinishedEventRoutine");
    
    // Mock implementation: store callback
    return api_true;
}

// =============================================================
// ImageWindowContext - Swap Directory Stubs
// =============================================================

api_bool ImageWindowContext::GetSwapDirectory(int32 index, char16_type* dir,
                                              size_type* length)
{
    logf("[Mock] ImageWindowContext::GetSwapDirectory (index=%d)", index);
    
    if (index == 0 && dir && length)
    {
        // Mock implementation: return a default swap directory
        const char16_t* defaultDir = u"/tmp/pixinsight_swap";
        size_t len = std::char_traits<char16_t>::length(defaultDir);
        
        if (*length > len)
        {
            std::memcpy(dir, defaultDir, (len + 1) * sizeof(char16_type));
            *length = len + 1;
            return api_true;
        }
        else
        {
            *length = len + 1;
            return api_false;
        }
    }
    
    return api_false;  // No more swap directories
}

api_bool ImageWindowContext::SetSwapDirectories(const char16_type** dirs, int32 count)
{
    logf("[Mock] ImageWindowContext::SetSwapDirectories (count=%d)", count);
    
    // Mock implementation: accept swap directories
    return api_true;
}

// =============================================================
// END OF GLOBAL PROJECT STUBS
// =============================================================

// =============================================================
// PenContext - Additional Stub
// =============================================================

uint32 PenContext::GetPenColor(const_pen_handle pen)
{
    logf("[Mock] PenContext::GetPenColor");
    
    // Mock implementation: return black color
    return 0xFF000000;  // RGBA: opaque black
}

// =============================================================
// FontContext - Additional Stub
// =============================================================

void FontContext::SetFontPixelSize(font_handle font, int32 size)
{
    logf("[Mock] FontContext::SetFontPixelSize (size=%d)", size);
    
    // Mock implementation: accept pixel size
    // In real implementation, this would modify the font handle
}

// =============================================================
// ViewContext - Additional Stub
// =============================================================

api_bool ViewContext::IsStoredPreview(const_view_handle view)
{
    logf("[Mock] ViewContext::IsStoredPreview");
    
    // Mock implementation: not a stored preview
    return api_false;
}

// =============================================================
// ActionContext - Additional Stub
// =============================================================

void ActionContext::SetActionIcon(action_handle action, const_bitmap_handle icon)
{
    logf("[Mock] ActionContext::SetActionIcon");
    
    // Mock implementation: accept icon
    // In real implementation, this would set the action's icon
}

// =============================================================
// GlobalContext - Additional Settings and Utility Stubs
// =============================================================

api_bool GlobalContext::ReadSettingsReal(api_handle hModule, double* value,
                                         const char* key, uint32 global)
{
    logf("[Mock] GlobalContext::ReadSettingsReal (key=%s, global=%s)",
         key ? key : "null", global ? "true" : "false");
    
    // Mock implementation: setting not found
    if (value)
        *value = 0.0;
    
    return api_false;
}

api_bool GlobalContext::WriteSettingsReal(api_handle hModule, double value,
                                          const char* key, uint32 global)
{
    logf("[Mock] GlobalContext::WriteSettingsReal (key=%s, value=%.6f, global=%s)",
         key ? key : "null", value, global ? "true" : "false");
    
    // Mock implementation: accept write
    return api_true;
}

void GlobalContext::GetCursorPosition(int32* x, int32* y)
{
    logf("[Mock] GlobalContext::GetCursorPosition");
    
    // Mock implementation: return cursor position
    QPoint pos = QCursor::pos();
    if (x) *x = pos.x();
    if (y) *y = pos.y();
}

void GlobalContext::HideToolTipWindow()
{
    logf("[Mock] GlobalContext::HideToolTipWindow");
    
    // Mock implementation: hide tooltip
    QToolTip::hideText();
}

void GlobalContext::ShowToolTipWindow(int32 x, int32 y, const char16_type* text,
                                      const_control_handle control,
                                      int32 rectX, int32 rectY,
                                      int32 rectW, int32 rectH)
{
    logf("[Mock] GlobalContext::ShowToolTipWindow (%d, %d)", x, y);
    
    // Mock implementation: show tooltip
    if (text)
    {
        QString qtext = QString::fromUtf16(text);
        QToolTip::showText(QPoint(x, y), qtext);
    }
}

api_bool GlobalContext::LaunchProcessInterface(meta_interface_handle interface, uint32 flags)
{
    logf("[Mock] GlobalContext::LaunchProcessInterface (flags=0x%08X)", flags);
    
    // Mock implementation: pretend to launch interface
    return api_true;
}

// =============================================================
// ControlContext - Additional Event Stubs
// =============================================================

void ControlContext::UpdateControlRect(control_handle control,
                                       int32 x, int32 y, int32 w, int32 h)
{
    logf("[Mock] ControlContext::UpdateControlRect (%d, %d, %d x %d)", x, y, w, h);
    
    QWidget* widget = widgetFromHandle(control);
    if (widget)
    {
        widget->update(x, y, w, h);
    }
}

api_bool ControlContext::SetEnterEventRoutine(control_handle control,
                                              control_handle receiver,
                                              pcl::control_event_routine callback)
{
    logf("[Mock] ControlContext::SetEnterEventRoutine");
    
    MockBase* base = get(control);
    if (base && base->widget)
    {
        // Store callback
        base->onEnter = callback;
        return api_true;
    }
    return api_false;
}

api_bool ControlContext::SetLeaveEventRoutine(control_handle control,
                                              control_handle receiver,
                                              pcl::control_event_routine callback)
{
    logf("[Mock] ControlContext::SetLeaveEventRoutine");
    
    MockBase* base = get(control);
    if (base && base->widget)
    {
        // Store callback
        base->onLeave = callback;
        return api_true;
    }
    return api_false;
}

api_bool ControlContext::SetResizeEventRoutine(control_handle control,
                                               control_handle receiver,
                                               pcl::resize_event_routine callback)
{
    logf("[Mock] ControlContext::SetResizeEventRoutine");
    
    MockBase* base = get(control);
    if (base && base->widget)
    {
        // Store callback
        base->onResize = callback;
        return api_true;
    }
    return api_false;
}

api_bool ControlContext::SetKeyReleaseEventRoutine(control_handle control,
                                                   control_handle receiver,
                                                   pcl::keyboard_event_routine callback)
{
    logf("[Mock] ControlContext::SetKeyReleaseEventRoutine");
    
    MockBase* base = get(control);
    if (base && base->widget)
    {
        // Store callback
        base->onKeyRelease = callback;
        return api_true;
    }
    return api_false;
}

// =============================================================
// GraphicsContext - Additional Drawing Stubs
// =============================================================

void GraphicsContext::DrawBitmap(graphics_handle handle, int32 x, int32 y,
                                const_bitmap_handle bitmap)
{
    logf("[Mock] GraphicsContext::DrawBitmap (%d, %d)", x, y);
    // No-op - graphics not rendered in mock
}

void GraphicsContext::DrawPolygon(graphics_handle handle, const int32* points,
                                 size_type count, int32 fillRule)
{
    logf("[Mock] GraphicsContext::DrawPolygon (count=%zu, fillRule=%d)",
         (size_t)count, fillRule);
    // No-op - graphics not rendered in mock
}

void GraphicsContext::DrawPolyline(graphics_handle handle, const int32* points,
                                  size_type count)
{
    logf("[Mock] GraphicsContext::DrawPolyline (count=%zu)", (size_t)count);
    // No-op - graphics not rendered in mock
}

pen_handle GraphicsContext::GetGraphicsPen(const_graphics_handle handle)
{
    logf("[Mock] GraphicsContext::GetGraphicsPen");
    
    // Mock implementation: return a dummy pen handle
    static int dummyPen = 0;
    return &dummyPen;
}

void GraphicsContext::SetGraphicsFont(graphics_handle handle, const_font_handle font)
{
    logf("[Mock] GraphicsContext::SetGraphicsFont");
    // No-op - graphics not rendered in mock
}

void GraphicsContext::SetGraphicsOpacity(graphics_handle handle, double opacity)
{
    logf("[Mock] GraphicsContext::SetGraphicsOpacity (opacity=%.2f)", opacity);
    // No-op - graphics not rendered in mock
}

void GraphicsContext::DrawText(graphics_handle handle, int32 x, int32 y,
                              const char16_type* text)
{
    logf("[Mock] GraphicsContext::DrawText (%d, %d)", x, y);
    // No-op - graphics not rendered in mock
}

void GraphicsContext::DrawPoint(graphics_handle handle, int32 x, int32 y)
{
    logf("[Mock] GraphicsContext::DrawPoint (%d, %d)", x, y);
    // No-op - graphics not rendered in mock
}

// =============================================================
// ScrollBoxContext - Comprehensive ScrollBox Stubs
// =============================================================

void ScrollBoxContext::GetScrollBoxPageSize(const_control_handle control,
                                           int32* width, int32* height)
{
    logf("[Mock] ScrollBoxContext::GetScrollBoxPageSize");
    
    QWidget* w = widgetFromHandle(const_cast<control_handle>(control));
    QScrollArea* scrollArea = qobject_cast<QScrollArea*>(w);
    if (scrollArea)
    {
        QSize pageSize = scrollArea->viewport()->size();
        if (width) *width = pageSize.width();
        if (height) *height = pageSize.height();
    }
    else
    {
        if (width) *width = 0;
        if (height) *height = 0;
    }
}

void ScrollBoxContext::GetScrollBoxPosition(const_control_handle control,
                                           int32* x, int32* y)
{
    logf("[Mock] ScrollBoxContext::GetScrollBoxPosition");
    
    QWidget* w = widgetFromHandle(const_cast<control_handle>(control));
    QScrollArea* scrollArea = qobject_cast<QScrollArea*>(w);
    if (scrollArea)
    {
        if (x) *x = scrollArea->horizontalScrollBar()->value();
        if (y) *y = scrollArea->verticalScrollBar()->value();
    }
    else
    {
        if (x) *x = 0;
        if (y) *y = 0;
    }
}

void ScrollBoxContext::SetScrollBarsVisible(control_handle control,
                                           uint32 horizontal, uint32 vertical)
{
    logf("[Mock] ScrollBoxContext::SetScrollBarsVisible (h=%s, v=%s)",
         horizontal ? "visible" : "hidden", vertical ? "visible" : "hidden");
    
    QWidget* w = widgetFromHandle(control);
    QScrollArea* scrollArea = qobject_cast<QScrollArea*>(w);
    if (scrollArea)
    {
        scrollArea->setHorizontalScrollBarPolicy(
            horizontal ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
        scrollArea->setVerticalScrollBarPolicy(
            vertical ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
    }
}

void ScrollBoxContext::SetScrollBoxPageSize(control_handle control,
                                           int32 width, int32 height)
{
    logf("[Mock] ScrollBoxContext::SetScrollBoxPageSize (%d x %d)", width, height);
    
    QWidget* w = widgetFromHandle(control);
    QScrollArea* scrollArea = qobject_cast<QScrollArea*>(w);
    if (scrollArea && scrollArea->widget())
    {
        scrollArea->widget()->resize(width, height);
    }
}

void ScrollBoxContext::SetScrollBoxPosition(control_handle control, int32 x, int32 y)
{
    logf("[Mock] ScrollBoxContext::SetScrollBoxPosition (%d, %d)", x, y);
    
    QWidget* w = widgetFromHandle(control);
    QScrollArea* scrollArea = qobject_cast<QScrollArea*>(w);
    if (scrollArea)
    {
        scrollArea->horizontalScrollBar()->setValue(x);
        scrollArea->verticalScrollBar()->setValue(y);
    }
}

void ScrollBoxContext::SetScrollBoxVerticalRange(control_handle control,
                                                 int32 min, int32 max)
{
    logf("[Mock] ScrollBoxContext::SetScrollBoxVerticalRange (%d, %d)", min, max);
    
    QWidget* w = widgetFromHandle(control);
    QScrollArea* scrollArea = qobject_cast<QScrollArea*>(w);
    if (scrollArea)
    {
        scrollArea->verticalScrollBar()->setRange(min, max);
    }
}

void ScrollBoxContext::SetScrollBoxHorizontalRange(control_handle control,
                                                   int32 min, int32 max)
{
    logf("[Mock] ScrollBoxContext::SetScrollBoxHorizontalRange (%d, %d)", min, max);
    
    QWidget* w = widgetFromHandle(control);
    QScrollArea* scrollArea = qobject_cast<QScrollArea*>(w);
    if (scrollArea)
    {
        scrollArea->horizontalScrollBar()->setRange(min, max);
    }
}

void ScrollBoxContext::SetScrollBoxAutoScrollEnabled(control_handle control,
                                                    uint32 horizontal, uint32 vertical)
{
    logf("[Mock] ScrollBoxContext::SetScrollBoxAutoScrollEnabled (h=%s, v=%s)",
         horizontal ? "enabled" : "disabled", vertical ? "enabled" : "disabled");
    
    // Mock implementation: accept auto-scroll settings
    // QScrollArea doesn't have a direct auto-scroll property
}

api_bool ScrollBoxContext::SetScrollBoxVerticalPosUpdatedEventRoutine(
    control_handle control,
    api_handle receiver,
    pcl::value_event_routine callback)
{
    logf("[Mock] ScrollBoxContext::SetScrollBoxVerticalPosUpdatedEventRoutine");
    
    QWidget* w = widgetFromHandle(control);
    QScrollArea* scrollArea = qobject_cast<QScrollArea*>(w);
    if (scrollArea)
    {
        MockBase* base = get(control);
        if (base)
        {
            QObject::connect(scrollArea->verticalScrollBar(), &QScrollBar::valueChanged,
                [base, callback](int value)
                {
                    if (callback)
                    {
                        callback(base->pcl_handle, base->pcl_handle, value);
                    }
                });
            return api_true;
        }
    }
    return api_false;
}

api_bool ScrollBoxContext::SetScrollBoxHorizontalPosUpdatedEventRoutine(
    control_handle control,
    api_handle receiver,
    pcl::value_event_routine callback)
{
    logf("[Mock] ScrollBoxContext::SetScrollBoxHorizontalPosUpdatedEventRoutine");
    
    QWidget* w = widgetFromHandle(control);
    QScrollArea* scrollArea = qobject_cast<QScrollArea*>(w);
    if (scrollArea)
    {
        MockBase* base = get(control);
        if (base)
        {
            QObject::connect(scrollArea->horizontalScrollBar(), &QScrollBar::valueChanged,
                [base, callback](int value)
                {
                    if (callback)
                    {
                        callback(base->pcl_handle, base->pcl_handle, value);
                    }
                });
            return api_true;
        }
    }
    return api_false;
}

// =============================================================
// RealTimePreviewContext - Additional Stub
// =============================================================

api_bool RealTimePreviewContext::IsRealTimePreviewUpdating()
{
    logf("[Mock] RealTimePreviewContext::IsRealTimePreviewUpdating");
    
    // Mock implementation: not updating
    return api_false;
}

// =============================================================
// END OF INTENSITY TRANSFORMATIONS STUBS
// =============================================================

// =============================================================
// CursorContext - Bitmap Cursor Creation Stub
// =============================================================

cursor_handle CursorContext::CreateBitmapCursor(api_handle module,
                                               const_bitmap_handle bitmap,
                                               int32 hx, int32 hy)
{
    logf("[Mock] CursorContext::CreateBitmapCursor (hotspot: %d, %d)", hx, hy);
    
    // Mock implementation: return a dummy cursor handle
    static int dummyCursor = 0;
    return &dummyCursor;
}

// =============================================================
// GlobalContext - Settings Block I/O Stubs
// =============================================================

api_bool GlobalContext::ReadSettingsBlock(api_handle hModule, void** data,
                                          size_type* size,
                                          const char* key, uint32 global)
{
    logf("[Mock] GlobalContext::ReadSettingsBlock (key=%s, global=%s)",
         key ? key : "null", global ? "true" : "false");
    
    // Mock implementation: setting not found
    if (data) *data = nullptr;
    if (size) *size = 0;
    
    return api_false;
}

api_bool GlobalContext::WriteSettingsBlock(api_handle hModule,
                                           const void* data, size_type size,
                                           const char* key, uint32 global)
{
    logf("[Mock] GlobalContext::WriteSettingsBlock (key=%s, size=%zu, global=%s)",
         key ? key : "null", (size_t)size, global ? "true" : "false");
    
    // Mock implementation: accept write
    return api_true;
}

// =============================================================
// ControlContext - Window Title Stub
// =============================================================

api_bool ControlContext::GetWindowTitle(const_control_handle control,
                                       char16_type* title, size_type* length)
{
    logf("[Mock] ControlContext::GetWindowTitle");
    
    QWidget* w = widgetFromHandle(const_cast<control_handle>(control));
    if (!w)
        return api_false;
    
    QString windowTitle = w->windowTitle();
    
    if (title && length)
    {
        // Convert to UTF-16 and copy
        std::u16string u16str = windowTitle.toStdU16String();
        size_type copyLen = std::min(*length - 1, u16str.length());
        std::memcpy(title, u16str.c_str(), copyLen * sizeof(char16_type));
        title[copyLen] = 0;
        *length = copyLen;
    }
    else if (length)
    {
        // Just return the required length
        *length = windowTitle.length() + 1;
    }
    
    return api_true;
}

// =============================================================
// GraphicsContext - Background Brush Stub
// =============================================================

brush_handle GraphicsContext::GetGraphicsBackgroundBrush(const_graphics_handle handle)
{
    logf("[Mock] GraphicsContext::GetGraphicsBackgroundBrush");
    
    // Mock implementation: return a dummy brush handle
    static int dummyBrush = 0;
    return &dummyBrush;
}

// =============================================================
// END OF MORPHOLOGY STUBS
// =============================================================

// =============================================================
// EditContext - Password Mode Stub
// =============================================================

void EditContext::SetEditPasswordEnabled(control_handle control, uint32 enabled)
{
    logf("[Mock] EditContext::SetEditPasswordEnabled (%s)",
         enabled ? "enabled" : "disabled");
    
    QWidget* w = widgetFromHandle(control);
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(w);
    if (lineEdit)
    {
        lineEdit->setEchoMode(enabled ? QLineEdit::Password : QLineEdit::Normal);
    }
}

// =============================================================
// NetworkTransferContext - SSL and POST Stubs
// =============================================================

api_bool NetworkTransferContext::SetNetworkTransferSSL(network_transfer_handle handle,
                                                       uint32 useSSL,
                                                       uint32 forceSSL,
                                                       uint32 verifyPeer,
                                                       uint32 verifyHost)
{
    logf("[Mock] NetworkTransferContext::SetNetworkTransferSSL (useSSL=%s, forceSSL=%s, verifyPeer=%s, verifyHost=%s)",
         useSSL ? "true" : "false",
         forceSSL ? "true" : "false",
         verifyPeer ? "true" : "false",
         verifyHost ? "true" : "false");
    
    // Mock implementation: accept SSL settings
    return api_true;
}

api_bool NetworkTransferContext::PerformNetworkTransferPOST(network_transfer_handle handle,
                                                            const char16_type* postFields)
{
    logf("[Mock] NetworkTransferContext::PerformNetworkTransferPOST");
    
    // Mock implementation: pretend to perform POST request
    return api_true;
}

// =============================================================
// END OF NETWORKSERVICE STUBS
// =============================================================

// =============================================================
// TabBoxContext - Additional TabBox Stubs
// =============================================================

void TabBoxContext::SetTabBoxPageIcon(control_handle control, int32 index,
                                     const_bitmap_handle icon)
{
    logf("[Mock] TabBoxContext::SetTabBoxPageIcon (index=%d)", index);
    
    QWidget* w = widgetFromHandle(control);
    QTabWidget* tabWidget = qobject_cast<QTabWidget*>(w);
    if (tabWidget && index >= 0 && index < tabWidget->count())
    {
        // In a full implementation, would convert bitmap_handle to QIcon
        // For now, just accept the icon
        // tabWidget->setTabIcon(index, qicon);
    }
}

void TabBoxContext::SetTabBoxPageLabel(control_handle control, int32 index,
                                       const char16_type* label)
{
    logf("[Mock] TabBoxContext::SetTabBoxPageLabel (index=%d)", index);
    
    QWidget* w = widgetFromHandle(control);
    QTabWidget* tabWidget = qobject_cast<QTabWidget*>(w);
    if (tabWidget && index >= 0 && index < tabWidget->count() && label)
    {
        QString qtext = QString::fromUtf16(label);
        tabWidget->setTabText(index, qtext);
    }
}

void TabBoxContext::SetTabBoxPageEnabled(control_handle control, int32 index,
                                        uint32 enabled)
{
    logf("[Mock] TabBoxContext::SetTabBoxPageEnabled (index=%d, enabled=%s)",
         index, enabled ? "true" : "false");
    
    QWidget* w = widgetFromHandle(control);
    QTabWidget* tabWidget = qobject_cast<QTabWidget*>(w);
    if (tabWidget && index >= 0 && index < tabWidget->count())
    {
        tabWidget->setTabEnabled(index, enabled != 0);
    }
}

int32 TabBoxContext::GetTabBoxCurrentPageIndex(const_control_handle control)
{
    logf("[Mock] TabBoxContext::GetTabBoxCurrentPageIndex");
    
    QWidget* w = widgetFromHandle(const_cast<control_handle>(control));
    QTabWidget* tabWidget = qobject_cast<QTabWidget*>(w);
    if (tabWidget)
    {
        return tabWidget->currentIndex();
    }
    
    return -1;
}

// =============================================================
// CodeEditorContext - Additional Code Editor Stubs
// =============================================================

api_bool CodeEditorContext::GetEditorText(const_control_handle control,
                                          char16_type* text, size_type* length)
{
    logf("[Mock] CodeEditorContext::GetEditorText");
    
    QWidget* w = widgetFromHandle(const_cast<control_handle>(control));
    QTextEdit* editor = qobject_cast<QTextEdit*>(w);
    if (!editor)
        return api_false;
    
    QString editorText = editor->toPlainText();
    
    if (text && length)
    {
        // Convert to UTF-16 and copy
        std::u16string u16str = editorText.toStdU16String();
        size_type copyLen = std::min(*length - 1, u16str.length());
        std::memcpy(text, u16str.c_str(), copyLen * sizeof(char16_type));
        text[copyLen] = 0;
        *length = copyLen;
    }
    else if (length)
    {
        // Just return the required length
        *length = editorText.length() + 1;
    }
    
    return api_true;
}

void CodeEditorContext::InsertEditorText(control_handle control, const char16_type* text)
{
    logf("[Mock] CodeEditorContext::InsertEditorText");
    
    QWidget* w = widgetFromHandle(control);
    QTextEdit* editor = qobject_cast<QTextEdit*>(w);
    if (editor && text)
    {
        QString qtext = QString::fromUtf16(text);
        editor->insertPlainText(qtext);
    }
}

void CodeEditorContext::SetEditorFilePath(control_handle control, const char16_type* path)
{
    logf("[Mock] CodeEditorContext::SetEditorFilePath");
    
    // Mock implementation: accept file path
    // In real implementation, this would track the file path for the editor
}

api_bool CodeEditorContext::GetEditorSelectedText(const_control_handle control,
                                                  char16_type* text, size_type* length)
{
    logf("[Mock] CodeEditorContext::GetEditorSelectedText");
    
    QWidget* w = widgetFromHandle(const_cast<control_handle>(control));
    QTextEdit* editor = qobject_cast<QTextEdit*>(w);
    if (!editor)
        return api_false;
    
    QTextCursor cursor = editor->textCursor();
    QString selectedText = cursor.selectedText();
    
    if (text && length)
    {
        // Convert to UTF-16 and copy
        std::u16string u16str = selectedText.toStdU16String();
        size_type copyLen = std::min(*length - 1, u16str.length());
        std::memcpy(text, u16str.c_str(), copyLen * sizeof(char16_type));
        text[copyLen] = 0;
        *length = copyLen;
    }
    else if (length)
    {
        // Just return the required length
        *length = selectedText.length() + 1;
    }
    
    return api_true;
}

void CodeEditorContext::SetEditorCursorCoordinates(control_handle control,
                                                   int32 line, int32 col)
{
    logf("[Mock] CodeEditorContext::SetEditorCursorCoordinates (line=%d, col=%d)",
         line, col);
    
    QWidget* w = widgetFromHandle(control);
    QTextEdit* editor = qobject_cast<QTextEdit*>(w);
    if (editor)
    {
        // Move cursor to specified line and column
        QTextCursor cursor = editor->textCursor();
        cursor.movePosition(QTextCursor::Start);
        
        // Move to line
        for (int32 i = 0; i < line; ++i)
        {
            cursor.movePosition(QTextCursor::Down);
        }
        
        // Move to column
        for (int32 i = 0; i < col; ++i)
        {
            cursor.movePosition(QTextCursor::Right);
        }
        
        editor->setTextCursor(cursor);
    }
}

void CodeEditorContext::SetEditorSelectionCoordinates(control_handle control,
                                                      int32 fromLine, int32 fromCol,
                                                      int32 toLine, int32 toCol)
{
    logf("[Mock] CodeEditorContext::SetEditorSelectionCoordinates (from: %d,%d to: %d,%d)",
         fromLine, fromCol, toLine, toCol);
    
    QWidget* w = widgetFromHandle(control);
    QTextEdit* editor = qobject_cast<QTextEdit*>(w);
    if (editor)
    {
        // Create selection from fromLine:fromCol to toLine:toCol
        QTextCursor cursor = editor->textCursor();
        
        // Move to start position
        cursor.movePosition(QTextCursor::Start);
        for (int32 i = 0; i < fromLine; ++i)
            cursor.movePosition(QTextCursor::Down);
        for (int32 i = 0; i < fromCol; ++i)
            cursor.movePosition(QTextCursor::Right);
        
        // Select to end position
        int32 lineDiff = toLine - fromLine;
        for (int32 i = 0; i < lineDiff; ++i)
            cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor);
        
        // Adjust column position
        int32 currentCol = (lineDiff == 0) ? fromCol : 0;
        int32 colDiff = toCol - currentCol;
        if (colDiff > 0)
        {
            for (int32 i = 0; i < colDiff; ++i)
                cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
        }
        else if (colDiff < 0)
        {
            for (int32 i = 0; i < -colDiff; ++i)
                cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
        }
        
        editor->setTextCursor(cursor);
    }
}

control_handle CodeEditorContext::CreateEditorLineNumbersControl(control_handle editor,
                                                                 control_handle client,
                                                                 control_handle parent,
                                                                 uint32 flags)
{
    logf("[Mock] CodeEditorContext::CreateEditorLineNumbersControl");
    
    // Mock implementation: create a simple label widget for line numbers
    QWidget* parentWidget = determineParentWidget(parent);
    QLabel* lineNumbers = new QLabel(parentWidget);
    lineNumbers->setAlignment(Qt::AlignRight | Qt::AlignTop);
    lineNumbers->setText("1\n2\n3\n4\n5");
    
    QFont monoFont("Courier New", 10);
    lineNumbers->setFont(monoFont);
    
    // Create mock wrapper
    auto base = std::make_unique<MockBase>();
    base->pcl_handle = client;
    base->widget = lineNumbers;
    
    control_handle handle = client;
    g_objects[handle] = std::move(base);
    
    return handle;
}

// =============================================================
// ImageWindowContext - Celestial Coordinate Conversion Stub
// =============================================================

api_bool ImageWindowContext::ImageToCelestial(const_window_handle window_context,
                                              double* ra, double* dec, uint32 rawRA)
{
    logf("[Mock] ImageWindowContext::ImageToCelestial (rawRA=%s)",
         rawRA ? "true" : "false");
    
    // Mock implementation: no astrometric solution available
    // In real implementation, this would convert image coordinates to RA/Dec
    // using WCS (World Coordinate System) metadata
    
    return api_false;  // No astrometric solution
}

// =============================================================
// END OF PIXELMATH STUBS
// =============================================================


// =============================================================
// GlobalContext - Console Flush Stub
// =============================================================

api_bool GlobalContext::FlushConsole(console_handle console)
{
    logf("[Mock] GlobalContext::FlushConsole");
    
    // In real implementation, this would flush buffered console output
    
    return api_true;
}

// =============================================================
// ProcessContext - Version Stub
// =============================================================

uint32 ProcessContext::GetProcessInstanceVersion(const_process_handle process)
{
    logf("[Mock] ProcessContext::GetProcessInstanceVersion");
    
    // Mock implementation: return a default version
    // Format is typically 0xMMmmRRBB (Major.minor.Release.Beta)
    return 0x01000000;  // Version 1.0.0.0
}

// =============================================================
// WebViewContext - Content and Script Stubs
// =============================================================

api_bool WebViewContext::SetWebViewContent(control_handle control,
                                           const void* data, size_type size,
                                           const char* mimeType)
{
    logf("[Mock] WebViewContext::SetWebViewContent (size=%zu, mimeType=%s)",
         (size_t)size, mimeType ? mimeType : "null");
    
    // Mock implementation: accept content
    // In real implementation, this would load HTML/text content into the web view
    return api_true;
}

api_bool WebViewContext::EvaluateWebViewScript(control_handle control,
                                               const char16_type* sourceCode,
                                               const char* language)
{
    logf("[Mock] WebViewContext::EvaluateWebViewScript (language=%s)",
         language ? language : "null");
    
    // Mock implementation: pretend to evaluate JavaScript
    return api_true;
}

api_bool WebViewContext::SetWebViewBackgroundColor(control_handle control,
                                                   uint32 color)
{
    logf("[Mock] WebViewContext::SetWebViewBackgroundColor (0x%08X)", color);
    
    // Mock implementation: accept background color
    // In real implementation, this would set the web view's background color
    return api_true;
}

api_bool WebViewContext::SetWebViewScriptResultAvailableEventRoutine(
    control_handle control,
    api_handle receiver,
    pcl::property_event_routine callback)
{
    logf("[Mock] WebViewContext::SetWebViewScriptResultAvailableEventRoutine");
    
    // Mock implementation: store callback
    // In real implementation, this would be called when JavaScript evaluation completes
    return api_true;
}

// =============================================================
// CodeEditorContext - Text Updated Event Stub
// =============================================================

api_bool CodeEditorContext::SetEditorTextUpdatedEventRoutine(control_handle control,
                                                             api_handle receiver,
                                                             pcl::event_routine callback)
{
    logf("[Mock] CodeEditorContext::SetEditorTextUpdatedEventRoutine");
    
    QWidget* w = widgetFromHandle(control);
    QTextEdit* editor = qobject_cast<QTextEdit*>(w);
    if (editor)
    {
        MockBase* base = get(control);
        if (base)
        {
            // Connect to textChanged signal
            QObject::connect(editor, &QTextEdit::textChanged,
                [base, callback]()
                {
                    if (callback)
                    {
                        callback(base->pcl_handle, base->pcl_handle);
                    }
                });
            return api_true;
        }
    }
    return api_false;
}

// =============================================================
// END OF SUBFRAMESELECTOR STUBS
// =============================================================

// =============================================================
// EditContext - Alignment and Event Stubs
// =============================================================

void EditContext::SetEditAlignment(control_handle control, int32 alignment)
{
    logf("[Mock] EditContext::SetEditAlignment (alignment=%d)", alignment);
    
    QWidget* w = widgetFromHandle(control);
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(w);
    if (lineEdit)
    {
        // Convert PCL alignment to Qt alignment
        // 0 = left, 1 = right, 2 = center
        Qt::Alignment qtAlign = Qt::AlignLeft;
        if (alignment == 1)
            qtAlign = Qt::AlignRight;
        else if (alignment == 2)
            qtAlign = Qt::AlignCenter;
        
        lineEdit->setAlignment(qtAlign);
    }
}

api_bool EditContext::SetTextUpdatedEventRoutine(control_handle control,
                                                 control_handle receiver,
                                                 pcl::unicode_event_routine callback)
{
    logf("[Mock] EditContext::SetTextUpdatedEventRoutine");
    
    QWidget* w = widgetFromHandle(control);
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(w);
    if (lineEdit)
    {
        MockBase* base = get(control);
        if (base)
        {
            // Connect to textChanged signal
            QObject::connect(lineEdit, &QLineEdit::textChanged,
                [base, callback](const QString& text)
                {
                    if (callback)
                    {
                        std::u16string u16str = text.toStdU16String();
                        callback(base->pcl_handle, base->pcl_handle,
                                reinterpret_cast<const char16_type*>(u16str.c_str()));
                    }
                });
            return api_true;
        }
    }
    return api_false;
}

// =============================================================
// GlobalContext - Abort Control Stub
// =============================================================

api_bool GlobalContext::DisableAbort()
{
    logf("[Mock] GlobalContext::DisableAbort");
    
    // Mock implementation: disable abort capability
    return api_true;
}

// =============================================================
// FontContext - Font Style Stubs
// =============================================================

void FontContext::SetFontItalic(font_handle font, uint32 italic)
{
    logf("[Mock] FontContext::SetFontItalic (%s)", italic ? "true" : "false");
    
    // Mock implementation: accept italic setting
    // In real implementation, this would modify the font handle
}

void FontContext::SetFontUnderline(font_handle font, uint32 underline)
{
    logf("[Mock] FontContext::SetFontUnderline (%s)", underline ? "true" : "false");
    
    // Mock implementation: accept underline setting
    // In real implementation, this would modify the font handle
}

// =============================================================
// GraphicsContext - Additional Bitmap Drawing Stubs
// =============================================================

void GraphicsContext::DrawBitmapD(graphics_handle handle, double x, double y,
                                  const_bitmap_handle bitmap)
{
    logf("[Mock] GraphicsContext::DrawBitmapD (%.2f, %.2f)", x, y);
    // No-op - graphics not rendered in mock
}

font_handle GraphicsContext::GetGraphicsFont(const_graphics_handle handle)
{
    logf("[Mock] GraphicsContext::GetGraphicsFont");
    
    // Mock implementation: return a dummy font handle
    static int dummyFont = 0;
    return &dummyFont;
}

void GraphicsContext::DrawScaledBitmapD(graphics_handle handle,
                                        double x, double y,
                                        double width, double height,
                                        const_bitmap_handle bitmap)
{
    logf("[Mock] GraphicsContext::DrawScaledBitmapD (%.2f, %.2f, %.2f x %.2f)",
         x, y, width, height);
    // No-op - graphics not rendered in mock
}

// =============================================================
// ControlContext - ToolTip Getter Stub
// =============================================================

api_bool ControlContext::GetWindowToolTip(const_control_handle control,
                                         char16_type* tooltip, size_type* length)
{
    logf("[Mock] ControlContext::GetWindowToolTip");
    
    QWidget* w = widgetFromHandle(const_cast<control_handle>(control));
    if (!w)
        return api_false;
    
    QString qtTooltip = w->toolTip();
    
    if (tooltip && length)
    {
        // Convert to UTF-16 and copy
        std::u16string u16str = qtTooltip.toStdU16String();
        size_type copyLen = std::min(*length - 1, u16str.length());
        std::memcpy(tooltip, u16str.c_str(), copyLen * sizeof(char16_type));
        tooltip[copyLen] = 0;
        *length = copyLen;
    }
    else if (length)
    {
        // Just return the required length
        *length = qtTooltip.length() + 1;
    }
    
    return api_true;
}

// =============================================================
// END OF MISCELLANEOUS STUBS
// =============================================================

struct MockTreeBox ;

struct MockControl {
    QWidget* widget;
    api_handle clientHandle;
    QLayout* layout;
    
    // Event handlers - add these to your existing structure
    pcl::paint_event_routine paintHandler;
    control_handle paintReceiver;
    
    pcl::resize_event_routine resizeHandler;
    control_handle resizeReceiver;
    
    pcl::move_event_routine moveHandler;
    control_handle moveReceiver;
    
    pcl::control_event_routine enterHandler;
    control_handle enterReceiver;
    
    pcl::control_event_routine leaveHandler;
    control_handle leaveReceiver;
    
    pcl::mouse_event_routine mouseMoveHandler;
    control_handle mouseMoveReceiver;
    
    pcl::mouse_button_event_routine mousePressHandler;
    control_handle mousePressReceiver;
    
    pcl::mouse_button_event_routine mouseReleaseHandler;
    control_handle mouseReleaseReceiver;
    
    pcl::keyboard_event_routine keyPressHandler;
    control_handle keyPressReceiver;
    
    pcl::keyboard_event_routine keyReleaseHandler;
    control_handle keyReleaseReceiver;
    
    pcl::wheel_event_routine wheelHandler;
    control_handle wheelReceiver;
    
    pcl::control_event_routine destroyHandler;
    control_handle destroyReceiver;
    
    pcl::control_event_routine showHandler;
    control_handle showReceiver;
    
    pcl::control_event_routine hideHandler;
    control_handle hideReceiver;
    
    pcl::control_event_routine closeHandler;
    control_handle closeReceiver;
    
    pcl::control_event_routine getFocusHandler;
    control_handle getFocusReceiver;
    
    pcl::control_event_routine loseFocusHandler;
    control_handle loseFocusReceiver;

    pcl::event_routine editCompletedHandler;
    control_handle editCompletedReceiver;

    MockTreeBox *parent;
    int flags;
  
    MockControl(QWidget* w = nullptr) 
        : widget(w ? w : new QWidget()),
          clientHandle(nullptr),
          layout(nullptr),
          paintHandler(nullptr), paintReceiver(nullptr),
          resizeHandler(nullptr), resizeReceiver(nullptr),
          moveHandler(nullptr), moveReceiver(nullptr),
          enterHandler(nullptr), enterReceiver(nullptr),
          leaveHandler(nullptr), leaveReceiver(nullptr),
          mouseMoveHandler(nullptr), mouseMoveReceiver(nullptr),
          mousePressHandler(nullptr), mousePressReceiver(nullptr),
          mouseReleaseHandler(nullptr), mouseReleaseReceiver(nullptr),
          keyPressHandler(nullptr), keyPressReceiver(nullptr),
          keyReleaseHandler(nullptr), keyReleaseReceiver(nullptr),
          wheelHandler(nullptr), wheelReceiver(nullptr),
          destroyHandler(nullptr), destroyReceiver(nullptr),
          showHandler(nullptr), showReceiver(nullptr),
          hideHandler(nullptr), hideReceiver(nullptr),
          closeHandler(nullptr), closeReceiver(nullptr),
          getFocusHandler(nullptr), getFocusReceiver(nullptr),
          loseFocusHandler(nullptr), loseFocusReceiver(nullptr),
          editCompletedHandler(nullptr), editCompletedReceiver(nullptr)
    {
    }
    
    ~MockControl() {
        // Qt handles widget cleanup
    }
};

struct MockEdit {
    QLineEdit* edit;

    // PCL objects:
    void* pclEdit;               // pcl::Edit* (used as hSender)
    void* editCompletedReceiver; // pcl::Control* (NumericControl etc.)

    pcl::event_routine editCompletedHandler;
    pcl::range_event_routine caretPositionUpdatedHandler;
    control_handle caretPositionUpdatedReceiver;
    pcl::range_event_routine selectionUpdatedHandler;
    control_handle selectionUpdatedReceiver;

};

struct MockImageStruct {
    uint32_t width;
    uint32_t height;
    uint32_t channels;
    uint32_t bitsPerSample;
    bool isFloat;
    uint32_t colorSpace;
    void** pixelData;    // Array of channel pointers
    double* stats;       // Min/max values per channel
};

struct MockFileInstance {
    std::string path;
    std::string extension;
    uint32_t selectedImage; // index of selected image
    std::vector<MockImageStruct*> images; // images in the file
};

struct MockGroupBox {
    QGroupBox* box;
    api_handle clientHandle;

    MockGroupBox() :
        box(new QGroupBox()),
        clientHandle(nullptr)
    {
    }

    ~MockGroupBox() {
        // QGroupBox is cleaned up by Qt parent ownership
    }
};

struct MockTreeNode
{
    std::vector<String>          text;
    std::vector<control_handle>  icon;
    std::vector<String>          tooltip;

    bool selected = false;

    explicit MockTreeNode( int columns )
    {
        text.resize( columns );
        icon.resize( columns, nullptr );
        tooltip.resize( columns );
    }
};

struct MockTreeBox : MockControl
{
    int columns = 1;
    std::vector<MockTreeNode*> nodes;

    control_handle viewport = nullptr;
    QTreeWidget* tree = nullptr;

    // Behavior flags we care about
    bool multipleSelection = false;
    bool uniformRowHeight = false;

    bool multipleSelections = false;
    bool rootDecoration     = false;
    bool alternateRowColor  = false;

    MockTreeBox() = default;

    ~MockTreeBox()
    {
        for ( MockTreeNode* n : nodes )
            delete n;
    }
};

struct MockLabel {
    QLabel* label;
    api_handle clientHandle;
    
    MockLabel(const char16_type* text = nullptr) 
        : label(new QLabel()),
          clientHandle(nullptr)
    {
        if (text && *text) {
            label->setText(QString::fromUtf16(reinterpret_cast<const ushort*>(text)));
        }
    }
    
    ~MockLabel() {
        // Qt parent ownership handles deletion
    }
};

static std::map<control_handle, MockLabel*> g_label_map;
static std::map<control_handle, MockTreeBox*> g_treebox_map;
static std::map<control_handle, MockGroupBox*> g_groupbox_map;
static std::map<const_control_handle, MockControl*> g_control_map;
static std::map<control_handle, MockEdit*> g_edit_map;
static std::map<file_format_handle, MockFileInstance*> g_file_instances;
static std::map<image_handle, MockImageStruct*> g_image_map;
static void* g_module_handle = nullptr;

std::string GetFileExtension(const std::string& path) {
    size_t pos = path.find_last_of('.');
    if (pos != std::string::npos) {
        std::string ext = path.substr(pos);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext;
    }
    return "";
}

MockControl* GetControlBox(const_control_handle h)
{
    if (!h)
        return nullptr;

    auto it = g_control_map.find(h);
    if (it == g_control_map.end())
        return nullptr;

    return it->second;
}

// ============================================================================
// ControlContext (3 functions)
// ============================================================================

class ControlEventFilter : public QObject {
    Q_OBJECT
    
public:
    control_handle controlHandle;
    MockControl* mockControl;
    
    ControlEventFilter(control_handle handle, MockControl* ctrl, QObject* parent = nullptr)
        : QObject(parent), controlHandle(handle), mockControl(ctrl)
    {
    }
    
protected:
    bool eventFilter(QObject* obj, QEvent* event) override {
        if (!mockControl) {
            // No mock control, just pass through
            return QObject::eventFilter(obj, event);
        }

        switch (event->type()) {
            case QEvent::Paint:
                if (mockControl->paintHandler && mockControl->paintReceiver) {
                    QPaintEvent* pe = static_cast<QPaintEvent*>(event);
                    mockControl->paintHandler(mockControl->paintReceiver, controlHandle,
                        pe->rect().x(), pe->rect().y(), 
                        pe->rect().width(), pe->rect().height());
                    return false; // Let Qt handle default painting too
                }
                break;
                
            case QEvent::Resize:
                if (mockControl->resizeHandler && mockControl->resizeReceiver) {
                    QResizeEvent* re = static_cast<QResizeEvent*>(event);
                    mockControl->resizeHandler(mockControl->resizeReceiver, controlHandle,
                        re->size().width(), re->size().height(),
                        re->oldSize().width(), re->oldSize().height());
                }
                break;
                
            case QEvent::Enter:
                if (mockControl->enterHandler && mockControl->enterReceiver) {
                    mockControl->enterHandler(mockControl->enterReceiver, controlHandle);
                }
                break;
                
            case QEvent::Leave:
                if (mockControl->leaveHandler && mockControl->leaveReceiver) {
                    mockControl->leaveHandler(mockControl->leaveReceiver, controlHandle);
                }
                break;
                
            case QEvent::MouseMove:
                if (mockControl->mouseMoveHandler && mockControl->mouseMoveReceiver) {
                    QMouseEvent* me = static_cast<QMouseEvent*>(event);
                    mockControl->mouseMoveHandler(mockControl->mouseMoveReceiver, controlHandle,
                        me->x(), me->y(),
                        static_cast<uint32>(me->buttons()),
                        static_cast<uint32>(me->modifiers()));
                }
                break;
                
            case QEvent::MouseButtonPress:
                if (mockControl->mousePressHandler && mockControl->mousePressReceiver) {
                    QMouseEvent* me = static_cast<QMouseEvent*>(event);
                    mockControl->mousePressHandler(mockControl->mousePressReceiver, controlHandle,
                        me->x(), me->y(),
                        static_cast<uint32>(me->button()),
                        static_cast<uint32>(me->buttons()),
                        static_cast<uint32>(me->modifiers()));
                }
                break;
                
            case QEvent::MouseButtonRelease:
                if (mockControl->mouseReleaseHandler && mockControl->mouseReleaseReceiver) {
                    QMouseEvent* me = static_cast<QMouseEvent*>(event);
                    mockControl->mouseReleaseHandler(mockControl->mouseReleaseReceiver, controlHandle,
                        me->x(), me->y(),
                        static_cast<uint32>(me->button()),
                        static_cast<uint32>(me->buttons()),
                        static_cast<uint32>(me->modifiers()));
                }
                break;
                
            case QEvent::KeyPress:
                if (mockControl->keyPressHandler && mockControl->keyPressReceiver) {
                    QKeyEvent* ke = static_cast<QKeyEvent*>(event);
                    mockControl->keyPressHandler(mockControl->keyPressReceiver, controlHandle,
                        ke->key(),
                        static_cast<uint32>(ke->modifiers()));
                }
                break;
                
            case QEvent::KeyRelease:
                if (mockControl->keyReleaseHandler && mockControl->keyReleaseReceiver) {
                    QKeyEvent* ke = static_cast<QKeyEvent*>(event);
                    mockControl->keyReleaseHandler(mockControl->keyReleaseReceiver, controlHandle,
                        ke->key(),
                        static_cast<uint32>(ke->modifiers()));
                }
                break;
                
            case QEvent::Wheel:
                if (mockControl->wheelHandler && mockControl->wheelReceiver) {
                    QWheelEvent* we = static_cast<QWheelEvent*>(event);
                    mockControl->wheelHandler(mockControl->wheelReceiver, controlHandle,
                        we->position().x(), we->position().y(),
                        we->angleDelta().y(),
                        static_cast<uint32>(we->buttons()),
                        static_cast<uint32>(we->modifiers()));
                }
                break;
                
            default:
                break;
        }
        
        return QObject::eventFilter(obj, event);
    }
};

static void EnableEvents(control_handle handle, MockControl* ctrl) {
    if (!ctrl) {
        LogDebug("EnableEvents: null ctrl");
        return;
    }
    
    if (!ctrl->widget) {
        LogDebug("EnableEvents: null widget - this shouldn't happen");
        return;
    }
    
    // Check if we already have an event filter installed
    QObjectList children = ctrl->widget->children();
    for (QObject* child : children) {
        if (ControlEventFilter* existingFilter = dynamic_cast<ControlEventFilter*>(child)) {
            LogDebug("EnableEvents: event filter already exists, updating");
            existingFilter->mockControl = ctrl;
            existingFilter->controlHandle = handle;
            return;
        }
    }
    
    LogDebug("EnableEvents: installing new event filter on widget");
    
    // Create and install an event filter
    // Set the widget as parent so it gets cleaned up automatically
    ControlEventFilter* filter = new ControlEventFilter(handle, ctrl, ctrl->widget);
    ctrl->widget->installEventFilter(filter);
    
    LogDebug("EnableEvents: event filter installed successfully");
}

// ControlContext::GetControlCursor
// From: ControlContext::GetControlCursor (old.cpp lines 3167-3180)
cursor_handle ControlContext::GetControlCursor(const_control_handle handle)
{
    if (!handle) return api_false;
    
    MockControl* wdg = GetControlBox(handle);
    if (!wdg) return api_false;
    QWidget* widget = wdg->widget;
    
    // Map Qt cursor to PCL cursor shape
    Qt::CursorShape shape = widget->cursor().shape();
    return reinterpret_cast<cursor_handle>(shape);
}

// ControlContext::SetDestroyEventRoutine
// From: ControlContext::SetDestroyEventRoutine (old.cpp lines 740-762)
api_bool ControlContext::SetDestroyEventRoutine(control_handle handle, control_handle receiver,
                                            pcl::control_event_routine handler)
{
    LogDbg("SetDestroyEventRoutine called");
    
    auto it = g_control_map.find(handle);
    if (it == g_control_map.end()) return api_false;
    
    MockControl* ctrl = it->second;
    ctrl->destroyHandler = handler;
    ctrl->destroyReceiver = receiver;
    
    if (handler) {
        QObject::connect(ctrl->widget, &QObject::destroyed, [ctrl, handle]() {
            if (ctrl->destroyHandler && ctrl->destroyReceiver) {
                ctrl->destroyHandler(ctrl->destroyReceiver, handle);
            }
        });
    }
    
    return api_true;
}

// ControlContext::SetMoveEventRoutine
// From: ControlContext::SetMoveEventRoutine (old.cpp lines 900-915)
api_bool ControlContext::SetMoveEventRoutine(control_handle handle, control_handle receiver,
                                         pcl::move_event_routine handler)
{
    LogDbg("SetMoveEventRoutine called");
    
    auto it = g_control_map.find(handle);
    if (it == g_control_map.end()) return api_false;
    
    MockControl* ctrl = it->second;
    ctrl->moveHandler = handler;
    ctrl->moveReceiver = receiver;
    
    EnableEvents(handle, ctrl);
    return api_true;
}


// ============================================================================
// EditContext (4 functions)
// ============================================================================

// EditContext::GetEditSelectedText
// From: EditContext::GetEditSelectedText (old.cpp lines 11358-11390)
api_bool EditContext::GetEditSelectedText(const_control_handle handle,
                                      char16_type* text,
                                      size_type* len)
{
    LogDbg("API_Edit_GetEditSelectedText called");

    auto it = g_edit_map.find(const_cast<control_handle>(handle));
    if (it == g_edit_map.end()) {
        if (len) *len = 0;
        return api_false;
    }

    MockEdit* edit = it->second;
    QString qsel = edit->edit->selectedText();
    std::u16string u16 = qsel.toStdU16String();

    if (!text) {
        if (len) *len = u16.length();
        return api_true;
    }

    if (!len || *len == 0) {
        return api_false;
    }

    size_type copyLen = std::min(*len - 1, (size_type)u16.length());
    std::memcpy(text, u16.c_str(), copyLen * sizeof(char16_type));
    text[copyLen] = 0;
    *len = copyLen;

    return api_true;
}

// EditContext::GetEditValid
// From: EditContext::GetEditValid (old.cpp lines 11200-11216)
api_bool EditContext::GetEditValid(const_control_handle handle)
{
    LogDbg("API_Edit_GetEditValid called");

    auto it = g_edit_map.find(const_cast<control_handle>(handle));
    if (it == g_edit_map.end())
        return api_false;

    MockEdit* edit = it->second;
    QLineEdit* w = edit->edit;

    if (!w->validator())
        return api_true; // No validator → always valid

    return w->hasAcceptableInput() ? api_true : api_false;
}

// EditContext::SetCaretPositionUpdatedEventRoutine
// From: EditContext::SetCaretPositionUpdatedEventRoutine (old.cpp lines 11505-11535)
api_bool EditContext::SetCaretPositionUpdatedEventRoutine(
        control_handle handle,
        control_handle receiver,
        pcl::range_event_routine routine )
{
    LogDbg("API_Edit_SetCaretPositionUpdatedEventRoutine called");

    auto it = g_edit_map.find(handle);
    if (it == g_edit_map.end()) return api_false;

    MockEdit* edit = it->second;
    edit->caretPositionUpdatedHandler  = routine;
    edit->caretPositionUpdatedReceiver = receiver;

    QLineEdit* w = edit->edit;

    if (routine)
        QObject::connect(w, &QLineEdit::cursorPositionChanged,
            [edit, w](int oldPos, int newPos) {
                if (edit->caretPositionUpdatedHandler && edit->caretPositionUpdatedReceiver) {
                    control_handle h = reinterpret_cast<control_handle>(w);
                    edit->caretPositionUpdatedHandler(edit->caretPositionUpdatedReceiver,
                                                      h,
                                                      oldPos,
                                                      newPos);
                }
            });

    return api_true;
}

// EditContext::SetSelectionUpdatedEventRoutine
// From: EditContext::SetSelectionUpdatedEventRoutine (old.cpp lines 11537-11569)
api_bool EditContext::SetSelectionUpdatedEventRoutine(
        control_handle handle,
        control_handle receiver,
        pcl::range_event_routine routine )
{
    LogDbg("API_Edit_SetSelectionUpdatedEventRoutine called");

    auto it = g_edit_map.find(handle);
    if (it == g_edit_map.end()) return api_false;

    MockEdit* edit = it->second;
    edit->selectionUpdatedHandler  = routine;
    edit->selectionUpdatedReceiver = receiver;

    QLineEdit* w = edit->edit;

    if (routine)
        QObject::connect(w, &QLineEdit::selectionChanged,
            [edit, w]() {
                if (edit->selectionUpdatedHandler && edit->selectionUpdatedReceiver) {
                    control_handle h = reinterpret_cast<control_handle>(w);
                    int start = w->selectionStart();
                    int len   = w->selectedText().length();
                    edit->selectionUpdatedHandler(edit->selectionUpdatedReceiver,
                                                  h,
                                                  start,
                                                  start + len);
                }
            });

    return api_true;
}


// ============================================================================
// FileFormatContext (3 functions)
// ============================================================================

std::string Utf16ToUtf8(const char16_type* utf16Str) {
    if (!utf16Str) return "";
    
    // Simple direct conversion
    std::string result;
    const char16_t* src = reinterpret_cast<const char16_t*>(utf16Str);
    while (*src) {
        char16_t ch = *src++;
        if (ch < 128) {
            result += static_cast<char>(ch);
        } else if (ch < 0x800) {
            result += static_cast<char>(0xC0 | (ch >> 6));
            result += static_cast<char>(0x80 | (ch & 0x3F));
        } else {
            result += static_cast<char>(0xE0 | (ch >> 12));
            result += static_cast<char>(0x80 | ((ch >> 6) & 0x3F));
            result += static_cast<char>(0x80 | (ch & 0x3F));
        }
    }
    return result;
}

// FileFormatContext::CreateImage
// From: FileFormatContext::CreateImage (old.cpp lines 6598-6796)
api_bool FileFormatContext::CreateImage(file_format_handle handle, const api_image_info* info)
{
    if (!handle || !info) {
        return api_false;
    }
    
    auto it = g_file_instances.find(handle);
    if (it == g_file_instances.end()) {
        return api_false;
    }
    
    // Create a new image
    MockImageStruct* img = new MockImageStruct();
    img->width = info->width;
    img->height = info->height;
    img->channels = info->numberOfChannels;
    img->bitsPerSample = 32; // Default to 32-bit float
    img->isFloat = true;
    img->colorSpace = info->colorSpace;
    
    // Allocate pixel data
    img->pixelData = new void*[img->channels];
    for (uint32_t i = 0; i < img->channels; i++) {
        img->pixelData[i] = calloc(img->width * img->height, sizeof(float));
    }
    
    // Create stats
    img->stats = new double[img->channels * 2];
    for (uint32_t i = 0; i < img->channels; i++) {
        img->stats[i*2] = 0.0;     // min
        img->stats[i*2+1] = 1.0;   // max
    }
    
    // Add to the file's image list
    it->second->images.push_back(img);
    it->second->selectedImage = it->second->images.size() - 1;
    
    return api_true;
}

// Write image data to the file
api_bool WriteImagePixelData(file_format_handle handle, const_image_handle image) {
    if (!handle || !image) {
        return api_false;
    }
    
    auto it = g_file_instances.find(handle);
    if (it == g_file_instances.end() || 
        it->second->selectedImage >= it->second->images.size()) {
        return api_false;
    }
    
    // Get the target image in the file
    MockImageStruct* dstImg = it->second->images[it->second->selectedImage];
    
    // Get the source image
    auto img_it = g_image_map.find((image_handle)image);
    if (img_it == g_image_map.end()) {
        return api_false;
    }
    
    MockImageStruct* srcImg = img_it->second;
    
    // Check compatibility
    if (dstImg->width != srcImg->width || 
        dstImg->height != srcImg->height || 
        dstImg->channels != srcImg->channels) {
        return api_false;
    }
    
    // Copy pixel data
    for (uint32_t i = 0; i < srcImg->channels; i++) {
        size_t pixelCount = srcImg->width * srcImg->height;
        size_t bytesPerSample = (srcImg->bitsPerSample <= 8) ? 1 : 
                               ((srcImg->bitsPerSample <= 16) ? 2 : 
                               ((srcImg->bitsPerSample <= 32) ? 4 : 8));
        
        memcpy(dstImg->pixelData[i], srcImg->pixelData[i], pixelCount * bytesPerSample);
    }
    
    // Copy stats
    for (uint32_t i = 0; i < srcImg->channels; i++) {
        dstImg->stats[i*2] = srcImg->stats[i*2];        // min
        dstImg->stats[i*2+1] = srcImg->stats[i*2+1];    // max
    }
    
    return api_true;
}

api_bool FileFormatContext::WriteImage(file_format_handle handle, const_image_handle image) {
    if (!handle || !image) {
        return api_false;
    }
    
    auto it = g_file_instances.find(handle);
    if (it == g_file_instances.end()) {
        return api_false;
    }
    
    MockFileInstance* instance = it->second;
    
    std::string extension = GetFileExtension(instance->path);
    bool isXISF = (extension == ".xisf");
    
    if (isXISF) {
	LogDbg("WriteImage: Using XISF writer for: " + instance->path);

	// Get the MockImageStruct
	auto img_it = g_image_map.find((image_handle)image);
	if (img_it == g_image_map.end()) {
	    LogDbg("WriteImage: Image not found in map");
	    return api_false;
	}

	MockImageStruct* mockImg = img_it->second;

	LogDbg("WriteImage: MockImageStruct is " + 
		 std::to_string(mockImg->width) + "x" + 
		 std::to_string(mockImg->height) + ", " +
		 std::to_string(mockImg->channels) + " channels");

	if (!mockImg->pixelData) {
	    LogDbg("WriteImage: pixelData is NULL!");
	    return api_false;
	}

	try {
	    // Determine color space
	    pcl::ColorSpace::value_type colorSpace;
	    if (mockImg->channels == 1) {
		colorSpace = pcl::ColorSpace::Gray;
	    } else if (mockImg->channels == 3) {
		colorSpace = pcl::ColorSpace::RGB;
	    } else {
		colorSpace = pcl::ColorSpace::RGB; // Default for other cases
	    }

	    // Create a PCL image with correct constructor
	    pcl::FImage outputImage(mockImg->width, mockImg->height, colorSpace);

	    // If we need more channels than the color space provides, allocate them
	    if (mockImg->channels > outputImage.NumberOfChannels()) {
		outputImage.AllocateData(mockImg->width, mockImg->height, mockImg->channels, colorSpace);
	    }

	    for (uint32_t c = 0; c < mockImg->channels; c++) {
		if (!mockImg->pixelData[c]) {
		    LogDbg("WriteImage: Channel " + std::to_string(c) + " is NULL!");
		    return api_false;
		}

		memcpy(outputImage.PixelData(c), 
		       mockImg->pixelData[c], 
		       mockImg->width * mockImg->height * sizeof(float));
	    }

	    pcl::XISFWriter xisfWriter;
	    pcl::XISFOptions xisfOptions;
	    xisfOptions.verbosity = 2;
	    xisfWriter.SetOptions(xisfOptions);

	    xisfWriter.Create(pcl::String(instance->path.c_str()), 1);

	    pcl::ImageOptions imgOptions;
	    imgOptions.bitsPerSample = 32;
	    imgOptions.ieeefpSampleFormat = true;
	    xisfWriter.SetImageOptions(imgOptions);

	    xisfWriter.WriteImage(outputImage);

	    xisfWriter.Close();
	    LogDbg("WriteImage: Successfully wrote XISF file");
	    return api_true;

	} catch (const std::exception& e) {
	    LogDbg("WriteImage: Exception: " + std::string(e.what()));
	    return api_false;
	}
    }    
    return api_false;
}  
  
// Set the RGB working space for an image in a file
api_bool SetImageRGBWS(file_format_handle handle, const api_RGBWS* rgbws) {
    if (!handle || !rgbws) {
        LogDbg("SetImageRGBWS: Invalid handle or RGBWS data");
        return api_false;
    }
    
    LogDbg("SetImageRGBWS: Setting RGB working space");
    
    // In a real implementation, you would store the RGBWS data
    // For the mock, we'll just return success
    return api_true;
}

// FileFormatContext::CreateImageFile
// From: FileFormatContext::CreateImageFile (old.cpp lines 6534-6553)
api_bool FileFormatContext::CreateImageFile(file_format_handle handle, const char16_type* filePath, 
                        uint32 hints)
{
    if (!handle || !filePath) {
        return api_false;
    }
    
    std::string path = Utf16ToUtf8(filePath);
    LogDbg("CreateImageFile called for: " + path);
    
    auto it = g_file_instances.find(handle);
    if (it == g_file_instances.end()) {
        return api_false;
    }
    
    // Store file path
    it->second->path = path;
    
    return api_true;
}

// Create a new file with extended options
api_bool FileFormatContext::CreateImageFileEx(file_format_handle handle, const char16_type* filePath, 
                          uint32 count, const char* hints, uint32 flags) {
    if (!handle || !filePath) {
        return api_false;
    }
    
    std::string path = Utf16ToUtf8(filePath);
    LogDbg("CreateImageFileEx called for: " + path + " with count: " + std::to_string(count) +
             " and flags: " + std::to_string(flags));
    
    auto it = g_file_instances.find(handle);
    if (it == g_file_instances.end()) {
        return api_false;
    }
    
    // Store file path
    it->second->path = path;
    
    // Clear any existing images
    for (MockImageStruct* img : it->second->images) {
        // Free pixel data for each channel
        for (uint32_t i = 0; i < img->channels; i++) {
            free(img->pixelData[i]);
        }
        
        delete[] img->pixelData;
        delete[] img->stats;
        delete img;
    }
    
    it->second->images.clear();
    it->second->selectedImage = 0;
    
    // Pre-allocate space for the requested number of images
    // (we won't create them yet - they'll be created when FileCreateImage is called)
    LogDbg("CreateImageFileEx: Prepared for " + std::to_string(count) + " images");
    
    return api_true;
}

// ============================================================================
// GroupBoxContext (2 functions)
// ============================================================================

// GroupBoxContext::GetGroupBoxChecked
// From: GroupBoxContext::GetGroupBoxChecked (old.cpp lines 10900-10911)
api_bool GroupBoxContext::GetGroupBoxChecked(const_control_handle handle)
{
    LogDbg("API_GroupBox_GetGroupBoxChecked called");

    auto it = g_groupbox_map.find(const_cast<control_handle>(handle));

    if (it == g_groupbox_map.end())
        return api_false;

    return it->second->box->isChecked() ? api_true : api_false;
}

// GroupBoxContext::GetGroupBoxTitle
// From: GroupBoxContext::GetGroupBoxTitle (old.cpp lines 10836-10868)
api_bool GroupBoxContext::GetGroupBoxTitle(
        const_control_handle handle,
        char16_type* text,
        size_type* len)
{
    LogDbg("API_GroupBox_GetGroupBoxTitle called");

    auto it = g_groupbox_map.find(const_cast<control_handle>(handle));

    if (it == g_groupbox_map.end()) {
        if (len) *len = 0;
        return api_false;
    }

    QString title = it->second->box->title();
    std::u16string u16 = title.toStdU16String();

    if (!text) {
        if (len) *len = u16.length();
        return api_true;
    }

    if (!len || *len == 0)
        return api_false;

    size_type copyLen = std::min(*len - 1, (size_type)u16.length());
    memcpy(text, u16.c_str(), copyLen * sizeof(char16_type));
    text[copyLen] = 0;
    *len = copyLen;

    return api_true;
}


// ============================================================================
// LabelContext (1 functions)
// ============================================================================

// LabelContext::GetLabelWordWrapping
// From: LabelContext::GetLabelWordWrapping (old.cpp lines 3354-3365)
api_bool LabelContext::GetLabelWordWrappingEnabled(const_control_handle handle)
{
    LogDebug("GetLabelWordWrapping called");
    
    auto it = g_label_map.find(const_cast<control_handle>(handle));
    if (it == g_label_map.end()) {
        return api_false;
    }
    
    return it->second->label->wordWrap() ? api_true : api_false;
}


// ============================================================================
// NumericalContext (2 functions)
// ============================================================================

// NumericalContext::FFTInverseComplexTransformD
// From: NumericalContext::FFTInverseComplexTransformD (old.cpp lines 1543-1573)
api_bool NumericalContext::FFTComplexInverseTransformD(fft_handle handle, void* y0, const void* x0)
{
    if (!handle || !y0 || !x0) return api_false;

    dcomplex *y = (dcomplex *)y0;
    const dcomplex* x = (const dcomplex *) x0;
    LogDebug("FFTInverseComplexTransformD called with handle " + std::to_string((uintptr_t)handle));
    
    auto it = g_fftw_plans.find(handle);
    if (it == g_fftw_plans.end() || it->second->isReal) {
        LogDebug("FFTInverseComplexTransformD: Invalid handle or not complex transform");
        return api_false;
    }
    
    FFTWPlanWrapper* wrapper = it->second;
    int n = wrapper->size;
    
    // Copy input data to FFTW buffer
    for (int i = 0; i < n; i++) {
        wrapper->complexIn[i][0] = x[i].Real();
        wrapper->complexIn[i][1] = x[i].Imag();
    }
    
    // Execute inverse plan
    fftw_execute(wrapper->inversePlan);
    
    // Copy results to output array and normalize
    for (int i = 0; i < n; i++) {
        y[i] = dcomplex(wrapper->complexOut[i][0] / n, wrapper->complexOut[i][1] / n);
    }
    
    return api_true;
}

// NumericalContext::FFTInverseRealTransformD
// From: NumericalContext::FFTInverseRealTransformD (old.cpp lines 1473-3563)
api_bool NumericalContext::FFTRealInverseTransformD(fft_handle handle, double* x, const void* y0)
{
    const dcomplex *y = (const dcomplex *)y0;
    if (!handle || !y || !x) return api_false;

    LogDebug("FFTInverseRealTransformD called with handle " + std::to_string((uintptr_t)handle));
    
    auto it = g_fftw_plans.find(handle);
    if (it == g_fftw_plans.end() || !it->second->isReal) {
        LogDebug("FFTInverseRealTransformD: Invalid handle or not real transform");
        return api_false;
    }
    
    FFTWPlanWrapper* wrapper = it->second;
    int n = wrapper->size;
    
    // Copy input data to FFTW buffer
    for (int i = 0; i < n/2 + 1; i++) {
        wrapper->complexIn[i][0] = y[i].Real();
        wrapper->complexIn[i][1] = y[i].Imag();
    }
    
    // Execute inverse plan
    fftw_execute(wrapper->inversePlan);
    
    // Copy results to output array
    // FFTW unnormalized results need to be divided by n
    memcpy(x, wrapper->realOut, sizeof(double) * n);
    
    // Normalize (FFTW doesn't normalize automatically)
    for (int i = 0; i < n; i++) {
        x[i] /= n;
    }
    
    return api_true;
}

// Forward complex transform (complex to complex)
api_bool NumericalContext::FFTComplexTransformD(fft_handle handle, void* y0, const void *x0) {
    const dcomplex *x = (const dcomplex *)x0;
    dcomplex *y = (dcomplex *)y0;
    if (!handle || !y || !x) return api_false;
    LogDebug("FFTComplexTransformD called with handle " + std::to_string((uintptr_t)handle));
    
    auto it = g_fftw_plans.find(handle);
    if (it == g_fftw_plans.end() || it->second->isReal) {
        LogDebug("FFTComplexTransformD: Invalid handle or not complex transform");
        return api_false;
    }
    
    FFTWPlanWrapper* wrapper = it->second;
    int n = wrapper->size;
    
    // Copy input data to FFTW buffer
    for (int i = 0; i < n; i++) {
        wrapper->complexIn[i][0] = x[i].Real();
        wrapper->complexIn[i][1] = x[i].Imag();
    }
    
    // Execute forward plan
    fftw_execute(wrapper->forwardPlan);
    
    // Copy results to output array
    for (int i = 0; i < n; i++) {
        y[i] = dcomplex(wrapper->complexOut[i][0], wrapper->complexOut[i][1]);
    }
    
    return api_true;
}

/*
// Inverse complex transform (complex to complex)
api_bool NumericalContext::FFTComplexInverseTransformD(fft_handle handle, void *y0, const void *x0) {
    if (!handle || !y || !x) return api_false;

    dcomplex *y = (dcomplex *)y0;
    dcomplex *x = (dcomplex *)x0;
    LogDebug("FFTInverseComplexTransformD called with handle " + std::to_string((uintptr_t)handle));
    
    auto it = g_fftw_plans.find(handle);
    if (it == g_fftw_plans.end() || it->second->isReal) {
        LogDebug("FFTInverseComplexTransformD: Invalid handle or not complex transform");
        return api_false;
    }
    
    FFTWPlanWrapper* wrapper = it->second;
    int n = wrapper->size;
    
    // Copy input data to FFTW buffer
    for (int i = 0; i < n; i++) {
        wrapper->complexIn[i][0] = x[i].Real();
        wrapper->complexIn[i][1] = x[i].Imag();
    }
    
    // Execute inverse plan
    fftw_execute(wrapper->inversePlan);
    
    // Copy results to output array and normalize
    for (int i = 0; i < n; i++) {
        y[i] = dcomplex(wrapper->complexOut[i][0] / n, wrapper->complexOut[i][1] / n);
    }
    
    return api_true;
}

// Another naming convention for inverse complex transform (alias for FFTInverseComplexTransformD)
api_bool NumericalContext::FFTComplexInverseTransformD(void* handle, dcomplex* y, const dcomplex* x) {
    LogDebug("FFTComplexInverseTransformD called (alias for FFTInverseComplexTransformD)");
    // Simply delegate to the existing implementation
    return NumericalContext::FFTInverseComplexTransformD(handle, y, x);
}  
*/

// Don't forget to call this at program exit
void ShutdownFFTFunctions() {
    CleanupFFTW();
}

// Call this in your InitializeMockAPI function
void InitializeMockAPI() {
    
    LogDebug("Mock API initialized");
}  

// Set the module handle
void SetModuleHandle(void* handle) {
    g_module_handle = handle;
    LogDebug("Module handle set to: " + std::to_string((uintptr_t)handle));
}

// Get the module handle
void* GetModuleHandle() {
    return g_module_handle;
}

// Set the log file
void SetLogFile(const std::string& filename) {
}

// ----------------------------------------------------------------------------
// Sizer Mock Implementation
// ----------------------------------------------------------------------------
/*
// Helper to get or create MockControl for any widget
static MockControl* GetOrCreateMockControl(control_handle handle) {
    auto it = g_control_map.find(handle);
    if (it != g_control_map.end()) {
        return it->second;
    }
    
    // Create new mock control for existing widget
    QWidget* widget = reinterpret_cast<QWidget*>(handle);
    MockControl* ctrl = new MockControl(widget);
    g_control_map[handle] = ctrl;
    return ctrl;
}
*/
struct MockSizer {
    QBoxLayout* layout;
    bool vertical;
    std::vector<QWidget*> widgets;
    
    MockSizer(bool vert) : vertical(vert) {
        if (vert) {
            layout = new QVBoxLayout();
        } else {
            layout = new QHBoxLayout();
        }
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
    }
    
    ~MockSizer() {
        // Don't delete layout - Qt parent ownership handles it
        // Don't delete widgets - they're owned by their parents
    }
};

// Global map to track sizers
static std::map<sizer_handle, MockSizer*> g_sizer_map;

// ----------------------------------------------------------------------------
// SizerContext API
// ----------------------------------------------------------------------------

control_handle SizerContext::GetSizerParentControl(const_sizer_handle handle)
{
    LogDebug("GetSizerParentControl called");
    
    auto it = g_sizer_map.find(const_cast<sizer_handle>(handle));
    if (it == g_sizer_map.end()) {
        return nullptr;
    }
    
    // Return the widget that owns this layout
    QWidget* parent = it->second->layout->parentWidget();
    return reinterpret_cast<control_handle>(parent);
}

api_bool SizerContext::GetSizerOrientation(const_sizer_handle handle)
{
    LogDebug("GetSizerOrientation called");
    
    auto it = g_sizer_map.find(const_cast<sizer_handle>(handle));
    if (it == g_sizer_map.end()) {
        return api_false;
    }
    
    return it->second->vertical ? api_true : api_false;
}

int32 SizerContext::GetSizerCount(const_sizer_handle handle)
{
    LogDebug("GetSizerCount called");
    
    auto it = g_sizer_map.find(const_cast<sizer_handle>(handle));
    if (it == g_sizer_map.end()) {
        return 0;
    }
    
    return it->second->layout->count();
}

void SizerContext::RemoveSizerControl(sizer_handle handle, control_handle control)
{
    LogDebug("RemoveSizerControl called");
    
    auto it = g_sizer_map.find(handle);
    if (it == g_sizer_map.end()) {
        return;
    }
    
    QWidget* widget = reinterpret_cast<QWidget*>(control);
    it->second->layout->removeWidget(widget);
}

int32 SizerContext::GetSizerMargin(const_sizer_handle handle)
{
    LogDebug("GetSizerMargin called");
    
    auto it = g_sizer_map.find(const_cast<sizer_handle>(handle));
    if (it == g_sizer_map.end()) {
        return 0;
    }
    
    QMargins margins = it->second->layout->contentsMargins();
    return margins.left(); // Return one margin value
}

int32 SizerContext::GetSizerSpacing(const_sizer_handle handle)
{
    LogDebug("GetSizerSpacing called");
    
    auto it = g_sizer_map.find(const_cast<sizer_handle>(handle));
    if (it == g_sizer_map.end()) {
        return 0;
    }
    
    return it->second->layout->spacing();
}

// ----------------------------------------------------------------------------
// Button Mock Implementation
// ----------------------------------------------------------------------------

struct MockButton {
    QWidget* button;  // Could be QPushButton or QToolButton
    bool isToolButton;
    bool checkable;
    bool checked;
    
    // Event handlers
    api_handle clientHandle;
    pcl::button_click_event_routine clickHandler;
    void* clickReceiver;
    
    MockButton(bool toolBtn = false) 
        : isToolButton(toolBtn), checkable(false), checked(false),
          clientHandle(nullptr), clickHandler(nullptr), clickReceiver(nullptr)
    {
        if (toolBtn) {
            button = new QToolButton();
        } else {
            button = new QPushButton();
        }
    }
    
    ~MockButton() {
        // Qt parent ownership handles deletion
    }
};

// Global map to track buttons
static std::map<control_handle, MockButton*> g_button_map;

// ----------------------------------------------------------------------------
// SpinBox Mock Implementation
// ----------------------------------------------------------------------------

struct MockSpinBox {
    QSpinBox* spinBox;
    
    // Event handlers
    api_handle clientHandle;
    pcl::value_event_routine valueHandler;
    control_handle valueReceiver;
    
    // Range and value
    int minValue;
    int maxValue;
    int currentValue;
    
    MockSpinBox() 
        : spinBox(new QSpinBox()),
          clientHandle(nullptr),
          valueHandler(nullptr),
          valueReceiver(nullptr),
          minValue(0),
          maxValue(100),
          currentValue(0)
    {
        spinBox->setRange(minValue, maxValue);
        spinBox->setValue(currentValue);
    }
    
    ~MockSpinBox() {
        // Qt parent ownership handles deletion
    }
};

// Global map to track spinboxes
static std::map<const_control_handle, MockSpinBox*> g_spinbox_map;

// ----------------------------------------------------------------------------
// SpinBoxContext API
// ----------------------------------------------------------------------------

int32 SpinBoxContext::GetSpinBoxStepSize(const_control_handle handle)
{
    LogDebug("GetSpinBoxStepSize called");
    
    auto it = g_spinbox_map.find(const_cast<control_handle>(handle));
    if (it == g_spinbox_map.end()) {
        return 1;
    }
    
    return it->second->spinBox->singleStep();
}

api_bool SpinBoxContext::GetSpinBoxWrappingEnabled(const_control_handle handle)
{
    LogDebug("GetSpinBoxWrapping called");
    
    auto it = g_spinbox_map.find(const_cast<control_handle>(handle));
    if (it == g_spinbox_map.end()) {
        return api_false;
    }
    
    return it->second->spinBox->wrapping() ? api_true : api_false;
}

void SpinBoxContext::SetSpinBoxWrappingEnabled(control_handle handle, api_bool wrapping)
{
    LogDebug("SetSpinBoxWrapping called, wrapping=" + std::to_string(wrapping));
    
    auto it = g_spinbox_map.find(handle);
    if (it == g_spinbox_map.end()) {
        return;
    }
    
    it->second->spinBox->setWrapping(wrapping != 0);
}

api_bool SpinBoxContext::GetSpinBoxPrefix(const_control_handle handle, char16_type* prefix, size_type* len)
{
    LogDebug("GetSpinBoxPrefix called");
    
    auto it = g_spinbox_map.find(const_cast<control_handle>(handle));
    if (it == g_spinbox_map.end()) {
        if (len) *len = 0;
        return api_false;
    }
    
    QString qprefix = it->second->spinBox->prefix();
    std::u16string u16prefix = qprefix.toStdU16String();
    
    if (prefix == nullptr) {
        // Just return the length
        if (len) *len = u16prefix.length();
        return api_true;
    }
    
    if (len && *len > 0) {
        size_type copyLen = std::min(*len - 1, u16prefix.length());
        std::memcpy(prefix, u16prefix.c_str(), copyLen * sizeof(char16_type));
        prefix[copyLen] = 0;
        *len = copyLen;
    }
    
    return api_true;
}

void SpinBoxContext::SetSpinBoxPrefix(control_handle handle, const char16_type* prefix)
{
    LogDebug("SetSpinBoxPrefix called");
    
    auto it = g_spinbox_map.find(handle);
    if (it == g_spinbox_map.end()) {
        return;
    }
    
    if (prefix) {
        QString qprefix = QString::fromUtf16(reinterpret_cast<const ushort*>(prefix));
        it->second->spinBox->setPrefix(qprefix);
    } else {
        it->second->spinBox->setPrefix(QString());
    }
}

api_bool SpinBoxContext::GetSpinBoxSuffix(const_control_handle handle, char16_type* suffix, size_type* len)
{
    LogDebug("GetSpinBoxSuffix called");
    
    auto it = g_spinbox_map.find(const_cast<control_handle>(handle));
    if (it == g_spinbox_map.end()) {
        if (len) *len = 0;
        return api_false;
    }
    
    QString qsuffix = it->second->spinBox->suffix();
    std::u16string u16suffix = qsuffix.toStdU16String();
    
    if (suffix == nullptr) {
        // Just return the length
        if (len) *len = u16suffix.length();
        return api_true;
    }
    
    if (len && *len > 0) {
        size_type copyLen = std::min(*len - 1, u16suffix.length());
        std::memcpy(suffix, u16suffix.c_str(), copyLen * sizeof(char16_type));
        suffix[copyLen] = 0;
        *len = copyLen;
    }
    
    return api_true;
}

void SpinBoxContext::SetSpinBoxSuffix(control_handle handle, const char16_type* suffix)
{
    LogDebug("SetSpinBoxSuffix called");
    
    auto it = g_spinbox_map.find(handle);
    if (it == g_spinbox_map.end()) {
        return;
    }
    
    if (suffix) {
        QString qsuffix = QString::fromUtf16(reinterpret_cast<const ushort*>(suffix));
        it->second->spinBox->setSuffix(qsuffix);
    } else {
        it->second->spinBox->setSuffix(QString());
    }
}

api_bool SpinBoxContext::SetSpinBoxValueUpdatedEventRoutine(
    control_handle handle,
    control_handle receiver,
    pcl::value_event_routine handler)
{
    LogDebug("SetSpinBoxValueUpdatedEventRoutine called");
    /*
    auto it = g_spinbox_map.find(handle);
    if (it == g_spinbox_map.end())
        return api_false;

    MockSpinBox* mockSpin = it->second;
    mockSpin->valueHandler  = handler;
    mockSpin->valueReceiver = receiver;
    
    // Disconnect any existing connections
    QObject::disconnect(mockSpin->spinBox, nullptr, nullptr, nullptr);

    // Connect valueChanged signal
    QObject::connect(
        mockSpin->spinBox,
        QOverload<int>::of(&QSpinBox::valueChanged),
        [mockSpin](int value)
        {
            if (mockSpin->valueHandler && mockSpin->valueReceiver)
            {
                control_handle spinHandle =
                    reinterpret_cast<control_handle>(mockSpin->spinBox);
                mockSpin->valueHandler(mockSpin->valueReceiver, spinHandle, value);
            }
        });
    */
    return api_true;
}

/*
int32 SpinBoxContext::GetSpinBoxMinEditWidth(const_control_handle handle)
{
    LogDebug("GetSpinBoxMinEditWidth called");
    
    auto it = g_spinbox_map.find(const_cast<control_handle>(handle));
    if (it == g_spinbox_map.end()) {
        return 0;
    }
    
    return it->second->spinBox->minimumWidth();
}

void SpinBoxContext::SetSpinBoxMinEditWidth(control_handle handle, int32 width)
{
    LogDebug("SetSpinBoxMinEditWidth called, width=" + std::to_string(width));
    
    auto it = g_spinbox_map.find(handle);
    if (it == g_spinbox_map.end()) {
        return;
    }
    
    it->second->spinBox->setMinimumWidth(width);
}

api_bool SpinBoxContext::IsSpinBoxReadOnly(const_control_handle handle)
{
    LogDebug("IsSpinBoxReadOnly called");
    
    auto it = g_spinbox_map.find(const_cast<control_handle>(handle));
    if (it == g_spinbox_map.end()) {
        return api_false;
    }
    
    return it->second->spinBox->isReadOnly() ? api_true : api_false;
}

void SpinBoxContext::SetSpinBoxReadOnly(control_handle handle, api_bool readOnly)
{
    LogDebug("SetSpinBoxReadOnly called, readOnly=" + std::to_string(readOnly));
    
    auto it = g_spinbox_map.find(handle);
    if (it == g_spinbox_map.end()) {
        return;
    }
    
    it->second->spinBox->setReadOnly(readOnly != 0);
}
*/

// ----------------------------------------------------------------------------
// ControlContext API
// ----------------------------------------------------------------------------

api_bool ControlContext::GetControlResourcePixelRatio(const_control_handle handle, double* ratio)
{
    LogDebug("GetControlResourcePixelRatio called");
    
    if (!handle) {
        *ratio = 1.0;
        return api_true;
    }

    MockControl* wdg = GetControlBox(handle);
    
    if (!ratio || !wdg) {
        return api_true;
    }
    
    QWidget* widget = wdg->widget;
    
    // Get the device pixel ratio from the widget's screen
    QScreen* screen = widget->screen();
    if (screen) {
        *ratio = screen->devicePixelRatio();
    } else {
        // Fallback to primary screen
        QScreen* primaryScreen = QGuiApplication::primaryScreen();
        if (primaryScreen) {
            *ratio = primaryScreen->devicePixelRatio();
        } else {
            *ratio = 1.0;
        }
    }

    *ratio = 1.0;
    return api_true;
}
/*
static inline int SanitizeSize(int v)
{
    return (v < 0) ? 0 : v;   // or 1, but 0 is accepted and means "no min"
}
*/
control_handle ControlContext::GetControlParent(const_control_handle handle)
{
    LogDebug("GetControlParent called");
    
    MockControl* wdg = GetControlBox(handle);
    
    if (!wdg) {
        return api_false;
    }
    
    QWidget* widget = wdg->widget;
    QWidget* parent = widget->parentWidget();
    
    return reinterpret_cast<control_handle>(parent);
}

void ControlContext::SetControlParent(control_handle handle, control_handle parent)
{
    LogDebug("SetControlParent called");
    
    MockControl* wdg = GetControlBox(handle);
    if (!wdg) return;
    QWidget* widget = wdg->widget;
    QWidget* parentWidget = parent ? reinterpret_cast<QWidget*>(parent) : nullptr;
    
    widget->setParent(parentWidget);
}

void ControlContext::GetControlPosition(const_control_handle handle, int32* x, int32* y)
{
    if (!handle) {
        if (x) *x = 0;
        if (y) *y = 0;
        return;
    }
    
    MockControl* wdg = GetControlBox(handle);
    if (!wdg) return;
    QWidget* widget = wdg->widget;
    QPoint pos = widget->pos();
    
    if (x) *x = pos.x();
    if (y) *y = pos.y();
}

void ControlContext::SetControlPosition(control_handle handle, int32 x, int32 y)
{
    LogDebug("SetControlPosition called, x=" + std::to_string(x) + ", y=" + std::to_string(y));
    
    MockControl* wdg = GetControlBox(handle);
    if (!wdg) return;
    QWidget* widget = wdg->widget;
    widget->move(x, y);
}

sizer_handle ControlContext::GetControlSizer(const_control_handle handle)
{    
    MockControl* wdg = GetControlBox(handle);
    if (!wdg) return nullptr;
    QWidget* widget = wdg->widget;
    QSize size = widget->size();
    
    int w = size.width();
    int h = size.height();
    return nullptr;
}

void ControlContext::SetControlSize(control_handle handle, int32 w, int32 h)
{
    LogDebug("SetControlSize called, w=" + std::to_string(w) + ", h=" + std::to_string(h));
    
    MockControl* wdg = GetControlBox(handle);
    if (!wdg) return;
    QWidget* widget = wdg->widget;
    widget->resize(w, h);
}

void ControlContext::GetControlMinSize(const_control_handle handle, int32* w, int32* h)
{
    if (!handle) {
        if (w) *w = 0;
        if (h) *h = 0;
        return;
    }
    
    MockControl* wdg = GetControlBox(handle);
    if (!wdg) return;
    QWidget* widget = wdg->widget;
    QSize size = widget->minimumSize();
    
    if (w) *w = size.width();
    if (h) *h = size.height();
}

void ControlContext::GetControlMaxSize(const_control_handle handle, int32* w, int32* h)
{
    if (!handle) {
        if (w) *w = 16777215; // Qt's default QWIDGETSIZE_MAX
        if (h) *h = 16777215;
        return;
    }
    
    MockControl* wdg = GetControlBox(handle);
    if (!wdg) return;
    QWidget* widget = wdg->widget;
    QSize size = widget->maximumSize();
    
    if (w) *w = size.width();
    if (h) *h = size.height();
}

void ControlContext::SetControlMaxSize(control_handle handle, int32 w, int32 h)
{
    LogDebug("SetControlMaxSize called, w=" + std::to_string(w) + ", h=" + std::to_string(h));
    
    MockControl* wdg = GetControlBox(handle);
    if (!wdg) return;
    QWidget* widget = wdg->widget;
    widget->setMaximumSize(w, h);
}

/*
void ControlContext::SetControlScaledMinSize(control_handle handle, int32 w, int32 h)
{
    LogDebug("SetControlScaledMinSize called, w=" + std::to_string(w) + ", h=" + std::to_string(h));
    
    MockControl* wdg = GetControlBox(handle);
    if (!wdg) return;
    QWidget* widget = wdg->widget;
    // In a real implementation, you'd scale by DPI
    // For now, just set minimum size directly
    int W = SanitizeSize(w);
    int H = SanitizeSize(h);
    
    widget->setMinimumSize(W, H);
}

void ControlContext::SetControlScaledMaxSize(control_handle handle, int32 w, int32 h)
{
    LogDebug("SetControlScaledMaxSize called, w=" + std::to_string(w) + ", h=" + std::to_string(h));
    
    MockControl* wdg = GetControlBox(handle);
    if (!wdg) return;
    QWidget* widget = wdg->widget;
    widget->setMaximumSize(w, h);
}

void ControlContext::SetControlScaledFixedSize(control_handle handle, int32 w, int32 h)
{
    LogDebug("SetControlScaledFixedSize called, w=" + std::to_string(w) + ", h=" + std::to_string(h));
    
    MockControl* wdg = GetControlBox(handle);
    if (!wdg) return;
    QWidget* widget = wdg->widget;
    widget->setFixedSize(w, h);
}

void ControlContext::SetControlScaledMinWidth(control_handle handle, int32 w)
{
    LogDebug("SetControlScaledMinWidth called, w=" + std::to_string(w));
    
    MockControl* wdg = GetControlBox(handle);
    if (!wdg) return;
    QWidget* widget = wdg->widget;
    widget->setMinimumWidth(w);
}

void ControlContext::SetControlScaledMinHeight(control_handle handle, int32 h)
{
    LogDebug("SetControlScaledMinHeight called, h=" + std::to_string(h));
    
    MockControl* wdg = GetControlBox(handle);
    if (!wdg) return;
    QWidget* widget = wdg->widget;
    widget->setMinimumHeight(h);
}
*/
api_bool ControlContext::GetControlVisible(const_control_handle handle)
{
    MockControl* wdg = GetControlBox(handle);
    if (!wdg) return api_false;
    QWidget* widget = wdg->widget;
    return widget->isVisible() ? api_true : api_false;
}
/*
void ControlContext::ShowControl(control_handle handle)
{
    LogDebug("ShowControl called");
    
    if (!handle && g_lastTopLevelControl) {
        handle = g_lastTopLevelControl;
    }
    
    MockControl* wdg = GetControlBox(handle);
    if (!wdg) return;
    
    QWidget* widget = wdg->widget;
    widget->show();
    widget->raise();
    widget->activateWindow();
}

void ControlContext::HideControl(control_handle handle)
{
    LogDebug("HideControl called");
    
    MockControl* wdg = GetControlBox(handle);
    if (!wdg) return;
    QWidget* widget = wdg->widget;
    widget->hide();
}
*/
api_bool ControlContext::GetControlEnabled(const_control_handle handle)
{
    if (!handle) return api_false;
    MockControl* wdg = GetControlBox(handle);
    if (!wdg) return api_false;
    QWidget* widget = wdg->widget;
    return widget->isEnabled() ? api_true : api_false;
}

void ControlContext::SetControlEnabled(control_handle handle, api_bool enabled)
{
    LogDebug("SetControlEnabled called, enabled=" + std::to_string(enabled));
    
    MockControl* wdg = GetControlBox(handle);
    if (!wdg) return;
    QWidget* widget = wdg->widget;
    widget->setEnabled(enabled != 0);
}
/*
void ControlContext::SetControlToolTip(control_handle handle, const char16_type* tooltip)
{
    if (!handle || !tooltip) return;
    
    std::string tipStr = Utf16ToUtf8(tooltip);
    LogDebug("SetControlToolTip called");
    
    MockControl* wdg = GetControlBox(handle);
    if (!wdg) return;
    QWidget* widget = wdg->widget;
    widget->setToolTip(QString::fromUtf16(reinterpret_cast<const ushort*>(tooltip)));
}
*/
void ControlContext::SetControlFocusStyle(control_handle handle, int32 style)
{
    LogDebug("SetControlFocusStyle called, style=" + std::to_string(style));
    
    MockControl* wdg = GetControlBox(handle);
    if (!wdg) return;
    QWidget* widget = wdg->widget;
    
    // Map PCL focus styles to Qt
    // 0 = NoFocus, 1 = TabFocus, 2 = ClickFocus, 3 = StrongFocus
    switch (style) {
        case 0:
            widget->setFocusPolicy(Qt::NoFocus);
            break;
        case 1:
            widget->setFocusPolicy(Qt::TabFocus);
            break;
        case 2:
            widget->setFocusPolicy(Qt::ClickFocus);
            break;
        case 3:
            widget->setFocusPolicy(Qt::StrongFocus);
            break;
        default:
            widget->setFocusPolicy(Qt::StrongFocus);
    }
}

void ControlContext::RepaintControl(control_handle handle)
{
    LogDebug("RepaintControl called");
    
    MockControl* wdg = GetControlBox(handle);
    if (!wdg) return;
    QWidget* widget = wdg->widget;
    widget->repaint();
}
/*
void ControlContext::SetControlMouseTrackingEnabled(control_handle handle, api_bool status)
{
    LogDebug("EnableMouseTracking called");
    
    MockControl* wdg = GetControlBox(handle);
    if (!wdg) return;
    QWidget* widget = wdg->widget;
    widget->setMouseTracking(status);
}

cursor_handle ControlContext::GetControlCursor(const_control_handle handle)
{
    if (!handle || !cursorShape) return api_false;
    
    MockControl* wdg = GetControlBox(handle);
    if (!wdg) return api_false;
    QWidget* widget = wdg->widget;
    
    // Map Qt cursor to PCL cursor shape
    Qt::CursorShape shape = widget->cursor().shape();
    
    return nullptr;
}

// ----------------------------------------------------------------------------
// LabelContext API
// ----------------------------------------------------------------------------

control_handle LabelContext::CreateLabel(api_handle hModule, control_handle client, 
                                     const char16_type* text, control_handle parent, uint32 flags)
{
    LogDebug("CreateLabel called");
    
    MockLabel* lbl = new MockLabel(text);
    lbl->clientHandle = client;
    
    // Set parent if provided
    if (parent) {
        QWidget* parentWidget = reinterpret_cast<QWidget*>(parent);
        lbl->label->setParent(parentWidget);
    }
    
    control_handle handle = reinterpret_cast<control_handle>(lbl->label);
    
    g_label_map[handle] = lbl;
    
    return handle;
}
*/
int32 LabelContext::GetLabelAlignment(const_control_handle handle)
{
    LogDebug("GetLabelTextAlignment called");
    
    auto it = g_label_map.find(const_cast<control_handle>(handle));
    if (it == g_label_map.end()) {
        return 0;
    }
    
    Qt::Alignment align = it->second->label->alignment();
    
    // Convert Qt alignment to PCL alignment flags
    int32 pclAlign = 0;
    if (align & Qt::AlignLeft) pclAlign |= 0x01;
    if (align & Qt::AlignRight) pclAlign |= 0x02;
    if (align & Qt::AlignHCenter) pclAlign |= 0x04;
    if (align & Qt::AlignTop) pclAlign |= 0x08;
    if (align & Qt::AlignBottom) pclAlign |= 0x10;
    if (align & Qt::AlignVCenter) pclAlign |= 0x20;
    
    return pclAlign;
}
/*
void LabelContext::SetLabelAlignment(control_handle handle, int32 alignment)
{
    LogDebug("SetLabelTextAlignment called, alignment=" + std::to_string(alignment));
    
    auto it = g_label_map.find(handle);
    if (it == g_label_map.end()) {
        return;
    }
    
    // Convert PCL alignment flags to Qt alignment
    Qt::Alignment qtAlign = Qt::AlignLeft | Qt::AlignTop;
    
    if (alignment & 0x01) qtAlign |= Qt::AlignLeft;
    if (alignment & 0x02) qtAlign |= Qt::AlignRight;
    if (alignment & 0x04) qtAlign |= Qt::AlignHCenter;
    if (alignment & 0x08) qtAlign |= Qt::AlignTop;
    if (alignment & 0x10) qtAlign |= Qt::AlignBottom;
    if (alignment & 0x20) qtAlign |= Qt::AlignVCenter;
    
    it->second->label->setAlignment(qtAlign);
}

api_bool LabelContext::GetLabelWordWrappingEnabled(const_control_handle handle)
{
    LogDebug("GetLabelWordWrapping called");
    
    auto it = g_label_map.find(const_cast<control_handle>(handle));
    if (it == g_label_map.end()) {
        return api_false;
    }
    
    return it->second->label->wordWrap() ? api_true : api_false;
}

void LabelContext::SetLabelWordWrappingEnabled(control_handle handle, api_bool wordWrap)
{
    LogDebug("SetLabelWordWrapping called, wordWrap=" + std::to_string(wordWrap));
    
    auto it = g_label_map.find(handle);
    if (it == g_label_map.end()) {
        return;
    }
    
    it->second->label->setWordWrap(wordWrap != 0);
}
*/
int32 LabelContext::GetLabelMargin(const_control_handle handle)
{
    LogDebug("GetLabelMargin called");
    
    auto it = g_label_map.find(const_cast<control_handle>(handle));
    if (it == g_label_map.end()) {
        return 0;
    }
    
    return it->second->label->margin();
}

void LabelContext::SetLabelMargin(control_handle handle, int32 margin)
{
    LogDebug("SetLabelMargin called, margin=" + std::to_string(margin));
    
    auto it = g_label_map.find(handle);
    if (it == g_label_map.end()) {
        return;
    }
    
    it->second->label->setMargin(margin);
}

/*
int32 LabelContext::GetLabelIndent(const_control_handle handle)
{
    LogDebug("GetLabelIndent called");
    
    auto it = g_label_map.find(const_cast<control_handle>(handle));
    if (it == g_label_map.end()) {
        return 0;
    }
    
    return it->second->label->indent();
}

void LabelContext::SetLabelIndent(control_handle handle, int32 indent)
{
    LogDebug("SetLabelIndent called, indent=" + std::to_string(indent));
    
    auto it = g_label_map.find(handle);
    if (it == g_label_map.end()) {
        return;
    }
    
    it->second->label->setIndent(indent);
}

int32 LabelContext::GetLabelFrameStyle(const_control_handle handle)
{
    LogDebug("GetLabelFrameStyle called");
    
    auto it = g_label_map.find(const_cast<control_handle>(handle));
    if (it == g_label_map.end()) {
        return 0;
    }
    
    return it->second->label->frameStyle();
}

void LabelContext::SetLabelFrameStyle(control_handle handle, int32 style)
{
    LogDebug("SetLabelFrameStyle called, style=" + std::to_string(style));
    
    auto it = g_label_map.find(handle);
    if (it == g_label_map.end()) {
        return;
    }
    
    it->second->label->setFrameStyle(style);
}

int32 LabelContext::GetLabelLineWidth(const_control_handle handle)
{
    LogDebug("GetLabelLineWidth called");
    
    auto it = g_label_map.find(const_cast<control_handle>(handle));
    if (it == g_label_map.end()) {
        return 0;
    }
    
    return it->second->label->lineWidth();
}

void LabelContext::SetLabelLineWidth(control_handle handle, int32 width)
{
    LogDebug("SetLabelLineWidth called, width=" + std::to_string(width));
    
    auto it = g_label_map.find(handle);
    if (it == g_label_map.end()) {
        return;
    }
    
    it->second->label->setLineWidth(width);
}

int32 LabelContext::GetLabelMinWidth(const_control_handle handle)
{
    LogDebug("GetLabelMinWidth called");
    
    auto it = g_label_map.find(const_cast<control_handle>(handle));
    if (it == g_label_map.end()) {
        return 0;
    }
    
    return it->second->label->minimumWidth();
}

void LabelContext::SetLabelMinWidth(control_handle handle, int32 width)
{
    LogDebug("SetLabelMinWidth called, width=" + std::to_string(width));
    
    auto it = g_label_map.find(handle);
    if (it == g_label_map.end()) {
        return;
    }
    
    it->second->label->setMinimumWidth(width);
}
*/

// ----------------------------------------------------------------------------
// UI Object ID Management
// ----------------------------------------------------------------------------

// Map to store object IDs
static std::map<api_handle, std::u16string> g_object_id_map;

// ----------------------------------------------------------------------------
// UIContext API
// ----------------------------------------------------------------------------
/*
// SpinBoxContext::GetSpinBoxWrapping
// From: SpinBoxContext::GetSpinBoxWrapping (old.cpp lines 2418-2429)
api_bool SpinBoxContext::GetSpinBoxWrappingEnabled(const_control_handle handle)
{
    LogDebug("GetSpinBoxWrapping called");
    
    auto it = g_spinbox_map.find(const_cast<control_handle>(handle));
    if (it == g_spinbox_map.end()) {
        return api_false;
    }
    
    return it->second->spinBox->wrapping() ? api_true : api_false;
}

// ============================================================================
// TreeBoxContext (21 functions)
// ============================================================================

// TreeBoxContext::Clear
// From: TreeBoxContext::Clear (old.cpp lines 5061-5071)
void TreeBoxContext::ClearTreeBox( control_handle h )
{
    MockTreeBox* tb = GetTreeBox( h );
    if ( !tb )
        return api_false;

    for ( MockTreeNode* n : tb->nodes )
        delete n;
    tb->nodes.clear();
    return api_true;
}

// TreeBoxContext::DisableRootDecoration
// From: TreeBoxContext::DisableRootDecoration (old.cpp lines 5369-5377)
void TreeBoxContext::SetTreeBoxRootDecorationEnabled( control_handle h, api_bool status)
{
    MockTreeBox* tb = GetTreeBox( h );
    if ( !tb )
        return api_false;

    tb->rootDecoration = status;
}

// TreeBoxContext::EnableAlternateRowColor
// From: TreeBoxContext::EnableAlternateRowColor (old.cpp lines 5379-5387)
void TreeBoxContext::SetTreeBoxAlternateRowColorEnabled( control_handle h, api_bool e )
{
    MockTreeBox* tb = GetTreeBox( h );
    if ( !tb )
        return api_false;

    tb->alternateRowColor = (e != api_false);
}

// TreeBoxContext::EnableMultipleSelections
// From: TreeBoxContext::EnableMultipleSelections (old.cpp lines 5359-5367)
void TreeBoxContext::SetTreeBoxMultipleNodeSelectionEnabled( control_handle h, api_bool e )
{
    MockTreeBox* tb = GetTreeBox( h );
    if ( !tb )
        return api_false;

    tb->multipleSelections = (e != api_false);
}

// TreeBoxContext::GetCurrentNode
// From: TreeBoxContext::GetCurrentNode (old.cpp lines 5277-5292)
api_handle TreeBoxContext::GetTreeBoxCurrentNode( const_control_handle h )
{
    MockTreeBox* tb = GetTreeBox( h );
    if ( !tb || !node )
        return api_false;

    for ( MockTreeNode* n : tb->nodes )
        if ( n->selected )
        {
            return reinterpret_cast<api_handle>( n );
        }

    return nullptr;
}

// TreeBoxContext::GetNodeIndex
// From: TreeBoxContext::GetNodeIndex (old.cpp lines 5124-5142)
api_bool TreeBoxContext::GetNodeIndex( control_handle h,
                                   control_handle nodeHandle,
                                   int32* index )
{
    MockTreeBox* tb      = GetTreeBox( h );
    MockTreeNode* target = reinterpret_cast<MockTreeNode*>( nodeHandle );

    if ( !tb || !index || !target )
        return api_false;

    for ( size_t i = 0; i < tb->nodes.size(); ++i )
        if ( tb->nodes[i] == target )
        {
            *index = int32( i );
            return api_true;
        }

    return api_false;
}

// TreeBoxContext::GetNodeText
// From: TreeBoxContext::GetNodeText (old.cpp lines 5161-5182)
api_bool TreeBoxContext::GetNodeText( control_handle hNode, int32 col,
                                  char* buffer, size_type* len )
{
    MockTreeNode* node = reinterpret_cast<MockTreeNode*>( hNode );
    if ( !node || !len )
        return api_false;

    if ( col < 0 || col >= int32( node->text.size() ) )
        return api_false;

    const String& t = node->text[col];

    if ( buffer == nullptr )
    {
        *len = t.Length();
        return api_true;
    }

    // PCL strings are UTF-16 (char16_type). We just copy the raw data.
    ::memcpy( buffer, t.c_str(), t.Length() * sizeof( char16_type ) );
    return api_true;
}

// TreeBoxContext::GetNumberOfColumns
// From: TreeBoxContext::GetNumberOfColumns (old.cpp lines 5041-5049)
api_bool TreeBoxContext::GetNumberOfColumns( control_handle h, int32* n )
{
    MockTreeBox* tb = GetTreeBox( h );
    if ( !tb || n == nullptr )
        return api_false;

    *n = tb->columns;
    return api_true;
}

// TreeBoxContext::GetNumberOfNodes
// From: TreeBoxContext::GetNumberOfNodes (old.cpp lines 5073-5081)
api_bool TreeBoxContext::GetNumberOfNodes( control_handle h, int32* n )
{
    MockTreeBox* tb = GetTreeBox( h );
    if ( !tb || n == nullptr )
        return api_false;

    *n = int32( tb->nodes.size() );
    return api_true;
}

// TreeBoxContext::GetPrevNode
// From: TreeBoxContext::GetPrevNode (old.cpp lines 5332-5353)
api_bool TreeBoxContext::GetPrevNode( control_handle h,
                                  control_handle hNode,
                                  control_handle* prev )
{
    MockTreeBox*  tb   = GetTreeBox( h );
    MockTreeNode* node = reinterpret_cast<MockTreeNode*>( hNode );

    if ( !tb || !node || !prev )
        return api_false;

    for ( size_t i = 0; i < tb->nodes.size(); ++i )
        if ( tb->nodes[i] == node )
        {
            if ( i > 0 )
                *prev = reinterpret_cast<control_handle>( tb->nodes[i-1] );
            else
                *prev = nullptr;
            return api_true;
        }

    return api_false;
}

// TreeBoxContext::HasSelectedNodes
// From: TreeBoxContext::HasSelectedNodes (old.cpp lines 5256-5271)
api_bool TreeBoxContext::HasSelectedNodes( control_handle h, api_bool* r )
{
    MockTreeBox* tb = GetTreeBox( h );
    if ( !tb || !r )
        return api_false;

    *r = api_false;
    for ( MockTreeNode* n : tb->nodes )
        if ( n->selected )
        {
            *r = api_true;
            break;
        }

    return api_true;
}

// TreeBoxContext::IsNodeSelected
// From: TreeBoxContext::IsNodeSelected (old.cpp lines 5232-5240)
api_bool TreeBoxContext::IsNodeSelected( control_handle hNode, api_bool* result )
{
    MockTreeNode* node = reinterpret_cast<MockTreeNode*>( hNode );
    if ( !node || !result )
        return api_false;

    *result = node->selected ? api_true : api_false;
    return api_true;
}

// TreeBoxContext::RemoveNode
// From: TreeBoxContext::RemoveNode (old.cpp lines 5098-5110)
api_bool TreeBoxContext::RemoveNode( control_handle h, int32 index )
{
    MockTreeBox* tb = GetTreeBox( h );
    if ( !tb )
        return api_false;

    if ( index < 0 || index >= int32( tb->nodes.size() ) )
        return api_false;

    delete tb->nodes[index];
    tb->nodes.erase( tb->nodes.begin() + index );
    return api_true;
}

// TreeBoxContext::SelectAllNodes
// From: TreeBoxContext::SelectAllNodes (old.cpp lines 5242-5254)
api_bool TreeBoxContext::SelectAllNodes( control_handle h )
{
    MockTreeBox* tb = GetTreeBox( h );
    if ( !tb )
        return api_false;

    if ( !tb->multipleSelections )
        return api_false;

    for ( MockTreeNode* n : tb->nodes )
        n->selected = true;
    return api_true;
}

// TreeBoxContext::SelectNode
// From: TreeBoxContext::SelectNode (old.cpp lines 5214-5230)
api_bool TreeBoxContext::SelectNode( control_handle h,
                                 control_handle hNode,
                                 api_bool selected )
{
    MockTreeBox*  tb   = GetTreeBox( h );
    MockTreeNode* node = reinterpret_cast<MockTreeNode*>( hNode );

    if ( !tb || !node )
        return api_false;

    if ( !tb->multipleSelections )
        for ( MockTreeNode* n : tb->nodes )
            n->selected = false;

    node->selected = (selected != api_false);
    return api_true;
}

// TreeBoxContext::SetCurrentNode
// From: TreeBoxContext::SetCurrentNode (old.cpp lines 5294-5307)
api_bool TreeBoxContext::SetCurrentNode( control_handle h, control_handle hNode )
{
    MockTreeBox*  tb   = GetTreeBox( h );
    MockTreeNode* node = reinterpret_cast<MockTreeNode*>( hNode );

    if ( !tb || !node )
        return api_false;

    for ( MockTreeNode* n : tb->nodes )
        n->selected = false;

    node->selected = true;
    return api_true;
}

// TreeBoxContext::SetNodeIcon
// From: TreeBoxContext::SetNodeIcon (old.cpp lines 5184-5195)
api_bool TreeBoxContext::SetNodeIcon( control_handle hNode, int32 col, control_handle icon )
{
    MockTreeNode* node = reinterpret_cast<MockTreeNode*>( hNode );
    if ( !node )
        return api_false;

    if ( col < 0 || col >= int32( node->icon.size() ) )
        return api_false;

    node->icon[col] = icon;
    return api_true;
}

// TreeBoxContext::SetNodeText
// From: TreeBoxContext::SetNodeText (old.cpp lines 5148-5159)
api_bool TreeBoxContext::SetNodeText( control_handle hNode, int32 col, const char* text )
{
    MockTreeNode* node = reinterpret_cast<MockTreeNode*>( hNode );
    if ( !node )
        return api_false;

    if ( col < 0 || col >= int32( node->text.size() ) )
        return api_false;

    node->text[col] = text ? String( text ) : String();
    return api_true;
}

// TreeBoxContext::SetNodeToolTip
// From: TreeBoxContext::SetNodeToolTip (old.cpp lines 5197-5208)
api_bool TreeBoxContext::SetNodeToolTip( control_handle hNode, int32 col, const char* text )
{
    MockTreeNode* node = reinterpret_cast<MockTreeNode*>( hNode );
    if ( !node )
        return api_false;

    if ( col < 0 || col >= int32( node->tooltip.size() ) )
        return api_false;

    node->tooltip[col] = text ? String( text ) : String();
    return api_true;
}

// TreeBoxContext::SetNumberOfColumns
// From: TreeBoxContext::SetNumberOfColumns (old.cpp lines 5031-5039)
api_bool TreeBoxContext::SetNumberOfColumns( control_handle h, int32 n )
{
    MockTreeBox* tb = GetTreeBox( h );
    if ( !tb )
        return api_false;

    tb->columns = (n > 0) ? n : 1;
    return api_true;
}


// ============================================================================
// UIContext (1 functions)
// ============================================================================

// UIContext::IsUIObjectValid
// From: UIContext::IsUIObjectValid (old.cpp lines 3619-5757)
api_bool UIContext::IsUIObjectValid(api_handle handle)
{
    if (!handle) {
        return api_false;
    }
    
    // Check if it's a valid QWidget
    QWidget* widget = reinterpret_cast<QWidget*>(handle);
    
    // Simple validation - in a real implementation you might want more checks
    // For now, just check if the pointer seems reasonable and the widget isn't deleted
    try {
        // Try to access a Qt property to see if it's valid
        widget->isVisible();
        return api_true;
    } catch (...) {
        return api_false;
    }
}

void UIContext::DeleteUIObject(api_handle handle)
{
    if (!handle) {
        return;
    }
    
    LogDebug("DeleteUIObject called");
    
    // Remove from ID map
    {
        g_object_id_map.erase(handle);
    }
    
    // Try to delete the widget
    QWidget* widget = reinterpret_cast<QWidget*>(handle);
    if (widget) {
        widget->deleteLater(); // Use deleteLater() for Qt safety
    }
}

api_bool UIContext::GetUIObjectType(const_api_handle handle, char *ptr, size_type *sz)
{
    if (!handle) {
        return "null";
    }
    
    QWidget* widget = reinterpret_cast<QWidget*>(handle);
    if (!widget) {
        return "invalid";
    }

    if (ptr)
      strcpy(ptr, widget->metaObject()->className());
    // Return the Qt metaobject class name
    return api_true;
}
*/
// ----------------------------------------------------------------------------
// Bitmap Mock Implementation
// ----------------------------------------------------------------------------

struct MockBitmap {
    QPixmap pixmap;
    api_handle moduleHandle;
    double devicePixelRatio;
    
    MockBitmap(api_handle hModule) 
        : moduleHandle(hModule),
          devicePixelRatio(1.0)
    {
    }
    
    MockBitmap(api_handle hModule, int width, int height)
        : pixmap(width, height),
          moduleHandle(hModule),
          devicePixelRatio(1.0)
    {
        pixmap.fill(Qt::transparent);
    }
    
    MockBitmap(api_handle hModule, const QPixmap& pm)
        : pixmap(pm),
          moduleHandle(hModule),
          devicePixelRatio(1.0)
    {
    }
    
    ~MockBitmap() {
        // QPixmap handles its own memory
    }
};

// Global map to track bitmaps
static std::map<bitmap_handle, MockBitmap*> g_bitmap_map;
/*
// Helper to get bitmap from handle
static MockBitmap* GetBitmap(bitmap_handle handle) {
    if (!handle) return nullptr;
    
    auto it = g_bitmap_map.find(handle);
    if (it != g_bitmap_map.end()) {
        return it->second;
    }
    return nullptr;
}
*/
// ----------------------------------------------------------------------------
// ComboBox Context Implementation
// ----------------------------------------------------------------------------

struct MockComboBox {
    QComboBox* comboBox;
    api_handle clientHandle;
    
    MockComboBox() 
        : comboBox(new QComboBox()),
          clientHandle(nullptr)
    {
    }
    
    ~MockComboBox() {
        // Qt parent ownership handles deletion
    }
};

// Global map to track comboboxes
static std::map<control_handle, MockComboBox*> g_combobox_map;
/*
control_handle ComboBoxContext::CreateComboBox(api_handle, control_handle client, 
                                           control_handle parent, uint32 flags)
{
    LogDbg("CreateComboBox called");
    
    MockComboBox* combo = new MockComboBox();
    combo->clientHandle = client;
    
    if (parent) {
        QWidget* parentWidget = reinterpret_cast<QWidget*>(parent);
        combo->comboBox->setParent(parentWidget);
    }
    
    control_handle handle = reinterpret_cast<control_handle>(combo->comboBox);
    
    g_combobox_map[handle] = combo;
    
    return handle;
}

// ----------------------------------------------------------------------------
// Edit Context Implementation
// ----------------------------------------------------------------------------
  
control_handle EditContext::CreateEdit(api_handle module,
                                   control_handle client,
                                   const char16_type* text,
                                   control_handle parent,
                                   uint32 flags)
{
    LogDbg("CreateEdit called");

    MockEdit* mock = new MockEdit(text);

    // 1) store the PCL Edit* (sender for EditCompleted)
    mock->pclEdit = client; // this is exactly 'this' from pcl::Edit ctor

    // 2) parent the Qt widget if needed
    if (parent) {
        QWidget* parentWidget = reinterpret_cast<QWidget*>(parent);
        mock->edit->setParent(parentWidget);
    }

    // 3) This is the handle PCL will use for this edit control
    control_handle handle = reinterpret_cast<control_handle>(mock->edit);

    {
        g_edit_map[handle] = mock;
    }

    return handle;
}
*/  
api_bool EditContext::GetEditText(const_control_handle handle, char16_type* text, size_type* len)
{
    LogDbg("GetEditText called");
    
    auto it = g_edit_map.find(const_cast<control_handle>(handle));
    if (it == g_edit_map.end()) {
        if (len) *len = 0;
        return api_false;
    }
    
    QString qtext = it->second->edit->text();
    std::u16string u16text = qtext.toStdU16String();
    
    if (text == nullptr) {
        if (len) *len = u16text.length();
        return api_true;
    }
    
    if (len && *len > 0) {
        size_type copyLen = std::min(*len - 1, u16text.length());
        std::memcpy(text, u16text.c_str(), copyLen * sizeof(char16_type));
        text[copyLen] = 0;
        *len = copyLen;
    }
    
    return api_true;
}

api_bool EditContext::GetEditReadOnly(const_control_handle handle)
{
    auto it = g_edit_map.find(const_cast<control_handle>(handle));
    if (it == g_edit_map.end()) {
        return api_false;
    }
    
    return it->second->edit->isReadOnly() ? api_true : api_false;
}

// ----------------------------------------------------------------------------
// TextBox Context Implementation (QTextEdit)
// ----------------------------------------------------------------------------

struct MockTextBox {
    QTextEdit* textEdit;
    api_handle clientHandle;
    
    MockTextBox(const char16_type* text = nullptr) 
        : textEdit(new QTextEdit()),
          clientHandle(nullptr)
    {
        if (text && *text) {
            textEdit->setText(QString::fromUtf16(reinterpret_cast<const ushort*>(text)));
        }
    }
    
    ~MockTextBox() {
        // Qt parent ownership handles deletion
    }
};

// Global map to track textboxes
static std::map<control_handle, MockTextBox*> g_textbox_map;

// ----------------------------------------------------------------------------
// Slider Context Implementation
// ----------------------------------------------------------------------------

struct MockSlider {
    QSlider* slider;
    api_handle clientHandle;
    
    MockSlider(bool vertical) 
        : slider(new QSlider(vertical ? Qt::Vertical : Qt::Horizontal)),
          clientHandle(nullptr)
    {
    }
    
    ~MockSlider() {
        // Qt parent ownership handles deletion
    }
};

// Global map to track sliders
static std::map<control_handle, MockSlider*> g_slider_map;

struct MockTimer {
    QTimer* timer;
    api_handle clientHandle;
    pcl::timer_event_routine timeoutHandler;

    MockTimer()
        : timer(new QTimer()),
          clientHandle(nullptr),
          timeoutHandler(nullptr)
    {
        timer->setSingleShot(false); // PixInsight timers are repeating by default
    }

    ~MockTimer() {
        // QTimer deleted by Qt parent hierarchy if parented
    }
};

static std::map<control_handle, MockTimer*> g_timer_map;

struct MockFont {
    QFont font;
    api_handle clientHandle;

    MockFont(const QFont& f, api_handle client)
        : font(f)
        , clientHandle(client)
    {
    }
};

static std::map<const_font_handle, MockFont*> g_font_map;

font_handle FontContext::CreateFontByFamily(api_handle client, int32 weight, double sizePt)
{
    QString family = QFont().defaultFamily();  // default family
    QFont f(family, sizePt);
    f.setPointSizeF(sizePt);
    f.setWeight(weight);

    MockFont* mf = new MockFont(f, client);

    font_handle handle = reinterpret_cast<font_handle>(mf);
    {
        g_font_map[handle] = mf;
    }
    return handle;
}

font_handle FontContext::CreateFontByFace(api_handle client, const char16_type* face, double ptSize)
{
    QString family = QString::fromUtf16(reinterpret_cast<const ushort*>(face));
    QFont f(family, ptSize);
    f.setPointSizeF(ptSize);

    MockFont* mf = new MockFont(f, client);

    font_handle h = reinterpret_cast<font_handle>(mf);
    {
        g_font_map[h] = mf;
    }
    return h;
}

font_handle ControlContext::GetControlFont(const_control_handle handle)
{
    LogDbg("API_Control_GetControlFont called");

    const QWidget* w = widgetFromHandle(handle);
    if (!w)
        return nullptr;

    QFont f = w->font();

    MockFont* mf = new MockFont(f, nullptr);

    font_handle h = reinterpret_cast<font_handle>(mf);
    {
        g_font_map[h] = mf;
    }

    return h;
}

struct MockCursor {
    QCursor cursor;
    api_handle clientHandle;

    MockCursor(const QCursor& c, api_handle client)
        : cursor(c)
        , clientHandle(client)
    {
    }
};

static std::map<cursor_handle, MockCursor*> g_cursor_map;

cursor_handle CursorContext::CreateCursor(api_handle client,
                                      int32 hot)
{
    LogDbg("API_Cursor_CreateCursor called");

    QString bitmapFile;
    QString maskFile;
    QCursor qc;

    if (!bitmapFile.isEmpty())
    {
        QPixmap pm(bitmapFile);
        if (!pm.isNull())
        {
            if (!maskFile.isEmpty()) {
                QBitmap mask(maskFile);
                if (!mask.isNull())
                    pm.setMask(mask);
            }
            qc = QCursor(pm, hot, hot);
        }
        else {
            LogDbg("[Cursor] Failed to load bitmap: " + bitmapFile);
            qc = QCursor(Qt::ArrowCursor);
        }
    }
    else {
        // No-file case: default to Arrow cursor
        qc = QCursor(Qt::ArrowCursor);
    }

    MockCursor* mc = new MockCursor(qc, client);

    cursor_handle h = reinterpret_cast<cursor_handle>(mc);

    {
        g_cursor_map[h] = mc;
    }

    return h;
}

static std::map<window_handle, MockImageWindow*> g_image_window_map;

struct MockView {
    MockImageStruct* image;
    QString identifier;
    api_handle clientHandle;
};

// View handling
static std::map<const_view_handle, MockView*> g_view_map;

// -----------------------------------------------------------------------------
// TreeBox Mock API
// -----------------------------------------------------------------------------
/*
// Small helper – avoids dynamic_cast on non-polymorphic MockControl.
static MockTreeBox* GetTreeBox( control_handle h )
{
    auto it = g_control_map.find( h );
    if ( it == g_control_map.end() )
        return nullptr;
    return static_cast<MockTreeBox*>( it->second );
}
*/
// -----------------------------------------------------------------------------
// Column management
// -----------------------------------------------------------------------------
/*
void TreeBoxContext::SetTreeBoxColumnCount( control_handle h, int32 n )
{
    MockTreeBox* tb = GetTreeBox( h );
    if ( !tb )
        return api_false;

    tb->columns = (n > 0) ? n : 1;
}

int32 TreeBoxContext::GetTreeBoxColumnCount( const_control_handle h)
{
    MockTreeBox* tb = GetTreeBox( h );
    if ( !tb )
        return 0;

    return tb->columns;
}

// -----------------------------------------------------------------------------
// Node management
// -----------------------------------------------------------------------------

void TreeBoxContext::ClearTreeBox( control_handle h )
{
    MockTreeBox* tb = GetTreeBox( h );
    if ( !tb )
        return api_false;

    for ( MockTreeNode* n : tb->nodes )
        delete n;
    tb->nodes.clear();
    return api_true;
}

api_bool TreeBoxContext::GetNumberOfNodes( control_handle h, int32* n )
{
    MockTreeBox* tb = GetTreeBox( h );
    if ( !tb || n == nullptr )
        return api_false;

    *n = int32( tb->nodes.size() );
    return api_true;
}

void TreeBoxContext::InsertTreeBoxNode( control_handle h, int32 index, api_handle itm )
{
    MockTreeBox* tb = GetTreeBox( h );
    if ( !tb )
        return nullptr;

    if ( index < 0 || index > int32( tb->nodes.size() ) )
        index = int32( tb->nodes.size() );

    MockTreeNode* node = new MockTreeNode( tb->columns );
    tb->nodes.insert( tb->nodes.begin() + index, node );

    return reinterpret_cast<control_handle>( node );
}

api_handle TreeBoxContext::GetTreeBoxNodeByPos( const_control_handle h, int32 x, int32 y )
{
    const MockTreeBox* tb = GetTreeBox( h );
    if ( !tb )
        return nullptr;

    if ( index < 0 || index >= int32( tb->nodes.size() ) )
        return nullptr;

    return reinterpret_cast<api_handle>( tb->nodes[index] );
}

api_bool TreeBoxContext::GetNodeIndex( control_handle h,
                                   control_handle nodeHandle,
                                   int32* index )
{
    MockTreeBox* tb      = GetTreeBox( h );
    MockTreeNode* target = reinterpret_cast<MockTreeNode*>( nodeHandle );

    if ( !tb || !index || !target )
        return api_false;

    for ( size_t i = 0; i < tb->nodes.size(); ++i )
        if ( tb->nodes[i] == target )
        {
            *index = int32( i );
            return api_true;
        }

    return api_false;
}

// -----------------------------------------------------------------------------
// Node text & icon & tooltip
// -----------------------------------------------------------------------------

void TreeBoxContext::SetTreeBoxNodeColText( api_handle hNode, int32 col, const char16_type *text )
{
    MockTreeNode* node = reinterpret_cast<MockTreeNode*>( hNode );
    if ( !node )
        return api_false;

    if ( col < 0 || col >= int32( node->text.size() ) )
        return api_false;

    node->text[col] = text ? String( text ) : String();
    return api_true;
}

api_bool TreeBoxContext::GetTreeBoxNodeColText( const_api_handle hNode, int32 col,
                                  char16_type* buffer, size_type* len )
{
    MockTreeNode* node = reinterpret_cast<MockTreeNode*>( hNode );
    if ( !node || !len )
        return api_false;

    if ( col < 0 || col >= int32( node->text.size() ) )
        return api_false;

    const String& t = node->text[col];

    if ( buffer == nullptr )
    {
        *len = t.Length();
        return api_true;
    }

    // PCL strings are UTF-16 (char16_type). We just copy the raw data.
    ::memcpy( buffer, t.c_str(), t.Length() * sizeof( char16_type ) );
    return api_true;
}

void TreeBoxContext::SetTreeBoxNodeColIcon( api_handle hNode, int32 col, const_bitmap_handle icon )
{
    MockTreeNode* node = reinterpret_cast<MockTreeNode*>( hNode );
    if ( !node )
        return api_false;

    if ( col < 0 || col >= int32( node->icon.size() ) )
        return api_false;

    node->icon[col] = icon;
    return api_true;
}

void TreeBoxContext::SetTreeBoxNodeColToolTip( api_handle hNode, int32 col, const char16_type *text )
{
    MockTreeNode* node = reinterpret_cast<MockTreeNode*>( hNode );
    if ( !node )
        return api_false;

    if ( col < 0 || col >= int32( node->tooltip.size() ) )
        return api_false;

    node->tooltip[col] = text ? String( text ) : String();
    return api_true;
}

// -----------------------------------------------------------------------------
// Selection
// -----------------------------------------------------------------------------

void TreeBoxContext::SetTreeBoxNodeSelected( api_handle h,
                                 api_bool selected )
{
    MockTreeBox*  tb   = GetTreeBox( h );
    MockTreeNode* node = reinterpret_cast<MockTreeNode*>( hNode );

    if ( !tb || !node )
        return api_false;

    if ( !tb->multipleSelections )
        for ( MockTreeNode* n : tb->nodes )
            n->selected = false;

    node->selected = (selected != api_false);
    return api_true;
}

api_bool TreeBoxContext::GetTreeBoxNodeSelected( const_api_handle hNode)
{
    MockTreeNode* node = reinterpret_cast<MockTreeNode*>( hNode );
    if ( !node )
        return api_false;

    return node->selected ? api_true : api_false;
}

void TreeBoxContext::SelectAllTreeBoxNodes( control_handle h )
{
    MockTreeBox* tb = GetTreeBox( h );
    if ( !tb )
        return api_false;

    if ( !tb->multipleSelections )
        return api_false;

    for ( MockTreeNode* n : tb->nodes )
        n->selected = true;
}

api_bool TreeBoxContext::GetTreeBoxSelectedNodes( const_control_handle h, api_handle *a, size_type *sz )
{
    MockTreeBox* tb = GetTreeBox( h );
    if ( !tb || !r )
        return api_false;

    *r = 0;
    for ( MockTreeNode* n : tb->nodes )
        if ( n->selected )
        {
	  (*r)++;
        }

    return api_true;
}

// -----------------------------------------------------------------------------
// Navigation
// -----------------------------------------------------------------------------

api_handle TreeBoxContext::GetTreeBoxCurrentNode( const_control_handle h )
{
    MockTreeBox* tb = GetTreeBox( h );
    if ( !tb || !node )
        return api_false;

    for ( MockTreeNode* n : tb->nodes )
        if ( n->selected )
        {
            *node = reinterpret_cast<control_handle>( n );
            return api_true;
        }

    *node = nullptr;
    return api_true;
}

void TreeBoxContext::SetTreeBoxCurrentNode( control_handle h, api_handle hNode )
{
    MockTreeBox*  tb   = GetTreeBox( h );
    MockTreeNode* node = reinterpret_cast<MockTreeNode*>( hNode );

    if ( !tb || !node )
        return api_false;

    for ( MockTreeNode* n : tb->nodes )
        n->selected = false;

    node->selected = true;
    return api_true;
}

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------

void TreeBoxContext::SetTreeBoxMultipleNodeSelectionEnabled( control_handle h, api_bool e )
{
    MockTreeBox* tb = GetTreeBox( h );
    if ( !tb )
        return api_false;

    tb->multipleSelections = (e != api_false);
    return api_true;
}

void TreeBoxContext::SetTreeBoxAlternateRowColorEnabled( control_handle h, api_bool e )
{
    MockTreeBox* tb = GetTreeBox( h );
    if ( !tb )
        return api_false;

    tb->alternateRowColor = (e != api_false);
    return api_true;
}
*/

// -----------------------------------------------------------------------------
// Height management
// -----------------------------------------------------------------------------

void TreeBoxContext::SetTreeBoxUniformRowHeightEnabled( control_handle, api_bool )
{

}

/*
api_bool TreeBoxContext::SetMinHeight( control_handle, int32 )
{
    return api_true;
}

api_bool TreeBoxContext::SetMaxHeight( control_handle, int32 )
{
    return api_true;
}

// -----------------------------------------------------------------------------
// Viewport
// -----------------------------------------------------------------------------

control_handle TreeBoxContext::GetViewportHandle( control_handle h )
{
    MockTreeBox* tb = GetTreeBox( h );
    return tb ? tb->viewport : nullptr;
}
*/

// -----------------------------------------------------------------------------
// Event routines (stored but not auto-invoked in the mock)
// -----------------------------------------------------------------------------

typedef void *treebox_currentnodeupdated_event;
typedef void * treebox_node_event ;
typedef void * treebox_tree_event ;

struct TreeBoxEventRoutines
{
    treebox_currentnodeupdated_event currentNodeUpdated = nullptr;
    treebox_node_event               nodeActivated      = nullptr;
    treebox_tree_event               selectionUpdated   = nullptr;
};

static std::map<control_handle, TreeBoxEventRoutines> g_tree_events;
/*
api_bool TreeBoxContext::SetTreeBoxCurrentNodeUpdatedEventRoutine( control_handle h,
                                                        api_handle receiver,
                                                        pcl::item_range_event_routine f )
{
    g_tree_events[h].currentNodeUpdated = f;
    return api_true;
}

api_bool TreeBoxContext::SetTreeBoxNodeActivatedEventRoutine( control_handle h,
                                                   api_handle receiver,
                                                   item_value_event_routine f )
{
    g_tree_events[h].nodeActivated = f;
    return api_true;
}

api_bool TreeBoxContext::SetTreeBoxNodeSelectionUpdatedEventRoutine( control_handle h,
                                                          api_handle receiver,
                                                          event_routine f )
{
    g_tree_events[h].selectionUpdated = f;
    return api_true;
}
*/
// Mock implementations for Global API functions

// Simple error code storage
static int g_last_error = 0;

// Mock console handle
static void* g_console_handle = (void*)0xDEADBEEF;

// Mock pixel traits LUT - create a simple lookup table
struct MockPixelTraitsLUT {
    // These would normally be function pointers, but we'll make them simple values
    int sample_format;
    int bytes_per_sample;
    int bits_per_sample;
    double min_sample_value;
    double max_sample_value;
};

// Create mock LUTs for different pixel formats
static MockPixelTraitsLUT g_pixel_luts[16] = {
    // Format 0: 8-bit unsigned integer
    { 0, 1, 8, 0.0, 255.0 },
    // Format 1: 16-bit unsigned integer  
    { 1, 2, 16, 0.0, 65535.0 },
    // Format 2: 32-bit unsigned integer
    { 2, 4, 32, 0.0, 4294967295.0 },
    // Format 3: 32-bit IEEE 754 floating point
    { 3, 4, 32, 0.0, 1.0 },
    // Format 4: 64-bit IEEE 754 floating point
    { 4, 8, 64, 0.0, 1.0 },
    // Add more formats as needed...
};

// ----------------------------------------------------------------------------
// Global Settings mock
// ----------------------------------------------------------------------------


// settings[module][key] = int
static std::map<api_handle, std::map<std::string, int32>> g_settings_local;
static std::map<std::string, int32> g_settings_global;

static void preload_default_global_settings()
{

    if (!g_settings_global.count("Workspace/PrimaryScreenCenterX"))
        g_settings_global["Workspace/PrimaryScreenCenterX"] = 400;

    if (!g_settings_global.count("Workspace/PrimaryScreenCenterY"))
        g_settings_global["Workspace/PrimaryScreenCenterY"] = 300;
}

api_bool GlobalContext::ReadSettingsInteger( api_handle module,
                                         int32*     outValue,
                                         const char* key,
                                         api_bool   global )
{
    LogDbg("API_Global_ReadSettingsInteger called");
    preload_default_global_settings();
 
    if (!outValue || !key)
        return api_false;

    std::string skey(key);


    if (global)
    {
        auto it = g_settings_global.find(skey);
        if (it == g_settings_global.end())
	  {
	    LogDbg("API_Global_ReadSettingsInteger missing: " + skey);
            return api_false;
	  }
        *outValue = it->second;
        return api_true;
    }
    else
    {
        auto modIt = g_settings_local.find(module);
        if (modIt == g_settings_local.end())
	  {
	    LogDbg("API_Local_ReadSettingsInteger missing: " + skey);
            return api_false;
	  }
	
        auto& m = modIt->second;
        auto it = m.find(skey);
        if (it == m.end())
            return api_false;

        *outValue = it->second;
        return api_true;
    }
}

// Mock for GetPixelTraitsLUT
void* GetPixelTraitsLUT(int format) {
    LogDbg("GetPixelTraitsLUT called with format: " + std::to_string(format));
    
    if (format >= 0 && format < 16) {
        return &g_pixel_luts[format];
    }
    return &g_pixel_luts[0]; // Default to format 0
}

// Mock for GetConsole
void* GetConsole() {
    LogDbg("GetConsole called");
    return g_console_handle;
}

// Mock for LastError
uint32 GlobalContext::LastError() {
    LogDbg("LastError called, returning: " + std::to_string(g_last_error));
    return g_last_error;
}

// Mock for setting error
void SetLastError(int error_code) {
    g_last_error = error_code;
    LogDbg("SetLastError called with: " + std::to_string(error_code));
}

// Mock for ClearError
void ClearError() {
    g_last_error = 0;
    LogDbg("ClearError called");
}

// Mock for ProcessEvents
void GlobalContext::ProcessEvents(api_bool excludeUserInputEvents) {
    LogDbg("ProcessEvents called");
}

// Mock for GetApplicationInstanceSlot
int GetApplicationInstanceSlot() {
    LogDbg("GetApplicationInstanceSlot called");
    return 0; // Root slot
}

// Mock for GetProcessStatus
uint32_t GlobalContext::GetProcessStatus() {
    LogDbg("GetProcessStatus called");
    // Return a status that indicates not aborted (bit 31 clear)
    // PCL checks if bit 31 (0x80000000) is set to determine if process should abort
    return 0x00000000; // Normal status, not aborted
}

// Mock for EnableAbort
int EnableAbort() {
  LogDbg("EnableAbort called");
  return 1; // api_true - success
}

// Mock for GetGlobalFlag
api_bool GlobalContext::GetGlobalFlag(const char* flag_name, api_bool* value) {
    LogDbg("GetGlobalFlag called with: " + std::string(flag_name ? flag_name : "(null)"));
    
    if (!value) {
        return 0; // api_false
    }
    
    // Return default values for common flags
    if (flag_name) {
        std::string name = flag_name;
        if (name.find("Abort") != std::string::npos) {
            *value = 0; // Not aborted
        } else if (name.find("Debug") != std::string::npos) {
            *value = 0; // Debug off
        } else {
            *value = 0; // Default to false/off
        }
    } else {
        *value = 0;
    }
    
    return 1; // api_true - success
}

// Mock for GetGlobalInteger
api_bool GlobalContext::GetGlobalInteger(const char* int_name, void* value, api_bool isSigned) {
    std::string skey(int_name ? int_name : "(null)");
    LogDbg("GetGlobalInteger called with: " + skey);
    preload_default_global_settings();
    
    if (!value) {
        return 0; // api_false
    }

    auto it = g_settings_global.find(skey);
    if (it == g_settings_global.end())
	  {
	    LogDbg("API_Global_GetGlobalInteger missing: " + skey);
            return api_false;
	  }
    *(int *)value = it->second;
    return api_true;
}

// =============================================================
// Control Context - Stubs and Additional Functions
// =============================================================

void ControlContext::SetChildControlToFocus(control_handle, control_handle) {}
void ControlContext::SetRealTimePreviewActive(control_handle, api_bool) {}
void ControlContext::SetControlFocus(control_handle, api_bool) {}
void ControlContext::SetControlUpdatesEnabled(control_handle, api_bool) {}
void ControlContext::AdjustControlToContents(control_handle) {}
api_bool ControlContext::SetGetFocusEventRoutine(control_handle, control_handle, pcl::control_event_routine) { return api_true; }
api_bool ControlContext::SetLoseFocusEventRoutine(control_handle, control_handle, pcl::control_event_routine) { return api_true; }
api_bool ControlContext::SetFileDragEventRoutine(control_handle, control_handle, pcl::file_drag_event_handler) { return api_true; }
api_bool ControlContext::SetFileDropEventRoutine(control_handle, control_handle, pcl::file_drag_event_handler) { return api_true; }
api_bool ControlContext::GetControlDisplayPixelRatio(const_control_handle, double* ratio) { *ratio = 1.0; return api_true; }
void ControlContext::GetControlExpansionEnabled(const_control_handle, api_bool*, api_bool*) {}
void ControlContext::SetControlExpansionEnabled(control_handle, api_bool, api_bool) {}
api_bool ControlContext::GetControlUnderMouseStatus(const_control_handle) { return api_false; }
void ControlContext::BringControlToFront(control_handle) {}
void ControlContext::SendControlToBack(control_handle) {}
void ControlContext::StackControls(control_handle, control_handle) {}
void ControlContext::GlobalToLocal(const_control_handle, int32*, int32*) {}
void ControlContext::LocalToGlobal(const_control_handle, int32*, int32*) {}
void ControlContext::ParentToLocal(const_control_handle, int32*, int32*) {}
void ControlContext::LocalToParent(const_control_handle, int32*, int32*) {}
void ControlContext::ControlToLocal(const_control_handle, const_control_handle, int32*, int32*) {}
void ControlContext::LocalToControl(const_control_handle, const_control_handle, int32*, int32*) {}
control_handle ControlContext::GetChildByPos(const_control_handle, int32, int32) { return nullptr; }
void ControlContext::GetChildrenRect(const_control_handle, int32*, int32*, int32*, int32*) {}
api_bool ControlContext::GetControlAncestry(const_control_handle, const_control_handle) { return api_false; }
void EditContext::SetEditSelected(control_handle, api_bool) {}
api_bool EditContext::SetEditCompletedEventRoutine(control_handle, control_handle, pcl::event_routine) { return api_true; }
api_bool EditContext::SetReturnPressedEventRoutine(control_handle, control_handle, pcl::event_routine) { return api_true; }

api_bool             FileFormatContext::EnumerateFileFormats( pcl::format_enumeration_callback, void* )
{
  return api_false;
}


// =============================================================
// END OF IMPLEMENTATION
// =============================================================
