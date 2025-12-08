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

// Complete_API_Implementation.cpp
// Strict implementation following the exact API prototypes
// Add to PCLMockAPI.cpp

#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
// ViewContext_Complete_Implementation.cpp
// Full ViewContext implementation with load/save support
// Add to PCLMockAPI.cpp

#include <map>
#include <vector>
#include <string>
#include <algorithm>

// ============================================================================
// View Property Storage
// ============================================================================

struct ViewProperty
{
    std::string id;
    uint64_t type;          // Property type (Variant::data_type)
    uint32_t flags;         // Property flags
    api_property_value value;
    
    ViewProperty() : type(0), flags(0)
    {
        memset(&value, 0, sizeof(api_property_value));
    }
};

// Extend MockView to include properties and state
struct MockViewExtended
{
    window_handle windowHandle;
    std::string viewId;
    MockImageWindow* imageWindow;
    
    // View state
    bool readLocked = false;
    bool writeLocked = false;
    bool isDynamicTarget = false;
    
    // Screen transfer functions (STF)
    bool hasSTF = false;
    bool stfEnabled = false;
    double stf_m[3] = {0.5, 0.5, 0.5};   // Midtones
    double stf_c0[3] = {0, 0, 0};        // Shadows
    double stf_c1[3] = {1, 1, 1};        // Highlights
    double stf_r0[3] = {0, 0, 0};        // Range low
    double stf_r1[3] = {1, 1, 1};        // Range high
    
    // Properties storage
    std::map<std::string, ViewProperty> properties;
    
    MockViewExtended(window_handle win, const std::string& id, MockImageWindow* imgWin)
        : windowHandle(win), viewId(id), imageWindow(imgWin)
    {
    }
};

// Global storage - replace the simple MockView with MockViewExtended
static std::map<view_handle, MockViewExtended*> g_mockViewsExt;

// ============================================================================
// Mock Image Storage (replaces previous MockImageWindow)
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

struct MockView
{
    std::string viewId;
    window_handle windowHandle;
    image_handle imageHandle;
    
    // View state
    bool readLocked = false;
    bool writeLocked = false;
    bool isDynamicTarget = false;
    
    // STF
    bool hasSTF = false;
    bool stfEnabled = false;
    double stf_m[3] = {0.5, 0.5, 0.5};
    double stf_c0[3] = {0, 0, 0};
    double stf_c1[3] = {1, 1, 1};
    double stf_r0[3] = {0, 0, 0};
    double stf_r1[3] = {1, 1, 1};
    
    // Properties
    std::map<std::string, std::pair<uint64_t, api_property_value>> properties;
    
    MockView(const std::string& id, window_handle win, image_handle img)
        : viewId(id), windowHandle(win), imageHandle(img) {}
};

struct MockWindow
{
    std::string windowId;
    QWidget* widget = nullptr;
    QLabel* imageLabel = nullptr;
    QScrollArea* scrollArea = nullptr;
    
    view_handle mainView = nullptr;
    view_handle currentView = nullptr;
    
    // File info
    std::string filePath;
    std::string fileURL;
    bool isNew = true;
    bool isCopy = false;
    size_t modifyCount = 0;
    
    int zoomFactor = 1;
    bool isVisible = true;
    
    MockWindow(const std::string& id) : windowId(id)
    {
        widget = new QWidget();
        widget->setWindowTitle(QString::fromStdString(id));
        
        QVBoxLayout* layout = new QVBoxLayout(widget);
        scrollArea = new QScrollArea(widget);
        imageLabel = new QLabel();
        imageLabel->setScaledContents(false);
        scrollArea->setWidget(imageLabel);
        scrollArea->setWidgetResizable(false);
        layout->addWidget(scrollArea);
    }
    
    ~MockWindow()
    {
        if (widget)
            delete widget;
    }
    
