#include "PNGWriter.h"
#include "common.h"

#include <ranges>

#if DEBUG && 1
#include "escapes.h"
#include <print>
#define PNGDEBUG 1
#define PNGRELEASE 0
#define PNGDEBUGONLY(...) __VA_ARGS__
#else
#define PNGDEBUG 0
#define PNGRELEASE 1
#define PNGDEBUGONLY(...)
#endif


/////////////
// HELPERS //
/////////////
namespace {
#if PNGDEBUG
    void debugPath(const std::filesystem::path& path) {
        static const char* colorOn = COLOR_GRN;
        static const char* colorOff = COLOR_GRY;
        const std::filesystem::path base = std::filesystem::current_path();
        std::println("path.{}root_name" ESC_OFF "()      => \"{}\"", path.has_root_name()      ? colorOn : colorOff, path.root_name().string());
        std::println("path.{}root_directory" ESC_OFF "() => \"{}\"", path.has_root_directory() ? colorOn : colorOff, path.root_directory().string());
        std::println("path.{}root_path" ESC_OFF "()      => \"{}\"", path.has_root_path()      ? colorOn : colorOff, path.root_path().string());
        std::println("path.{}relative_path" ESC_OFF "()  => \"{}\"", path.has_relative_path()  ? colorOn : colorOff, path.relative_path().string());
        std::println("path.{}parent_path" ESC_OFF "()    => \"{}\"", path.has_parent_path()    ? colorOn : colorOff, path.parent_path().string());
        std::println("path.{}filename" ESC_OFF "()       => \"{}\"", path.has_filename()       ? colorOn : colorOff, path.filename().string());
        std::println("path.{}stem" ESC_OFF "()           => \"{}\"", path.has_stem()           ? colorOn : colorOff, path.stem().string());
        std::println("path.{}extension" ESC_OFF "()      => \"{}\"", path.has_extension()      ? colorOn : colorOff, path.extension().string());
        std::println("path.native() "         "        => \"{}\"", path.native());
        std::println("std::filesystem::temp_directory_path() => \"{}\"", std::filesystem::temp_directory_path().string());
        std::println("base = std::filesystem::current_path() => \"{}\"", base.string());
        std::println("path.lexically_normal()        => \"{}\"", path.lexically_normal().string());
        std::println("path.lexically_relative(base)  => \"{}\"", path.lexically_relative(base).string());
        std::println("path.lexically_proximate(base) => \"{}\"", path.lexically_proximate(base).string());
        std::println("Path {} absolute" ESC_OFF "; path {} relative" ESC_OFF, path.is_absolute() ? COLOR_GRN "is" : COLOR_GRY "is not", path.is_relative() ? COLOR_GRN "is" : COLOR_GRY "is not");
    }

    std::string permString(std::filesystem::perms p) {
        using std::filesystem::perms;
        return {
            (perms::owner_read   & p) == perms::none ? '-' : 'r',
            (perms::owner_write  & p) == perms::none ? '-' : 'w',
            (perms::owner_exec   & p) == perms::none ? '-' : 'x',
            (perms::group_read   & p) == perms::none ? '-' : 'r',
            (perms::group_write  & p) == perms::none ? '-' : 'w',
            (perms::group_exec   & p) == perms::none ? '-' : 'x',
            (perms::others_read  & p) == perms::none ? '-' : 'r',
            (perms::others_write & p) == perms::none ? '-' : 'w',
            (perms::others_exec  & p) == perms::none ? '-' : 'x',
        };
    }
#endif

