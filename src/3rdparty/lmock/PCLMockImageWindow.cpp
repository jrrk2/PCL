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
static std::map<window_handle, MockImageWindow*> g_mockWindows;
static std::map<view_handle, MockViewExtended*> g_mockViews;

// Global storage
static std::map<image_handle, MockImage*> g_images;
static std::map<std::string, window_handle> g_windowsByID;
static std::vector<view_handle> g_dynamicTargets;
static int g_nextWindowID = 1;
static SharedImageContext SharedImage;

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
    qDebug() << "*** GetImageFormat called, handle:" << handle;
    qDebug() << "  g_images.size():" << g_images.size();
    
    auto it = g_images.find(const_cast<image_handle>(handle));
    if (it == g_images.end())
    {
        qDebug() << "  ERROR: handle not found in g_images!";
        qDebug() << "  Available handles:";
        for (const auto& pair : g_images) {
            qDebug() << "    " << pair.first;
        }
        return api_false;
    }
    
    qDebug() << "  Found image:";
    qDebug() << "    bitsPerSample:" << it->second->bitsPerSample;
    qDebug() << "    floatSample:" << it->second->floatSample;
    
    if (nbits)
        *nbits = it->second->bitsPerSample;
    if (flt)
        *flt = it->second->floatSample ? api_true : api_false;
    
    qDebug() << "  Returning values: bits=" << (nbits ? *nbits : 0) 
             << " float=" << (flt ? *flt : 0);
    
    return api_true;
}

api_bool SharedImageContext::GetImageGeometry(const_image_handle handle, uint32* w, uint32* h, uint32* n)
{
    qDebug() << "*** GetImageGeometry called, handle:" << handle;
    
    auto it = g_images.find(const_cast<image_handle>(handle));
    if (it == g_images.end())
    {
        qDebug() << "  ERROR: handle not found!";
        return api_false;
    }
    
    qDebug() << "  Found image:";
    qDebug() << "    width:" << it->second->width;
    qDebug() << "    height:" << it->second->height;
    qDebug() << "    channels:" << it->second->numberOfChannels;
    
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

api_bool SharedImageContext::GetImageRGBWS(const_image_handle handle, api_RGBWS* rgbws)
{
    auto it = g_images.find(const_cast<image_handle>(handle));
    if (it == g_images.end() || !rgbws)
        return api_false;
    
    // Return default sRGB working space
    rgbws->gamma = 2.2;
    rgbws->isSRGBGamma = api_true;
    
    // sRGB chromaticity coordinates
    rgbws->x[0] = 0.6400;  // Red x
    rgbws->x[1] = 0.3000;  // Green x
    rgbws->x[2] = 0.1500;  // Blue x
    
    rgbws->y[0] = 0.3300;  // Red y
    rgbws->y[1] = 0.6000;  // Green y
    rgbws->y[2] = 0.0600;  // Blue y
    
    // Luminance coefficients (Rec. 709)
    rgbws->Y[0] = 0.2126;  // Red
    rgbws->Y[1] = 0.7152;  // Green
    rgbws->Y[2] = 0.0722;  // Blue
    
    return api_true;
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
    
    for (const auto& pair : g_mockViews)
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
    
    for (const auto& pair : g_mockViews)
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
    auto it = g_mockViews.find(const_cast<view_handle>(handle));
    if (it == g_mockViews.end())
        return nullptr;
    return it->second->windowHandle;
}

api_bool ViewContext::GetViewId(const_view_handle handle, char* id, size_type* len)
{
    auto it = g_mockViews.find(const_cast<view_handle>(handle));
    if (it == g_mockViews.end())
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
    auto it = g_mockViews.find(handle);
    if (it == g_mockViews.end() || !newId)
        return api_false;
    
    it->second->viewId = newId;
    return api_true;
}

void ViewContext::GetViewLocks(const_view_handle handle, api_bool* readLock, api_bool* writeLock)
{
    auto it = g_mockViews.find(const_cast<view_handle>(handle));
    if (it == g_mockViews.end())
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
    auto it = g_mockViews.find(handle);
    if (it == g_mockViews.end()) return;
    
    if (readLock) it->second->readLocked = true;
    if (writeLock) it->second->writeLocked = true;
}

void ViewContext::UnlockView(view_handle handle, api_bool readLock, api_bool writeLock, api_bool)
{
    auto it = g_mockViews.find(handle);
    if (it == g_mockViews.end()) return;
    
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
        auto it = g_mockViews.find(handle);
        if (it != g_mockViews.end())
            it->second->isDynamicTarget = true;
    }
}

