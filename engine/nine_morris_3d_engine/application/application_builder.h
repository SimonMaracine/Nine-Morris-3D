#pragma once

class ApplicationBuilder {
public:
    enum class Renderer {
        R3D, R2D, RImGui
    };

    ApplicationBuilder() = default;
    ~ApplicationBuilder() = default;

    ApplicationBuilder& display_config(int width, int height, std::string_view title = "Nine Morris 3D Engine",
        bool fullscreen = false, bool native_resolution = true, bool resizable = true, int min_width = -1, int min_height = -1);
    ApplicationBuilder& file_names_config(std::string_view application_name, std::string_view info_file_name);
    ApplicationBuilder& version_config(unsigned int major, unsigned int minor, unsigned int patch);
    ApplicationBuilder& authors_config(const std::vector<std::string>& authors);
    ApplicationBuilder& encrypt_key_config(std::string_view encrypt_key);
    ApplicationBuilder& with(Renderer renderer);
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
    std::vector<std::string> authors;
    std::string encrypt_key = "Nine Morris 3D Engine";
    bool renderer_3d = false;
    bool renderer_2d = false;
    bool renderer_imgui = false;

    friend class Application;
};
