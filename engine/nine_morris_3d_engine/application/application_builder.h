#pragma once

class ApplicationBuilder {
public:
    enum Renderer {
        R3D, R2D, RImGui
    };

    ApplicationBuilder() = default;
    ~ApplicationBuilder() = default;

    ApplicationBuilder& display(int width, int height, std::string_view title = "Nine Morris 3D Engine");
    ApplicationBuilder& display_flags(bool fullscreen, bool native_resolution, bool resizable);
    ApplicationBuilder& display_min_resolution(int min_width, int min_height);
    ApplicationBuilder& file_names(std::string_view application_name, std::string_view info_file_name);
    ApplicationBuilder& version(unsigned int major, unsigned int minor, unsigned int patch);
    ApplicationBuilder& authors(const std::vector<std::string>& author_list);
    ApplicationBuilder& encrypt_key(std::string_view encryption_key);
    ApplicationBuilder& with_renderer(Renderer renderer);
private:
    int width = 800;
    int height = 600;
    std::string title = "Nine Morris 3D Engine";
    bool fullscreen = false;
    bool native_resolution = true;
    bool resizable = true;
    int min_width = -1;
    int min_height = -1;
    std::string application_name = "NineMorris3DEngineExampleApp";
    std::string info_file_name = "NineMorris3DEngineInfo.txt";
    unsigned int major = 0;
    unsigned int minor = 1;
    unsigned int patch = 0;
    std::vector<std::string> author_list;
    std::string encryption_key = "Nine Morris 3D Engine";
    bool renderer_3d = false;
    bool renderer_2d = false;
    bool renderer_imgui = false;

    friend class Application;
};
