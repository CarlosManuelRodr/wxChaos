// ReSharper disable CppFunctionResultShouldBeUsed
#include "export/NativeVideoWriter.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <sstream>
#include <utility>

#ifdef _WIN32
#include <windows.h>
#include <codecapi.h>
#include <icodecapi.h>
#include <mfapi.h>
#include <mferror.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#elif defined(__linux__)
#include <gst/app/gstappsrc.h>
#include <gst/gst.h>
#endif

class NativeVideoWriter::Impl
{
    friend class NativeVideoWriter;

    std::string _error;
    unsigned int _sourceWidth{};
    unsigned int _sourceHeight{};
    unsigned int _width{};
    unsigned int _height{};
    unsigned int _fps{};
    uint64_t _frameIndex{};

    static unsigned int MakeEvenDimension(unsigned int value);
    static unsigned int CalculateBitRate(unsigned int width, unsigned int height, unsigned int fps);

#ifdef _WIN32
    IMFSinkWriter* _writer{};
    DWORD _streamIndex{};
    bool _mediaFoundationStarted{};
    bool _comInitialized{};

    bool SetError(const char* action, HRESULT result);
    void ReleaseResources();
    static std::wstring Utf8ToWide(const std::string& value);
    static std::string HResultToString(const char* action, HRESULT result);

    template<class T>
    static void SafeRelease(T*& value)
    {
        if (value != nullptr)
        {
            value->Release();
            value = nullptr;
        }
    }
#elif defined(__linux__)
    GstElement* _pipeline{};
    GstElement* _appSource{};

    bool SetError(std::string error);
    void ReleaseResources();
#else
    bool SetError(std::string error);
#endif

public:
    bool Open(const std::string& outputPath, unsigned int width, unsigned int height, unsigned int fps,
              const NativeVideoEncodingOptions& options);
    bool WriteFrame(const sf::Image& frame);
    bool Close();
    [[nodiscard]] std::string GetError() const { return _error; }
};

unsigned int NativeVideoWriter::Impl::MakeEvenDimension(const unsigned int value)
{
    return std::max(2U, value - value % 2U);
}

unsigned int NativeVideoWriter::Impl::CalculateBitRate(const unsigned int width, const unsigned int height,
                                                       const unsigned int fps)
{
    const uint64_t scaledBitRate = static_cast<uint64_t>(width) * height * fps / 2U;
    return static_cast<unsigned int>(std::clamp<uint64_t>(
        scaledBitRate, 1000000U, std::numeric_limits<unsigned int>::max()));
}

#ifdef _WIN32
std::wstring NativeVideoWriter::Impl::Utf8ToWide(const std::string& value)
{
    if (value.empty())
        return {};

    const int size = MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, nullptr, 0);
    if (size <= 0)
        return {};

    std::wstring wide(static_cast<size_t>(size - 1), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, wide.data(), size);
    return wide;
}

std::string NativeVideoWriter::Impl::HResultToString(const char* action, const HRESULT result)
{
    std::ostringstream stream;
    stream << action << " failed with HRESULT 0x" << std::hex << static_cast<unsigned long>(result);
    return stream.str();
}

bool NativeVideoWriter::Impl::SetError(const char* action, const HRESULT result)
{
    _error = HResultToString(action, result);
    return false;
}

void NativeVideoWriter::Impl::ReleaseResources()
{
    SafeRelease(_writer);
    if (_mediaFoundationStarted)
    {
        MFShutdown();
        _mediaFoundationStarted = false;
    }
    if (_comInitialized)
    {
        CoUninitialize();
        _comInitialized = false;
    }
}