    void updateDisplay(MockImage* img)
    {
        if (!img || !imageLabel) return;
        
        // Convert image data to QImage for display
        QImage qimg(img->width, img->height, 
                   img->numberOfChannels == 1 ? QImage::Format_Grayscale8 : QImage::Format_RGB888);
        
        for (uint32_t y = 0; y < img->height; ++y)
        {
            for (uint32_t x = 0; x < img->width; ++x)
            {
                if (img->floatSample)
                {
                    if (img->numberOfChannels == 1)
                    {
                        float* data = (float*)img->channelData[0];
                        float val = data[y * img->width + x];
                        int gray = (int)(val * 255.0f);
                        gray = std::max(0, std::min(255, gray));
                        qimg.setPixel(x, y, qRgb(gray, gray, gray));
                    }
                    else if (img->numberOfChannels >= 3)
                    {
                        float* rData = (float*)img->channelData[0];
                        float* gData = (float*)img->channelData[1];
                        float* bData = (float*)img->channelData[2];
                        size_t idx = y * img->width + x;
                        int r = (int)(rData[idx] * 255.0f);
                        int g = (int)(gData[idx] * 255.0f);
                        int b = (int)(bData[idx] * 255.0f);
                        r = std::max(0, std::min(255, r));
                        g = std::max(0, std::min(255, g));
                        b = std::max(0, std::min(255, b));
                        qimg.setPixel(x, y, qRgb(r, g, b));
                    }
                }
                else
                {
                    if (img->numberOfChannels == 1)
                    {
                        uint16_t* data = (uint16_t*)img->channelData[0];
                        int gray = data[y * img->width + x] >> 8;
                        qimg.setPixel(x, y, qRgb(gray, gray, gray));
                    }
                    else if (img->numberOfChannels >= 3)
                    {
                        uint16_t* rData = (uint16_t*)img->channelData[0];
                        uint16_t* gData = (uint16_t*)img->channelData[1];
                        uint16_t* bData = (uint16_t*)img->channelData[2];
                        size_t idx = y * img->width + x;
                        int r = rData[idx] >> 8;
                        int g = gData[idx] >> 8;
                        int b = bData[idx] >> 8;
                        qimg.setPixel(x, y, qRgb(r, g, b));
                    }
                }
            }
        }
        
        QPixmap pixmap = QPixmap::fromImage(qimg);
        if (zoomFactor != 1)
        {
            pixmap = pixmap.scaled(img->width * zoomFactor, img->height * zoomFactor,
                                  Qt::KeepAspectRatio, Qt::FastTransformation);
        }
        
        imageLabel->setPixmap(pixmap);
        imageLabel->resize(pixmap.size());
    }
};

// Global storage
static std::map<image_handle, MockImage*> g_images;
static std::map<view_handle, MockView*> g_views;
static std::map<window_handle, MockWindow*> g_windows;
static std::map<std::string, window_handle> g_windowsByID;
static std::vector<view_handle> g_dynamicTargets;
static int g_nextWindowID = 1;

// ============================================================================
// SharedImageContext Implementation (CRITICAL for pixel access)
// ============================================================================

image_handle SharedImageContext::CreateImage(uint32 w, uint32 h, uint32 n, 
                                             uint32 nbits, api_bool flt, uint32 cs, void* owner)
{
    MockImage* img = new MockImage(w, h, n, nbits, flt != 0, cs, owner);
    image_handle handle = reinterpret_cast<image_handle>(img);
    g_images[handle] = img;
    
    LogDbg("SharedImageContext::CreateImage: " + std::to_string(w) + "x" + std::to_string(h) + 
           "x" + std::to_string(n) + " bits=" + std::to_string(nbits) + 
           " float=" + std::to_string(flt));
    
    return handle;
}

void* SharedImageContext::GetImageOwner(const_image_handle handle)
{
    auto it = g_images.find(const_cast<image_handle>(handle));
    if (it == g_images.end())
        return nullptr;
    return it->second->owner;
}

api_bool SharedImageContext::GetImageRefCount(const_image_handle handle, uint32* count)
{
    auto it = g_images.find(const_cast<image_handle>(handle));
    if (it == g_images.end())
        return api_false;
    
    if (count)
        *count = it->second->refCount;
    return api_true;
}

api_bool SharedImageContext::IsValidImageHandle(const_image_handle handle)
{
    return g_images.find(const_cast<image_handle>(handle)) != g_images.end() 
        ? api_true : api_false;
}

api_bool SharedImageContext::AttachToImage(image_handle handle, void* owner)
{
    auto it = g_images.find(handle);
    if (it == g_images.end())
        return api_false;
    
    it->second->refCount++;
    return api_true;
}

api_bool SharedImageContext::DetachFromImage(image_handle handle, void* owner)
{
    auto it = g_images.find(handle);
    if (it == g_images.end())
        return api_false;
    
    if (it->second->refCount > 0)
        it->second->refCount--;
    
    if (it->second->refCount == 0)
    {
        delete it->second;
        g_images.erase(it);
    }
    
    return api_true;
}

