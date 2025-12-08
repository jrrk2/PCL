// ===============================================================
//  PCLMockAPI.h  —  Unified Mock PixInsight Core API (Header)
// ===============================================================
#pragma once
#include <cstdint>
#include <QtWidgets>
#include <pcl/api/APIDefs.h>
#include <pcl/ProcessInterface.h>

// ---------------------------------------------------------------
// Basic Types
// ---------------------------------------------------------------
typedef void* api_handle;
typedef control_handle label_handle;
typedef control_handle edit_handle;
typedef control_handle slider_handle;
typedef control_handle button_handle;
typedef control_handle combo_handle;
typedef control_handle spin_handle;
typedef void* sizer_handle;
typedef void* thread_handle;

function_resolver GetMockFunctionResolver();

void LogDebug(const std::string& message);

static inline void LogDbg(const std::string& msg) {
    LogDebug(msg.c_str());
}
/*
static inline void LogDbg(const std::string& msg) {
    LogDebug(msg.c_str());
}
*/
static inline void LogDbg(const QString& msg) {
  //    LogDebug(msg);
}

inline void LogDbg(const char* msg) {
    LogDebug(msg);
}

// ---------------------------------------------------------------
// Event Routine Typedefs (Stubs)
// ---------------------------------------------------------------
namespace pcl
{
    typedef void (*edit_event_routine)(api_handle receiver,
                                       edit_handle edit,
                                       const char16_t* text);
    typedef void (*api_button_event_routine)(api_handle receiver,
                                             button_handle btn);
    typedef void (*api_slider_value_event_routine)(api_handle receiver,
                                                   slider_handle slider,
                                                   int value);
    typedef void (*api_spinbox_value_event_routine)(api_handle receiver,
                                                    spin_handle spin,
                                                    int value);
}

// =============================================================
// Mock Object: Unified structure for all object types
// =============================================================
struct MockBase
{
    // Type flags
    bool isSizer = false;
    bool isTreeNode = false;
    
    // Module ownership
    api_handle moduleHandle = nullptr;
    
    // PCL-side handle (client pointer - this is the KEY in g_objects)
    control_handle pcl_handle = nullptr;
    
    // Qt objects (only ONE should be set based on type)
    QWidget* widget = nullptr;              // For controls
    QBoxLayout* layout = nullptr;           // For sizers
    QTreeWidgetItem* treeItem = nullptr;    // For tree nodes
    
    // Layout properties
    bool vertical = false;
    
    // Event receiver for callbacks
    control_handle eventReceiver = nullptr;
    
    // Control event callbacks
    pcl::control_event_routine onShow = nullptr;
    pcl::mouse_event_routine onMouseMove = nullptr;
    pcl::mouse_button_event_routine onMousePress = nullptr;
    pcl::mouse_button_event_routine onMouseRelease = nullptr;
    pcl::keyboard_event_routine onKeyPress = nullptr;
    
    // Button event callbacks
    pcl::button_click_event_routine onButtonClick = nullptr;
    pcl::button_check_event_routine onButtonCheck = nullptr;
    
    // TreeBox event callbacks
    pcl::item_value_event_routine onTreeNodeActivated = nullptr;
    pcl::item_range_event_routine onTreeNodeUpdated = nullptr;
    pcl::event_routine onTreeSelectionUpdated = nullptr;

    // Add these for Timer support:
    api_handle timer_api_handle = nullptr;
    QTimer* qtTimer = nullptr;
    void (*onTimerNotify)(timer_handle, pcl::Control*) = nullptr;
    api_handle timerReceiver = nullptr;

    pcl::control_event_routine onClose;
    api_handle process_handle;
    pcl::wheel_event_routine onMouseWheel;
    pcl::mouse_event_routine onMouseDoubleClick;

    pcl::control_event_routine onEnter;
    pcl::control_event_routine onLeave;
    pcl::resize_event_routine onResize;
    pcl::keyboard_event_routine onKeyRelease;

    // Destructor - Qt manages memory via parent-child relationships
    ~MockBase()
    {
        // Only delete if no Qt parent manages it
        if (widget && !widget->parent()) {
	  //            delete widget;
            widget = nullptr;
        }
        
        // Tree items are owned by their tree or parent item
        if (treeItem && !treeItem->treeWidget() && !treeItem->parent()) {
            delete treeItem;
            treeItem = nullptr;
        }
        
        // Layouts are owned by their parent widget
        if (layout && !layout->parent()) {
            delete layout;
            layout = nullptr;
        }
    }
};

extern QList<MockBase*> g_topLevelWidgets;