void ViewContext::RemoveViewFromDynamicTargets(view_handle handle)
{
    auto pos = std::find(g_dynamicTargets.begin(), g_dynamicTargets.end(), handle);
    if (pos != g_dynamicTargets.end())
    {
        g_dynamicTargets.erase(pos);
        auto it = g_mockViews.find(handle);
        if (it != g_mockViews.end())
            it->second->isDynamicTarget = false;
    }
}

image_handle ViewContext::GetViewImage(view_handle handle)
{
    qDebug() << "\n*** GetViewImage called ***";
    qDebug() << "  handle:" << handle;
    qDebug() << "  g_mockViews.size():" << g_mockViews.size();
    
    // Show all view handles in the map
    if (!g_mockViews.empty())
    {
        qDebug() << "  Available view handles:";
        for (const auto& pair : g_mockViews)
        {
            qDebug() << "    " << pair.first << "-> imageHandle:" << pair.second->imageWindow->imageHandle;
        }
    }
    
    auto it = g_mockViews.find(handle);
    if (it == g_mockViews.end())
    {
        qDebug() << "  ERROR: view handle NOT FOUND in g_mockViews!";
        return nullptr;
    }
    
    qDebug() << "  Found view, imageHandle:" << it->second->imageWindow->imageHandle;
    qDebug() << "*** GetViewImage complete ***\n";
    
    return it->second->imageWindow ? it->second->imageWindow->imageHandle : nullptr;
}

/*
// CRITICAL: Returns the image handle for this view
image_handle ViewContext::GetViewImage(view_handle handle)
{
    auto it = g_mockViews.find(handle);
    if (it == g_mockViews.end())
        return nullptr;
    
    return it->second->imageHandle;
}
*/

api_bool ViewContext::IsViewColorImage(const_view_handle handle)
{
    auto it = g_mockViews.find(const_cast<view_handle>(handle));
    if (it == g_mockViews.end())
        return api_false;
    
    image_handle imgHandle = it->second->imageWindow->imageHandle;
    auto imgIt = g_images.find(imgHandle);
    if (imgIt == g_images.end())
        return api_false;
    
    return (imgIt->second->numberOfChannels >= 3) ? api_true : api_false;
}

api_bool ViewContext::GetViewDimensions(const_view_handle handle, int32* width, int32* height)
{
    auto it = g_mockViews.find(const_cast<view_handle>(handle));
    if (it == g_mockViews.end())
        return api_false;
    
    image_handle imgHandle = it->second->imageWindow->imageHandle;
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
    auto it = g_mockViews.find(const_cast<view_handle>(handle));
    if (it == g_mockViews.end() || !it->second->hasSTF)
        return api_false;
    
    MockViewExtended* view = it->second;
    image_handle imgHandle = view->imageWindow->imageHandle;
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
    auto it = g_mockViews.find(handle);
    if (it == g_mockViews.end())
        return api_false;
    
    MockViewExtended* view = it->second;
    
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
    auto it = g_mockViews.find(handle);
    if (it == g_mockViews.end())
        return api_false;
    
    it->second->hasSTF = false;
    return api_true;
}

api_bool ViewContext::GetViewScreenTransferFunctionsEnabled(view_handle handle)
{
    auto it = g_mockViews.find(handle);
    if (it == g_mockViews.end())
        return api_false;
    
    return it->second->stfEnabled ? api_true : api_false;
}

void ViewContext::SetViewScreenTransferFunctionsEnabled(view_handle handle, api_bool enabled, api_bool)
{
    auto it = g_mockViews.find(handle);
    if (it == g_mockViews.end())
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
    auto it = g_mockViews.find(const_cast<view_handle>(handle));
    if (it == g_mockViews.end() || !callback)
        return api_false;
    
    // The callback signature is: api_bool (*callback)(const char* id, uint64 type, void* data)
    for (const auto& prop : it->second->properties)
    {
        // Call the callback with property ID, type, and user data
        if (!callback(prop.first.c_str(), prop.second.type, data))
            break;
    }
    
    return api_true;
}