api_bool SharedImageContext::GetImageFormat(const_image_handle handle, uint32* nbits, api_bool* flt)
{
    auto it = g_images.find(const_cast<image_handle>(handle));
    if (it == g_images.end())
        return api_false;
    
    if (nbits)
        *nbits = it->second->bitsPerSample;
    if (flt)
        *flt = it->second->floatSample ? api_true : api_false;
    
    return api_true;
}

api_bool SharedImageContext::GetImageGeometry(const_image_handle handle, uint32* w, uint32* h, uint32* n)
{
    auto it = g_images.find(const_cast<image_handle>(handle));
    if (it == g_images.end())
        return api_false;
    
    if (w) *w = it->second->width;
    if (h) *h = it->second->height;
    if (n) *n = it->second->numberOfChannels;
    
    return api_true;
}

api_bool SharedImageContext::SetImageGeometry(image_handle handle, uint32 w, uint32 h, uint32 n)
{
    auto it = g_images.find(handle);
    if (it == g_images.end())
        return api_false;
    
    // Would need to reallocate - for now, not supported
    return api_false;
}

api_bool SharedImageContext::GetImageColorSpace(const_image_handle handle, uint32* cs)
{
    auto it = g_images.find(const_cast<image_handle>(handle));
    if (it == g_images.end())
        return api_false;
    
    if (cs)
        *cs = it->second->colorSpace;
    return api_true;
}

api_bool SharedImageContext::SetImageColorSpace(image_handle handle, uint32 cs)
{
    auto it = g_images.find(handle);
    if (it == g_images.end())
        return api_false;
    
    it->second->colorSpace = cs;
    return api_true;
}

api_bool SharedImageContext::GetImageRGBWS(const_image_handle, api_RGBWS*)
{
    return api_false;  // Not implemented
}

api_bool SharedImageContext::SetImageRGBWS(image_handle, const api_RGBWS*)
{
    return api_false;  // Must be set through ImageWindow
}

// CRITICAL: Returns array of pointers to channel data
api_bool SharedImageContext::GetImagePixelData(image_handle handle, void*** data)
{
    auto it = g_images.find(handle);
    if (it == g_images.end() || !data)
        return api_false;
    
    *data = it->second->channelData.data();
    return api_true;
}

api_bool SharedImageContext::SetImagePixelData(image_handle handle, void** data)
{
    auto it = g_images.find(handle);
    if (it == g_images.end() || !data)
        return api_false;
    
    // Replace pointers (takes ownership)
    for (size_t i = 0; i < it->second->channelData.size(); ++i)
    {
        if (it->second->channelData[i])
            free(it->second->channelData[i]);
        it->second->channelData[i] = data[i];
    }
    
    return api_true;
}

// ============================================================================
// ViewContext Implementation
// ============================================================================

view_handle ViewContext::GetViewById(const char* fullId)
{
    if (!fullId) return nullptr;
    
    for (const auto& pair : g_views)
    {
        if (pair.second->viewId == fullId)
            return pair.first;
    }
    return nullptr;
}

void ViewContext::EnumerateViews(pcl::view_enumeration_callback callback, void* data,
                                 api_bool includeMainViews, api_bool includePreviews)
{
    if (!callback) return;
    
    for (const auto& pair : g_views)
    {
        if (includeMainViews)
        {
            if (!callback(pair.first, data))
                break;
        }
    }
}

api_bool ViewContext::IsPreview(const_view_handle) { return api_false; }
api_bool ViewContext::IsVolatilePreview(const_view_handle) { return api_false; }
api_bool ViewContext::IsStoredPreview(const_view_handle) { return api_false; }

window_handle ViewContext::GetViewParentWindow(const_view_handle handle)
{
    auto it = g_views.find(const_cast<view_handle>(handle));
    if (it == g_views.end())
        return nullptr;
    return it->second->windowHandle;
}

api_bool ViewContext::GetViewId(const_view_handle handle, char* id, size_type* len)
{
    auto it = g_views.find(const_cast<view_handle>(handle));
    if (it == g_views.end())
        return api_false;
    
    const std::string& viewId = it->second->viewId;
    
    if (id == nullptr)
    {
        if (len) *len = viewId.length() + 1;
        return api_true;
    }
    
    if (len && *len > 0)
    {
        size_t copyLen = std::min(static_cast<size_t>(*len - 1), viewId.length());
        std::memcpy(id, viewId.c_str(), copyLen);
        id[copyLen] = '\0';
        *len = viewId.length() + 1;
    }
    
    return api_true;
}