// ============================================================================
// Mock Image Window Storage
// ============================================================================

struct MockImageWindow
{
    QWidget* widget = nullptr;
    QLabel* imageLabel = nullptr;
    QScrollArea* scrollArea = nullptr;
    
    std::string id;
    int width = 0;
    int height = 0;
    int channels = 0;
    int bitsPerSample = 0;
    bool floatSample = false;
    
    // Image data storage
    std::vector<float> imageData;  // For float images
    std::vector<uint16_t> imageData16;  // For 16-bit images
    
    view_handle mainView = nullptr;
    int zoomFactor = 1;
    bool isVisible = true;
    
    MockImageWindow(const std::string& windowId, int w, int h, int ch, int bits, bool isFloat)
        : id(windowId), width(w), height(h), channels(ch), bitsPerSample(bits), floatSample(isFloat)
    {
        // Allocate image data
        size_t pixelCount = width * height * channels;
        if (floatSample)
            imageData.resize(pixelCount, 0.0f);
        else
            imageData16.resize(pixelCount, 0);
        
        // Create Qt widget for display
        widget = new QWidget();
        widget->setWindowTitle(QString::fromStdString(id));
        
        QVBoxLayout* layout = new QVBoxLayout(widget);
        
        scrollArea = new QScrollArea(widget);
        imageLabel = new QLabel();
        imageLabel->setScaledContents(false);
        scrollArea->setWidget(imageLabel);
        scrollArea->setWidgetResizable(false);
        
        layout->addWidget(scrollArea);
        widget->resize(width + 20, height + 20);
    }
    
    ~MockImageWindow()
    {
        if (widget)
            delete widget;
    }
    
    void updateDisplay()
    {
        // Convert our image data to QImage for display
        QImage qimg(width, height, channels == 1 ? QImage::Format_Grayscale8 : QImage::Format_RGB888);
        
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                if (floatSample)
                {
                    // Convert float [0,1] to 8-bit [0,255]
                    if (channels == 1)
                    {
                        float val = imageData[y * width + x];
                        int gray = static_cast<int>(val * 255.0f);
                        gray = std::max(0, std::min(255, gray));
                        qimg.setPixel(x, y, qRgb(gray, gray, gray));
                    }
                    else if (channels == 3)
                    {
                        int idx = (y * width + x) * 3;
                        int r = static_cast<int>(imageData[idx + 0] * 255.0f);
                        int g = static_cast<int>(imageData[idx + 1] * 255.0f);
                        int b = static_cast<int>(imageData[idx + 2] * 255.0f);
                        r = std::max(0, std::min(255, r));
                        g = std::max(0, std::min(255, g));
                        b = std::max(0, std::min(255, b));
                        qimg.setPixel(x, y, qRgb(r, g, b));
                    }
                }
                else
                {
                    // Convert 16-bit to 8-bit for display
                    if (channels == 1)
                    {
                        int gray = imageData16[y * width + x] >> 8;
                        qimg.setPixel(x, y, qRgb(gray, gray, gray));
                    }
                    else if (channels == 3)
                    {
                        int idx = (y * width + x) * 3;
                        int r = imageData16[idx + 0] >> 8;
                        int g = imageData16[idx + 1] >> 8;
                        int b = imageData16[idx + 2] >> 8;
                        qimg.setPixel(x, y, qRgb(r, g, b));
                    }
                }
            }
        }
        
        QPixmap pixmap = QPixmap::fromImage(qimg);
        if (zoomFactor != 1)
        {
            int scaledWidth = width * zoomFactor;
            int scaledHeight = height * zoomFactor;
            pixmap = pixmap.scaled(scaledWidth, scaledHeight, Qt::KeepAspectRatio, Qt::FastTransformation);
        }
        
        imageLabel->setPixmap(pixmap);
        imageLabel->resize(pixmap.size());
    }
    
    void zoomToFit()
    {
        if (!widget || !scrollArea) return;
        
        int viewWidth = scrollArea->viewport()->width() - 20;
        int viewHeight = scrollArea->viewport()->height() - 20;
        
        float scaleX = static_cast<float>(viewWidth) / width;
        float scaleY = static_cast<float>(viewHeight) / height;
        float scale = std::min(scaleX, scaleY);
        
        zoomFactor = std::max(1, static_cast<int>(scale));
        updateDisplay();
    }
};

// ---------------------------------------------------------------
// Core APIs
// ---------------------------------------------------------------
extern "C" {
void SetDebugLogging(bool);
void MockMainWriteConsole(const char16_type *text, bool appendNewline);
};