api_bool ViewContext::GetViewPropertyValue(api_handle, const_view_handle handle,
                                           const char* id, api_property_value* value)
{
    auto it = g_mockViews.find(const_cast<view_handle>(handle));
    if (it == g_mockViews.end() || !id || !value)
        return api_false;
    
    auto propIt = it->second->properties.find(id);
    if (propIt == it->second->properties.end())
        return api_false;
    
    *value = propIt->second.value;
    return api_true;
}

api_bool ViewContext::GetViewPropertyAttributes(api_handle, const_view_handle handle,
                                                const char* id, uint32* flags, uint64* type)
{
    auto it = g_mockViews.find(const_cast<view_handle>(handle));
    if (it == g_mockViews.end() || !id)
        return api_false;
    
    auto propIt = it->second->properties.find(id);
    if (propIt == it->second->properties.end())
        return api_false;
    
    if (type) *type = propIt->second.type;
    if (flags) *flags = 0;  // Would need to store flags separately
    
    return api_true;
}

api_bool ViewContext::SetViewPropertyValue(api_handle, view_handle handle,
                                           const char* id, const api_property_value* value,
                                           uint32 flags, api_bool)
{
    auto it = g_mockViews.find(handle);
    if (it == g_mockViews.end() || !id || !value)
        return api_false;
    
    // Store type as 0 for now (would need to extract from value)
    it->second->properties[id] = * new ViewProperty ();
    return api_true;
}

api_bool ViewContext::SetViewPropertyAttributes(api_handle, view_handle, const char*, uint32, api_bool)
{
    return api_false;  // Not storing flags separately
}

api_bool ViewContext::GetViewPropertyExists(api_handle, const_view_handle handle,
                                            const char* id, uint64* type)
{
    auto it = g_mockViews.find(const_cast<view_handle>(handle));
    if (it == g_mockViews.end() || !id)
        return api_false;
    
    auto propIt = it->second->properties.find(id);
    if (propIt == it->second->properties.end())
        return api_false;
    
    if (type) *type = propIt->second.type;
    return api_true;
}

api_bool ViewContext::DeleteViewProperty(api_handle, view_handle handle,
                                         const char* id, api_bool)
{
    auto it = g_mockViews.find(handle);
    if (it == g_mockViews.end() || !id)
        return api_false;
    
    return it->second->properties.erase(id) > 0 ? api_true : api_false;
}

api_bool ViewContext::ComputeViewProperty(api_handle, view_handle, const char*, api_bool, api_property_value*)
{
    return api_false;  // Not computing derived properties
}

