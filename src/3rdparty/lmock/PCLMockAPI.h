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

struct MockImage
{
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t numberOfChannels = 0;
    uint32_t bitsPerSample = 32;
    bool floatSample = true;
    uint32_t colorSpace = 0;  // 0=Gray, 1=RGB
    
    void* owner = nullptr;
    uint32_t refCount = 1;
    
    // Pixel data - one pointer per channel
    std::vector<void*> channelData;
    
    MockImage(uint32_t w, uint32_t h, uint32_t n, uint32_t bits, bool flt, uint32_t cs, void* own)
        : width(w), height(h), numberOfChannels(n), bitsPerSample(bits)
        , floatSample(flt), colorSpace(cs), owner(own)
    {
        size_t pixelsPerChannel = width * height;
        size_t bytesPerPixel = floatSample ? sizeof(float) : sizeof(uint16_t);
        size_t bytesPerChannel = pixelsPerChannel * bytesPerPixel;
        
        channelData.resize(numberOfChannels);
        for (uint32_t i = 0; i < numberOfChannels; ++i)
        {
            channelData[i] = malloc(bytesPerChannel);
            memset(channelData[i], 0, bytesPerChannel);
        }
    }
    
    ~MockImage()
    {
        for (void* ptr : channelData)
            free(ptr);
    }
};

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
    image_handle imageHandle = nullptr;   // the real PCL image
    MockImage*  imagePtr    = nullptr;    // cached pointer for convenience
  
    view_handle mainView = nullptr;
    double zoomFactor = 1.0;
    bool isVisible = true;
    
    MockImageWindow(const std::string& windowId, int w, int h, int ch, int bits, bool isFloat)
        : id(windowId), width(w), height(h), channels(ch), bitsPerSample(bits), floatSample(isFloat)
    {
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
        widget->resize(std::min(width,800), std::min(height,600));
    }
    
    ~MockImageWindow()
    {
        if (widget)
            delete widget;
    }

    void updateDisplay()
    {
	qDebug() << "*** updateDisplay() CALLED ***";
	qDebug() << "  this:" << this;
	qDebug() << "  widget:" << widget;
	qDebug() << "  imageLabel:" << imageLabel;
	qDebug() << "  imagePtr:" << imagePtr;

	if (!widget || !imageLabel || !imagePtr)
	{
	    qDebug() << "  *** EARLY RETURN - Missing component! ***";
	    return;
	}

	MockImage* img = imagePtr;
	qDebug() << "  Image info:";
	qDebug() << "    width:" << img->width << "height:" << img->height;
	qDebug() << "    channels:" << img->numberOfChannels;
	qDebug() << "    floatSample:" << img->floatSample;
	qDebug() << "    channelData.size():" << img->channelData.size();

	if (img->channelData.empty()) {
	    qDebug() << "  *** ERROR: channelData is empty! ***";
	    return;
	}

	qDebug() << "    channelData[0]:" << img->channelData[0];

	if (!img->channelData[0]) {
	    qDebug() << "  *** ERROR: channelData[0] is NULL! ***";
	    return;
	}

	// Sample some pixel values
	if (img->floatSample) {
	    float* data = (float*)img->channelData[0];
	    qDebug() << "  Sample pixels (float):";
	    qDebug() << "    [0,0] (data[0]):" << data[0];
	    qDebug() << "    [255,255] (data[255*512+255]):" << data[255 * img->width + 255];
	    qDebug() << "    [511,511] (data[last]):" << data[(img->width * img->height) - 1];
	}

	qDebug() << "  Creating QImage...";
	QImage qimg(img->width,
		    img->height,
		    img->numberOfChannels == 1
			? QImage::Format_Grayscale8
			: QImage::Format_RGB888);

	qDebug() << "  QImage created:";
	qDebug() << "    isNull:" << qimg.isNull();
	qDebug() << "    size:" << qimg.size();

	// Rest of conversion code...
	for (uint32_t y = 0; y < img->height; ++y)
	for (uint32_t x = 0; x < img->width;  ++x)
	{
	    if (img->floatSample)
	    {
		if (img->numberOfChannels == 1)
		{
		    float* data = (float*)img->channelData[0];
		    float val = data[y * img->width + x];
		    int gray = std::clamp(int(val * 255.0f), 0, 255);
		    qimg.setPixel(x, y, qRgb(gray, gray, gray));
		}
		else
		{
		    float* r = (float*)img->channelData[0];
		    float* g = (float*)img->channelData[1];
		    float* b = (float*)img->channelData[2];
		    size_t idx = y * img->width + x;

		    int rr = std::clamp(int(r[idx] * 255.0f), 0, 255);
		    int gg = std::clamp(int(g[idx] * 255.0f), 0, 255);
		    int bb = std::clamp(int(b[idx] * 255.0f), 0, 255);

		    qimg.setPixel(x, y, qRgb(rr, gg, bb));
		}
	    }
	    else
	    {
		uint16_t* r = (uint16_t*)img->channelData[0];
		uint16_t* g = (uint16_t*)img->channelData[1];
		uint16_t* b = (uint16_t*)img->channelData[2];
		size_t idx = y * img->width + x;

		if (img->numberOfChannels == 1)
		{
		    int gray = r[idx] >> 8;
		    qimg.setPixel(x, y, qRgb(gray, gray, gray));
		}
		else
		{
		    qimg.setPixel(x, y, qRgb(r[idx]>>8, g[idx]>>8, b[idx]>>8));
		}
	    }
	}

	qDebug() << "  Pixel conversion complete";

	// Sample converted QImage pixels
	qDebug() << "  QImage pixel samples:";
	qDebug() << "    (0,0):" << qGray(qimg.pixel(0, 0));
	qDebug() << "    (255,255):" << qGray(qimg.pixel(255, 255));
	qDebug() << "    (511,511):" << qGray(qimg.pixel(511, 511));

	qDebug() << "  Creating QPixmap...";
	QPixmap pix = QPixmap::fromImage(qimg);
	qDebug() << "  QPixmap created:";
	qDebug() << "    isNull:" << pix.isNull();
	qDebug() << "    size:" << pix.size();
	qDebug() << "    zoomFactor:" << zoomFactor;

	// Apply zoom scaling
	if (zoomFactor != 1.0)
	{
	    int scaledWidth = static_cast<int>(img->width * zoomFactor);
	    int scaledHeight = static_cast<int>(img->height * zoomFactor);

	    qDebug() << "  Scaling pixmap:";
	    qDebug() << "    original:" << pix.size();
	    qDebug() << "    scaled to:" << QSize(scaledWidth, scaledHeight);

	    pix = pix.scaled(scaledWidth, scaledHeight, 
				  Qt::KeepAspectRatio, 
				  Qt::SmoothTransformation);

	    qDebug() << "    final size:" << pix.size();
	}

	qDebug() << "  Setting pixmap on label...";
	qDebug() << "    Label before:";
	qDebug() << "      size:" << imageLabel->size();
	qDebug() << "      hasPixmap:" << !imageLabel->pixmap(Qt::ReturnByValue).isNull();

	imageLabel->setPixmap(pix);
	imageLabel->resize(pix.size());

	qDebug() << "    Label after:";
	qDebug() << "      size:" << imageLabel->size();
	qDebug() << "      hasPixmap:" << !imageLabel->pixmap(Qt::ReturnByValue).isNull();
	qDebug() << "      pixmap size:" << imageLabel->pixmap(Qt::ReturnByValue).size();

	qDebug() << "  *** updateDisplay() COMPLETE ***\n";
    }  

    void zoomToFit()
    {
	if (!widget || !scrollArea) return;

	int viewWidth = scrollArea->viewport()->width() - 20;
	int viewHeight = scrollArea->viewport()->height() - 20;

	float scaleX = static_cast<float>(viewWidth) / width;
	float scaleY = static_cast<float>(viewHeight) / height;
	float scale = std::min(scaleX, scaleY);

	// Store exact fractional zoom
	zoomFactor = static_cast<double>(scale);

	qDebug() << "*** zoomToFit()";
	qDebug() << "  viewport:" << viewWidth << "x" << viewHeight;
	qDebug() << "  image:" << width << "x" << height;
	qDebug() << "  scaleX:" << scaleX << "scaleY:" << scaleY;
	qDebug() << "  final zoom:" << zoomFactor;

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