api_bool ViewContext::GetViewFullId(const_view_handle handle, char* fullId, size_type* len)
{
    return GetViewId(handle, fullId, len);
}

api_bool ViewContext::SetViewId(view_handle handle, const char* newId)
{
    auto it = g_views.find(handle);
    if (it == g_views.end() || !newId)
        return api_false;
    
    it->second->viewId = newId;
    return api_true;
}

void ViewContext::GetViewLocks(const_view_handle handle, api_bool* readLock, api_bool* writeLock)
{
    auto it = g_views.find(const_cast<view_handle>(handle));
    if (it == g_views.end())
    {
        if (readLock) *readLock = api_false;
        if (writeLock) *writeLock = api_false;
        return;
    }
    
    if (readLock) *readLock = it->second->readLocked ? api_true : api_false;
    if (writeLock) *writeLock = it->second->writeLocked ? api_true : api_false;
}

void ViewContext::LockView(view_handle handle, api_bool readLock, api_bool writeLock, api_bool)
{
    auto it = g_views.find(handle);
    if (it == g_views.end()) return;
    
    if (readLock) it->second->readLocked = true;
    if (writeLock) it->second->writeLocked = true;
}

void ViewContext::UnlockView(view_handle handle, api_bool readLock, api_bool writeLock, api_bool)
{
    auto it = g_views.find(handle);
    if (it == g_views.end()) return;
    
    if (readLock) it->second->readLocked = false;
    if (writeLock) it->second->writeLocked = false;
}

api_bool ViewContext::IsViewDynamicTarget(const_view_handle handle)
{
    view_handle h = const_cast<view_handle>(handle);
    return std::find(g_dynamicTargets.begin(), g_dynamicTargets.end(), h) != g_dynamicTargets.end()
        ? api_true : api_false;
}

void ViewContext::AddViewToDynamicTargets(view_handle handle)
{
    if (!IsViewDynamicTarget(handle))
    {
        g_dynamicTargets.push_back(handle);
        auto it = g_views.find(handle);
        if (it != g_views.end())
            it->second->isDynamicTarget = true;
    }
}

void ViewContext::RemoveViewFromDynamicTargets(view_handle handle)
{
    auto pos = std::find(g_dynamicTargets.begin(), g_dynamicTargets.end(), handle);
    if (pos != g_dynamicTargets.end())
    {
        g_dynamicTargets.erase(pos);
        auto it = g_views.find(handle);
        if (it != g_views.end())
            it->second->isDynamicTarget = false;
    }
}

// CRITICAL: Returns the image handle for this view
image_handle ViewContext::GetViewImage(view_handle handle)
{
    auto it = g_views.find(handle);
    if (it == g_views.end())
        return nullptr;
    
    return it->second->imageHandle;
}

api_bool ViewContext::IsViewColorImage(const_view_handle handle)
{
    auto it = g_views.find(const_cast<view_handle>(handle));
    if (it == g_views.end())
        return api_false;
    
    image_handle imgHandle = it->second->imageHandle;
    auto imgIt = g_images.find(imgHandle);
    if (imgIt == g_images.end())
        return api_false;
    
    return (imgIt->second->numberOfChannels >= 3) ? api_true : api_false;
}

api_bool ViewContext::GetViewDimensions(const_view_handle handle, int32* width, int32* height)
{
    auto it = g_views.find(const_cast<view_handle>(handle));
    if (it == g_views.end())
        return api_false;
    
    image_handle imgHandle = it->second->imageHandle;
    auto imgIt = g_images.find(imgHandle);
    if (imgIt == g_images.end())
        return api_false;
    
    if (width) *width = imgIt->second->width;
    if (height) *height = imgIt->second->height;
    
    return api_true;
}

// STF functions - storing per view
api_bool ViewContext::GetViewScreenTransferFunctions(const_view_handle handle,
                                                     double* m, double* c0, double* c1, double* r0, double* r1)
{
    auto it = g_views.find(const_cast<view_handle>(handle));
    if (it == g_views.end() || !it->second->hasSTF)
        return api_false;
    
    MockView* view = it->second;
    image_handle imgHandle = view->imageHandle;
    auto imgIt = g_images.find(imgHandle);
    int channels = (imgIt != g_images.end()) ? imgIt->second->numberOfChannels : 1;
    
    for (int i = 0; i < std::min(channels, 3); ++i)
    {
        if (m) m[i] = view->stf_m[i];
        if (c0) c0[i] = view->stf_c0[i];
        if (c1) c1[i] = view->stf_c1[i];
        if (r0) r0[i] = view->stf_r0[i];
        if (r1) r1[i] = view->stf_r1[i];
    }
    
    return api_true;
}