    std::filesystem::path makePath(std::string&& filename) {
#if !defined(_WIN32) || defined(__CYGWIN__)
        if (filename.starts_with("~")) {
            std::string rootPath;
            if (filename.starts_with("~/") || filename.length() == 1uz) {
                rootPath = std::getenv("HOME");
                if (rootPath.empty())
                    throw util::Critical("Could not resolve \"HOME\" environment variable");
            } else {
                rootPath = "/home/";
            }
            filename = rootPath + filename.substr(1uz);
        }
#endif

        std::filesystem::path path(filename);
        std::error_code ec;

        PNGDEBUGONLY(debugPath(path);)

        if (!path.has_stem())
            throw util::Critical("Could not determine filename from path \"{}\"", filename);

        path.replace_extension("png");

        std::filesystem::path parent = path.has_parent_path() ? path.parent_path() : std::filesystem::current_path(ec);
        if (ec)
            throw util::Critical("Could not resolve local directory");

        if (!std::filesystem::exists(parent, ec) || ec)
            throw util::Critical("Directory \"{}\" does not exist", parent.string());
        if (!std::filesystem::is_directory(parent, ec) || ec)
            throw util::Critical("Parent \"{}\" is not a directory", parent.string());

        if (std::filesystem::exists(path, ec)) {
            PNGDEBUGONLY(std::println("I think the file already exists");)
            if (ec)
                throw util::Critical("Could not access \"{}\"", path.filename().string());
            auto st = std::filesystem::status(path, ec);
            if (ec)
                throw util::Critical("Cannot read permissions of \"{}\"", filename);
            auto perms = st.permissions();
            PNGDEBUGONLY(std::println("Perms of \"{}\": {}", path.string(), permString(perms));)
            // If any write bit is set, assume likely writable (heuristic).
            if ((perms & (std::filesystem::perms::owner_write | std::filesystem::perms::group_write | std::filesystem::perms::others_write)) == std::filesystem::perms::none)
                throw util::Critical("No write permissions for \"{}\"", path.filename().string());
        } else {
            PNGDEBUGONLY(std::println("I do not think the file exists");)
            // Target doesn't already exist; if parent is writable by permissions bits, assume likely writable.
            auto pst = std::filesystem::status(parent, ec);
            if (ec)
                throw util::Critical("Cannot read permissions of \"{}\"", parent.string());
            auto perms = pst.permissions();
            PNGDEBUGONLY(std::println("Perms of \"{}\": {}", parent.string(), permString(perms));)
            if ((perms & (std::filesystem::perms::owner_write | std::filesystem::perms::group_write | std::filesystem::perms::others_write)) == std::filesystem::perms::none)
                throw util::Critical("No write permissions for \"{}\"", path.filename().string());
        }

        return path;
    }

    int channelCount(PixelFormat fmt) {
        switch (fmt) {
            case PixelFormat::GRAY: return 1;
            case PixelFormat::RGB:  return 3;
            case PixelFormat::RGBA: return 4;
            default:                return 3;
        }
    }
}


///////////////
// PNGWriter //
///////////////
PNGWriter::PNGWriter(std::string&& filename)
  : m_path(makePath(std::forward<std::string>(filename)))
  , m_file(std::fopen(m_path.c_str(), "wb")) {
    if (!m_file)
        throw util::Critical("Failed to open \"{}\" for writing", m_path.filename().string());
}

PNGWriter::~PNGWriter() {
    std::fclose(m_file);
}

void PNGWriter::write(std::vector<png_byte>& pixels, PixelFormat format, size_t width, size_t height, size_t bitDepth) {
    const size_t channels = channelCount(format);
    const size_t expectedSize = width * height * channels;

    if (pixels.size() != expectedSize)
        throw util::Critical("Pixel buffer size mismatch");

    png_struct* png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png)
        throw util::Critical("png_create_write_struct failed");

    png_info* info = png_create_info_struct(png);
    if (!info) {
        png_destroy_write_struct(&png, nullptr);
        throw util::Critical("png_create_info_struct failed");
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        throw util::Critical("libpng error during write");
    }

    png_init_io(png, m_file);

    png_set_IHDR(png,
                 info,
                 width,
                 height,
                 bitDepth,
                 static_cast<int>(format),
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_BASE);

    // --- Future hook ---
    // if (useSRGB) {
    //     png_set_sRGB(png, info, PNG_sRGB_INTENT_PERCEPTUAL);
    // }

    png_write_info(png, info);

    const size_t stride = width * channels;
    std::vector rows{std::from_range, std::views::transform(std::views::chunk(pixels, width * channels), [](auto&& chunk) { return chunk.data(); })};

    png_write_image(png, rows.data());
    png_write_end(png, nullptr);

    png_destroy_write_struct(&png, &info);
}