window_handle ImageWindowContext::CreateImageWindow(int width, int height, int numberOfChannels, 
                                                     int bitsPerSample, uint32 floatSample, 
                                                     uint32, uint32, const char* id)
{
    std::string windowId = id ? id : ("ImageWindow_" + std::to_string(g_nextWindowID++));
    
    MockImageWindow* mockWin = new MockImageWindow(
        windowId, width, height, numberOfChannels, bitsPerSample, floatSample != 0
    );

    // CREATE THE IMAGE IN SHARED CONTEXT
    image_handle imgHandle = SharedImage.CreateImage(
	width, height, numberOfChannels, bitsPerSample,
	floatSample,              // float?
	numberOfChannels == 1 ? 0u : 1u,   // colorspace: 0 gray, 1 RGB
	mockWin                   // owner
    );

    // Link window → image
    mockWin->imageHandle = imgHandle;
    mockWin->imagePtr    = g_images[imgHandle];

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

int ImageWindowContext::GetImageWindowZoomFactor(const_window_handle handle)
{
    auto it = g_mockWindows.find(const_cast<window_handle>(handle));
    if (it == g_mockWindows.end())
        return 1;
    
    MockImageWindow* win = it->second;
    double zoom = win->zoomFactor;
    
    // Convert our double zoom to PCL's integer format
    if (zoom >= 1.0)
    {
        // Zoom in: 1.0 → 1, 2.0 → 2, 3.5 → 3
        return static_cast<int>(std::round(zoom));
    }
    else if (zoom > 0.0)
    {
        // Zoom out: 0.5 → -1 (1:2), 0.25 → -2 (1:4), 0.125 → -3 (1:8)
        double ratio = 1.0 / zoom;
        return -static_cast<int>(std::round(ratio - 1));
    }
    else
    {
        return 1;  // Default
    }
}

void ImageWindowContext::SetImageWindowZoomFactor(window_handle handle, int zoom)
{
    qDebug() << "*** SetImageWindowZoomFactor called";
    qDebug() << "  handle:" << handle;
    qDebug() << "  PCL zoom factor:" << zoom;
    
    auto it = g_mockWindows.find(handle);
    if (it == g_mockWindows.end())
        return;
    
    MockImageWindow* win = it->second;
    
    // Convert PCL's integer zoom to our double zoom factor
    if (zoom >= 1)
    {
        // Zoom in: 1 → 1.0x, 2 → 2.0x, 3 → 3.0x
        win->zoomFactor = static_cast<double>(zoom);
    }
    else if (zoom < 0)
    {
        // Zoom out: -1 → 0.5x (1:2), -2 → 0.25x (1:4), -3 → 0.125x (1:8)
        win->zoomFactor = 1.0 / static_cast<double>(-zoom + 1);
    }
    else  // zoom == 0
    {
        // PCL shouldn't send 0, but treat as 1:1
        win->zoomFactor = 1.0;
    }
    
    qDebug() << "  Internal zoom factor set to:" << win->zoomFactor;
    
    win->updateDisplay();
}

void ImageWindowContext::SetImageWindowVisible(window_handle handle, api_bool visible)
{
    auto it = g_mockWindows.find(handle);
    if (it == g_mockWindows.end()) return;
    
    MockImageWindow* win = it->second;
    if (win->widget)
    {
        if (visible)
	  {
	    win->updateDisplay();
            win->widget->show();
	  }
        else
	  {
            win->widget->hide();
	  }
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

void ImageWindowContext::GetPreviewRect(const_window_handle handle, 
                                           const char* previewId,
                                           int32* x0, int32* y0, 
                                           int32* x1, int32* y1)
{
    qDebug() << "*** GetPreviewRect called";
    qDebug() << "  handle:" << handle;
    qDebug() << "  previewId:" << (previewId ? previewId : "NULL");
    
    auto it = g_mockWindows.find(const_cast<window_handle>(handle));
    if (it == g_mockWindows.end())
    {
        qDebug() << "  ERROR: window not found";
        return;
    }
    
    // For now, return no preview rect (preview system not implemented)
    // Real implementation would look up preview rectangles by ID
    if (x0) *x0 = 0;
    if (y0) *y0 = 0;
    if (x1) *x1 = 0;
    if (y1) *y1 = 0;
    
    qDebug() << "  No preview rect (not implemented)";
    return ;
}

api_bool ImageWindowContext::CloseImageWindow(window_handle handle, api_bool flags)
{
    qDebug() << "*** CloseImageWindow called";
    qDebug() << "  handle:" << handle;
    qDebug() << "  flags:" << flags;
    
    auto it = g_mockWindows.find(handle);
    if (it == g_mockWindows.end())
    {
        qDebug() << "  ERROR: window not found";
        return api_false;
    }
    
    MockImageWindow* win = it->second;
    
    // Hide and close the window
    if (win->widget)
    {
        win->widget->close();
        win->widget->deleteLater();
    }
    
    // Remove from tracking
    g_mockWindows.erase(it);
    
    qDebug() << "  Window closed and removed";
    qDebug() << "  Remaining windows:" << g_mockWindows.size();
    return api_true;
}

void ImageWindowContext::GetImageWindowRGBWS(const_window_handle handle, api_RGBWS* rgbws)
{
    qDebug() << "*** GetImageWindowRGBWS (window) called";
    qDebug() << "  handle:" << handle;
    
    auto it = g_mockWindows.find(const_cast<window_handle>(handle));
    if (it == g_mockWindows.end() || !rgbws)
    {
        qDebug() << "  ERROR: window not found or rgbws is NULL";
        return ;
    }
    
    MockImageWindow* win = it->second;
    /*    
    // Get the image's RGBWS by calling the image version
    if (win->imageHandle)
    {
        GetImageWindowRGBWS(win->imageHandle, rgbws);
	return;
    }
    */
    // Fallback: return default sRGB working space
    rgbws->gamma = 2.2;
    rgbws->isSRGBGamma = api_true;
    rgbws->x[0] = 0.6400; rgbws->x[1] = 0.3000; rgbws->x[2] = 0.1500;
    rgbws->y[0] = 0.3300; rgbws->y[1] = 0.6000; rgbws->y[2] = 0.0600;
    rgbws->Y[0] = 0.2126; rgbws->Y[1] = 0.7152; rgbws->Y[2] = 0.0722;
    
    qDebug() << "  Returned default sRGB";
    return ;
}

view_handle ImageWindowContext::GetImageWindowCurrentView(const_window_handle handle)
{
    qDebug() << "*** GetImageWindowCurrentView called";
    qDebug() << "  handle:" << handle;
    
    auto it = g_mockWindows.find(const_cast<window_handle>(handle));
    if (it == g_mockWindows.end())
    {
        qDebug() << "  ERROR: window not found";
        return nullptr;
    }
    
    MockImageWindow* win = it->second;
    
    qDebug() << "  Current view:" << win->mainView;
    return win->mainView;
}

void ImageWindowContext::SetImageWindowCurrentView(window_handle handle, view_handle view)
{
    qDebug() << "*** SetImageWindowCurrentView called";
    qDebug() << "  handle:" << handle;
    qDebug() << "  view:" << view;
    
    auto it = g_mockWindows.find(handle);
    if (it == g_mockWindows.end())
    {
        qDebug() << "  ERROR: window not found";
        return;
    }
    
    MockImageWindow* win = it->second;
    
    // Verify the view exists
    auto viewIt = g_mockViews.find(view);
    if (viewIt == g_mockViews.end())
    {
        qDebug() << "  ERROR: view not found";
        return;
    }
    
    // Set as current view (though we only have main view for now)
    win->mainView = view;
    
    qDebug() << "  Current view set";
}

void ImageWindowContext::GetImageWindowSampleFormat(const_window_handle handle,
                                                       uint32* bitsPerSample,
                                                       uint32* floatSample)
{
    qDebug() << "*** GetImageWindowSampleFormat called";
    qDebug() << "  handle:" << handle;
    
    auto it = g_mockWindows.find(const_cast<window_handle>(handle));
    if (it == g_mockWindows.end())
    {
        qDebug() << "  ERROR: window not found";
        return ;
    }
    
    MockImageWindow* win = it->second;
    
    if (bitsPerSample)
        *bitsPerSample = win->bitsPerSample;
    
    if (floatSample)
        *floatSample = win->floatSample ? 1 : 0;
    
    qDebug() << "  bitsPerSample:" << win->bitsPerSample;
    qDebug() << "  floatSample:" << win->floatSample;
    
    return ;
}

void ImageWindowContext::SetImageWindowSampleFormat(window_handle handle,
                                                    uint32 bitsPerSample,
                                                    uint32 floatSample)
{
    qDebug() << "*** SetImageWindowSampleFormat called";
    qDebug() << "  handle:" << handle;
    qDebug() << "  bitsPerSample:" << bitsPerSample;
    qDebug() << "  floatSample:" << floatSample;
    
    auto it = g_mockWindows.find(handle);
    if (it == g_mockWindows.end())
    {
        qDebug() << "  ERROR: window not found";
        return;
    }
    
    MockImageWindow* win = it->second;
    
    win->bitsPerSample = bitsPerSample;
    win->floatSample = (floatSample != 0);
    
    // Note: This doesn't convert existing pixel data, just changes the format flag
    // Real implementation would need to convert the actual image data
    
    qDebug() << "  Sample format updated";
    qDebug() << "  WARNING: Pixel data not converted - format change is metadata only";
}