api_bool ViewContext::SetViewScreenTransferFunctions(view_handle handle,
                                                     const double* m, const double* c0, const double* c1,
                                                     const double* r0, const double* r1, api_bool)
{
    auto it = g_views.find(handle);
    if (it == g_views.end())
        return api_false;
    
    MockView* view = it->second;
    
    for (int i = 0; i < 3; ++i)
    {
        if (m) view->stf_m[i] = m[i];
        if (c0) view->stf_c0[i] = c0[i];
        if (c1) view->stf_c1[i] = c1[i];
        if (r0) view->stf_r0[i] = r0[i];
        if (r1) view->stf_r1[i] = r1[i];
    }
    
    view->hasSTF = true;
    return api_true;
}

api_bool ViewContext::DestroyViewScreenTransferFunctions(view_handle handle, api_bool)
{
    auto it = g_views.find(handle);
    if (it == g_views.end())
        return api_false;
    
    it->second->hasSTF = false;
    return api_true;
}

api_bool ViewContext::GetViewScreenTransferFunctionsEnabled(view_handle handle)
{
    auto it = g_views.find(handle);
    if (it == g_views.end())
        return api_false;
    
    return it->second->stfEnabled ? api_true : api_false;
}

void ViewContext::SetViewScreenTransferFunctionsEnabled(view_handle handle, api_bool enabled, api_bool)
{
    auto it = g_views.find(handle);
    if (it == g_views.end())
        return;
    
    it->second->stfEnabled = (enabled != 0);
}

// Property functions
api_bool ViewContext::IsReservedViewPropertyId(const char* id)
{
    // List of reserved property IDs
    static const char* reserved[] = {
        "PCL:CFASourceFilePath", "PCL:CFASourcePattern",
        "Observation:Time:Start", "Observation:Time:End",
        nullptr
    };
    
    if (!id) return api_false;
    
    for (int i = 0; reserved[i] != nullptr; ++i)
    {
        if (std::strcmp(id, reserved[i]) == 0)
            return api_true;
    }
    
    return api_false;
}

api_bool ViewContext::EnumerateViewProperties(const_view_handle handle,
                                              pcl::property_enumeration_callback callback,
                                              char* id, size_type* len, void* data)
{
    auto it = g_views.find(const_cast<view_handle>(handle));
    if (it == g_views.end() || !callback)
        return api_false;
    
    // The callback signature is: api_bool (*callback)(const char* id, uint64 type, void* data)
    for (const auto& prop : it->second->properties)
    {
        // Call the callback with property ID, type, and user data
        if (!callback(prop.first.c_str(), prop.second.first, data))
            break;
    }
    
    return api_true;
}

api_bool ViewContext::GetViewPropertyValue(api_handle, const_view_handle handle,
                                           const char* id, api_property_value* value)
{
    auto it = g_views.find(const_cast<view_handle>(handle));
    if (it == g_views.end() || !id || !value)
        return api_false;
    
    auto propIt = it->second->properties.find(id);
    if (propIt == it->second->properties.end())
        return api_false;
    
    *value = propIt->second.second;
    return api_true;
}

api_bool ViewContext::GetViewPropertyAttributes(api_handle, const_view_handle handle,
                                                const char* id, uint32* flags, uint64* type)
{
    auto it = g_views.find(const_cast<view_handle>(handle));
    if (it == g_views.end() || !id)
        return api_false;
    
    auto propIt = it->second->properties.find(id);
    if (propIt == it->second->properties.end())
        return api_false;
    
    if (type) *type = propIt->second.first;
    if (flags) *flags = 0;  // Would need to store flags separately
    
    return api_true;
}

api_bool ViewContext::SetViewPropertyValue(api_handle, view_handle handle,
                                           const char* id, const api_property_value* value,
                                           uint32 flags, api_bool)
{
    auto it = g_views.find(handle);
    if (it == g_views.end() || !id || !value)
        return api_false;
    
    // Store type as 0 for now (would need to extract from value)
    it->second->properties[id] = std::make_pair(0, *value);
    return api_true;
}