bool NativeVideoWriter::Impl::Open(const std::string& outputPath, const unsigned int width, const unsigned int height,
                                  const unsigned int fps, const NativeVideoEncodingOptions& options)
{
    Close();
    _error.clear();
    _sourceWidth = width;
    _sourceHeight = height;
    _width = MakeEvenDimension(width);
    _height = MakeEvenDimension(height);
    _fps = std::max(1U, fps);
    _frameIndex = 0;

    HRESULT result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(result) && result != RPC_E_CHANGED_MODE)
        return SetError("CoInitializeEx", result);
    _comInitialized = SUCCEEDED(result);

    result = MFStartup(MF_VERSION);
    if (FAILED(result))
        return SetError("MFStartup", result);
    _mediaFoundationStarted = true;

    const std::wstring outputPathWide = Utf8ToWide(outputPath);
    if (outputPathWide.empty())
    {
        _error = "The output path is empty or could not be converted to UTF-16.";
        return false;
    }

    result = MFCreateSinkWriterFromURL(outputPathWide.c_str(), nullptr, nullptr, &_writer);
    if (FAILED(result))
        return SetError("MFCreateSinkWriterFromURL", result);

    IMFMediaType* outputType = nullptr;
    result = MFCreateMediaType(&outputType);
    if (FAILED(result))
        return SetError("MFCreateMediaType", result);

    const auto releaseOutputType = [&outputType] { SafeRelease(outputType); };
    result = outputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    if (SUCCEEDED(result))
        result = outputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264);
    if (SUCCEEDED(result))
        result = outputType->SetUINT32(MF_MT_AVG_BITRATE, options.bitRate);
    if (SUCCEEDED(result))
        result = outputType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
    if (SUCCEEDED(result))
        result = MFSetAttributeSize(outputType, MF_MT_FRAME_SIZE, _width, _height);
    if (SUCCEEDED(result))
        result = MFSetAttributeRatio(outputType, MF_MT_FRAME_RATE, _fps, 1);
    if (SUCCEEDED(result))
        result = MFSetAttributeRatio(outputType, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
    if (SUCCEEDED(result))
        result = _writer->AddStream(outputType, &_streamIndex);
    releaseOutputType();
    if (FAILED(result))
        return SetError("Configuring output media type", result);

    IMFMediaType* inputType = nullptr;
    result = MFCreateMediaType(&inputType);
    if (FAILED(result))
        return SetError("MFCreateMediaType", result);

    const auto releaseInputType = [&inputType] { SafeRelease(inputType); };
    result = inputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    if (SUCCEEDED(result))
        result = inputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
    if (SUCCEEDED(result))
        result = inputType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
    if (SUCCEEDED(result))
        result = MFSetAttributeSize(inputType, MF_MT_FRAME_SIZE, _width, _height);
    if (SUCCEEDED(result))
        result = MFSetAttributeRatio(inputType, MF_MT_FRAME_RATE, _fps, 1);
    if (SUCCEEDED(result))
        result = MFSetAttributeRatio(inputType, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
    if (SUCCEEDED(result))
        result = _writer->SetInputMediaType(_streamIndex, inputType, nullptr);
    releaseInputType();
    if (FAILED(result))
        return SetError("Configuring input media type", result);

    ICodecAPI* codecApi = nullptr;
    result = _writer->GetServiceForStream(_streamIndex, GUID_NULL, IID_PPV_ARGS(&codecApi));
    if (FAILED(result))
        return SetError("Accessing video encoder settings", result);

    VARIANT quality;
    VariantInit(&quality);
    quality.vt = VT_UI4;
    quality.ulVal = std::min(options.quality, 100U);
    result = codecApi->SetValue(&CODECAPI_AVEncCommonQualityVsSpeed, &quality);
    SafeRelease(codecApi);
    if (FAILED(result))
        return SetError("Configuring video encoder quality", result);

    result = _writer->BeginWriting();
    if (FAILED(result))
        return SetError("BeginWriting", result);

    return true;
}

bool NativeVideoWriter::Impl::WriteFrame(const sf::Image& frame)
{
    if (_writer == nullptr)
    {
        _error = "Video writer has not been opened.";
        return false;
    }

    const sf::Vector2u frameSize = frame.getSize();
    if (frameSize.x < _width || frameSize.y < _height)
    {
        _error = "The rendered frame is smaller than the configured video size.";
        return false;
    }

    constexpr LONGLONG hundredNanosecondsPerSecond = 10000000;
    const DWORD stride = _width * 4;
    const DWORD bufferLength = stride * _height;

    IMFMediaBuffer* buffer = nullptr;
    HRESULT result = MFCreateMemoryBuffer(bufferLength, &buffer);
    if (FAILED(result))
        return SetError("MFCreateMemoryBuffer", result);

    BYTE* destination = nullptr;
    result = buffer->Lock(&destination, nullptr, nullptr);
    if (FAILED(result))
    {
        SafeRelease(buffer);
        return SetError("Locking frame buffer", result);
    }

    const uint8_t* source = frame.getPixelsPtr();
    for (unsigned int y = 0; y < _height; ++y)
    {
        const uint8_t* sourceRow = source + y * frameSize.x * 4;
        BYTE* destinationRow = destination + y * stride;
        for (unsigned int x = 0; x < _width; ++x)
        {
            const uint8_t* pixel = sourceRow + x * 4;
            BYTE* outputPixel = destinationRow + x * 4;
            outputPixel[0] = pixel[2];
            outputPixel[1] = pixel[1];
            outputPixel[2] = pixel[0];
            outputPixel[3] = 0xFF;
        }
    }

    buffer->Unlock();
    result = buffer->SetCurrentLength(bufferLength);
    if (FAILED(result))
    {
        SafeRelease(buffer);
        return SetError("SetCurrentLength", result);
    }

    IMFSample* sample = nullptr;
    result = MFCreateSample(&sample);
    if (FAILED(result))
    {
        SafeRelease(buffer);
        return SetError("MFCreateSample", result);
    }

    result = sample->AddBuffer(buffer);
    SafeRelease(buffer);
    if (SUCCEEDED(result))
        result = sample->SetSampleTime(static_cast<LONGLONG>(_frameIndex) * hundredNanosecondsPerSecond / _fps);
    if (SUCCEEDED(result))
        result = sample->SetSampleDuration(hundredNanosecondsPerSecond / _fps);
    if (SUCCEEDED(result))
        result = _writer->WriteSample(_streamIndex, sample);
    SafeRelease(sample);
    if (FAILED(result))
        return SetError("WriteSample", result);

    ++_frameIndex;
    return true;
}

bool NativeVideoWriter::Impl::Close()
{
    bool result = true;
    if (_writer != nullptr)
    {
        const HRESULT finalizeResult = _writer->Finalize();
        if (FAILED(finalizeResult))
            result = SetError("Finalizing video", finalizeResult);
    }

    ReleaseResources();
    return result;
}
#elif defined(__linux__)
bool NativeVideoWriter::Impl::SetError(std::string error)
{
    _error = std::move(error);
    return false;
}

void NativeVideoWriter::Impl::ReleaseResources()
{
    if (_pipeline != nullptr)
    {
        gst_element_set_state(_pipeline, GST_STATE_NULL);
        gst_object_unref(_pipeline);
        _pipeline = nullptr;
        _appSource = nullptr;
    }
}

bool NativeVideoWriter::Impl::Open(const std::string& outputPath, const unsigned int width, const unsigned int height,
                                  const unsigned int fps, const NativeVideoEncodingOptions& options)
{
    Close();
    _error.clear();
    _sourceWidth = width;
    _sourceHeight = height;
    _width = MakeEvenDimension(width);
    _height = MakeEvenDimension(height);
    _fps = std::max(1U, fps);
    _frameIndex = 0;

    gst_init(nullptr, nullptr);

    _pipeline = gst_pipeline_new("wxchaos-video-writer");
    _appSource = gst_element_factory_make("appsrc", "source");
    GstElement* videoConvert = gst_element_factory_make("videoconvert", "convert");
    GstElement* encoder = gst_element_factory_make("x264enc", "encoder");
    GstElement* parser = gst_element_factory_make("h264parse", "parser");
    GstElement* muxer = gst_element_factory_make("mp4mux", "muxer");
    GstElement* fileSink = gst_element_factory_make("filesink", "sink");

    if (_pipeline == nullptr || _appSource == nullptr || videoConvert == nullptr || encoder == nullptr ||
        parser == nullptr || muxer == nullptr || fileSink == nullptr)
    {
        ReleaseResources();
        return SetError("Could not create the GStreamer H.264 MP4 pipeline. Check that appsrc, videoconvert, x264enc, h264parse, and mp4mux plugins are installed.");
    }

    GstCaps* caps = gst_caps_new_simple(
        "video/x-raw",
        "format", G_TYPE_STRING, "RGBA",
        "width", G_TYPE_INT, static_cast<int>(_width),
        "height", G_TYPE_INT, static_cast<int>(_height),
        "framerate", GST_TYPE_FRACTION, static_cast<int>(_fps), 1,
        nullptr);

    g_object_set(_appSource,
                 "caps", caps,
                 "format", GST_FORMAT_TIME,
                 "is-live", FALSE,
                 "stream-type", GST_APP_STREAM_TYPE_STREAM,
                 nullptr);
    gst_caps_unref(caps);
    const unsigned int bitRateKbps = options.bitRate / 1000U + (options.bitRate % 1000U != 0);
    const unsigned int speedPreset = 1U + std::min(options.quality, 100U) * 8U / 100U;
    g_object_set(encoder,
                 "bitrate", bitRateKbps,
                 "tune", 0x00000004,
                 "speed-preset", speedPreset,
                 nullptr);
    g_object_set(fileSink, "location", outputPath.c_str(), nullptr);

    gst_bin_add_many(GST_BIN(_pipeline), _appSource, videoConvert, encoder, parser, muxer, fileSink, nullptr);
    if (!gst_element_link_many(_appSource, videoConvert, encoder, parser, muxer, fileSink, nullptr))
    {
        ReleaseResources();
        return SetError("Could not link the GStreamer video pipeline.");
    }

    if (gst_element_set_state(_pipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE)
    {
        ReleaseResources();
        return SetError("Could not start the GStreamer video pipeline.");
    }

    return true;
}

bool NativeVideoWriter::Impl::WriteFrame(const sf::Image& frame)
{
    if (_pipeline == nullptr || _appSource == nullptr)
        return SetError("Video writer has not been opened.");

    const sf::Vector2u frameSize = frame.getSize();
    if (frameSize.x < _width || frameSize.y < _height)
        return SetError("The rendered frame is smaller than the configured video size.");

    const size_t rowBytes = static_cast<size_t>(_width) * 4;
    GstBuffer* buffer = gst_buffer_new_allocate(nullptr, rowBytes * _height, nullptr);
    if (buffer == nullptr)
        return SetError("Could not allocate a GStreamer frame buffer.");

    GstMapInfo mapInfo;
    if (!gst_buffer_map(buffer, &mapInfo, GST_MAP_WRITE))
    {
        gst_buffer_unref(buffer);
        return SetError("Could not map a GStreamer frame buffer.");
    }

    const uint8_t* source = frame.getPixelsPtr();
    for (unsigned int y = 0; y < _height; ++y)
        std::copy_n(source + y * frameSize.x * 4, rowBytes, mapInfo.data + y * rowBytes);

    gst_buffer_unmap(buffer, &mapInfo);

    GST_BUFFER_PTS(buffer) = gst_util_uint64_scale(_frameIndex, GST_SECOND, _fps);
    GST_BUFFER_DURATION(buffer) = gst_util_uint64_scale(1, GST_SECOND, _fps);

    const GstFlowReturn flowReturn = gst_app_src_push_buffer(GST_APP_SRC(_appSource), buffer);
    if (flowReturn != GST_FLOW_OK)
        return SetError("GStreamer rejected a video frame.");

    ++_frameIndex;
    return true;
}

bool NativeVideoWriter::Impl::Close()
{
    if (_pipeline == nullptr || _appSource == nullptr)
        return true;

    gst_app_src_end_of_stream(GST_APP_SRC(_appSource));
    GstBus* bus = gst_element_get_bus(_pipeline);
    bool success = true;
    GstMessage* message = gst_bus_timed_pop_filtered(
        bus,
        30 * GST_SECOND,
        static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    if (message == nullptr)
        success = SetError("Timed out while finalizing the GStreamer video.");
    else if (GST_MESSAGE_TYPE(message) == GST_MESSAGE_ERROR)
    {
        GError* error = nullptr;
        gchar* debug = nullptr;
        gst_message_parse_error(message, &error, &debug);
        std::string errorText = error != nullptr ? error->message : "Unknown GStreamer error.";
        if (debug != nullptr)
            errorText += std::string(" ") + debug;
        if (error != nullptr)
            g_error_free(error);
        if (debug != nullptr)
            g_free(debug);
        success = SetError(errorText);
    }

    if (message != nullptr)
        gst_message_unref(message);
    gst_object_unref(bus);
    ReleaseResources();
    return success;
}
#else
bool NativeVideoWriter::Impl::SetError(std::string error)
{
    _error = std::move(error);
    return false;
}

bool NativeVideoWriter::Impl::Open(const std::string&, unsigned int, unsigned int, unsigned int,
                                   const NativeVideoEncodingOptions&)
{
    return SetError("Zoom video export is not supported on this platform.");
}

bool NativeVideoWriter::Impl::WriteFrame(const sf::Image&)
{
    return SetError("Zoom video export is not supported on this platform.");
}

bool NativeVideoWriter::Impl::Close()
{
    return true;
}
#endif

NativeVideoWriter::NativeVideoWriter() : _impl(std::make_unique<Impl>())
{
}

NativeVideoWriter::~NativeVideoWriter()
{
    _impl->Close();
}

bool NativeVideoWriter::Open(const std::string& outputPath, const unsigned int width, const unsigned int height,
                             const unsigned int fps, const NativeVideoEncodingOptions& options) const
{
    NativeVideoEncodingOptions normalizedOptions = options;
    if (normalizedOptions.bitRate == 0)
        normalizedOptions.bitRate = GetRecommendedBitRate(width, height, fps);
    normalizedOptions.quality = std::min(normalizedOptions.quality, 100U);
    return _impl->Open(outputPath, width, height, fps, normalizedOptions);
}

bool NativeVideoWriter::WriteFrame(const sf::Image& frame) const
{
    return _impl->WriteFrame(frame);
}

bool NativeVideoWriter::Close() const
{
    return _impl->Close();
}

std::string NativeVideoWriter::GetError() const
{
    return _impl->GetError();
}

unsigned int NativeVideoWriter::GetRecommendedBitRate(const unsigned int width, const unsigned int height,
                                                      const unsigned int fps)
{
    return Impl::CalculateBitRate(Impl::MakeEvenDimension(width), Impl::MakeEvenDimension(height), std::max(1U, fps));
}