api_bool ViewContext::SetViewPropertyAttributes(api_handle, view_handle, const char*, uint32, api_bool)
{
    return api_false;  // Not storing flags separately
}

api_bool ViewContext::GetViewPropertyExists(api_handle, const_view_handle handle,
                                            const char* id, uint64* type)
{
    auto it = g_views.find(const_cast<view_handle>(handle));
    if (it == g_views.end() || !id)
        return api_false;
    
    auto propIt = it->second->properties.find(id);
    if (propIt == it->second->properties.end())
        return api_false;
    
    if (type) *type = propIt->second.first;
    return api_true;
}

api_bool ViewContext::DeleteViewProperty(api_handle, view_handle handle,
                                         const char* id, api_bool)
{
    auto it = g_views.find(handle);
    if (it == g_views.end() || !id)
        return api_false;
    
    return it->second->properties.erase(id) > 0 ? api_true : api_false;
}

api_bool ViewContext::ComputeViewProperty(api_handle, view_handle, const char*, api_bool, api_property_value*)
{
    return api_false;  // Not computing derived properties
}


// Global storage for mock windows
static std::map<window_handle, MockImageWindow*> g_mockWindows;
static std::map<view_handle, MockViewExtended*> g_mockViews;

window_handle ImageWindowContext::CreateImageWindow(int width, int height, int numberOfChannels, 
                                                     int bitsPerSample, uint32 floatSample, 
                                                     uint32, uint32, const char* id)
{
    std::string windowId = id ? id : ("ImageWindow_" + std::to_string(g_nextWindowID++));
    
    MockImageWindow* mockWin = new MockImageWindow(
        windowId, width, height, numberOfChannels, bitsPerSample, floatSample != 0
    );
    
    window_handle handle = reinterpret_cast<window_handle>(mockWin->widget);
    g_mockWindows[handle] = mockWin;
    g_windowsByID[windowId] = handle;
    
    // Create a mock view for the main view
    //    MockViewExtended(window_handle win, const std::string& id, MockImageWindow* imgWin)
    MockViewExtended* mainView = new MockViewExtended(handle, windowId + "_main", mockWin);
    view_handle viewHandle = reinterpret_cast<view_handle>(mainView);
    g_mockViews[viewHandle] = mainView;
    mockWin->mainView = viewHandle;
    
    return handle;
}

window_handle ImageWindowContext::GetActiveImageWindow()
{
    // Return the first window if any exist
    if (!g_mockWindows.empty())
        return g_mockWindows.begin()->first;
    
    return nullptr;
}


void ImageWindowContext::ZoomImageWindowToFit(window_handle handle, api_bool, api_bool, api_bool, api_bool)
{
    auto it = g_mockWindows.find(handle);
    if (it == g_mockWindows.end()) return;
    
    MockImageWindow* win = it->second;
    win->zoomToFit();
}

void ImageWindowContext::SetImageWindowVisible(window_handle handle, api_bool visible)
{
    auto it = g_mockWindows.find(handle);
    if (it == g_mockWindows.end()) return;
    
    MockImageWindow* win = it->second;
    if (win->widget)
    {
        if (visible)
            win->widget->show();
        else
            win->widget->hide();
        win->isVisible = (visible != 0);
    }
}


view_handle ImageWindowContext::GetImageWindowMainView(const_window_handle handle)
{
    auto it = g_mockWindows.find(const_cast<window_handle>(handle));
    if (it == g_mockWindows.end()) return nullptr;
    
    return it->second->mainView;
}

void ImageWindowContext::EnumerateImageWindows(pcl::window_enumeration_callback callback, void* data, api_bool)
{
    if (!callback) return;
    
    for (const auto& pair : g_mockWindows)
    {
        if (!callback(pair.first, data))
            break;
    }
}

window_handle ImageWindowContext::GetImageWindowById(const char* id)
{
    if (!id) return nullptr;
    
    auto it = g_windowsByID.find(id);
    if (it != g_windowsByID.end())
        return it->second;
    
    return nullptr;
}

// Continued in next part...

/*
TO BE CONTINUED: ImageWindowContext implementation follows same pattern.
Key insight: 
- CreateImageWindow() creates both a MockWindow AND calls SharedImageContext::CreateImage()
- MainView gets the image_handle from the created image
- GenericImage::PixelAddress() ultimately calls SharedImageContext::GetImagePixelData()
  to get the array of channel pointers, then indexes into the appropriate channel
*/
