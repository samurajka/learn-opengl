namespace rectangle{
    float vertices[] = {
        0.7f, 0.7f, 0.0f,
        0.7f, -0.7f, 0.0f,
        -0.7f, -0.7f, 0.0f,
        -0.7f, 0.7f, 0.0f
    };

    unsigned int indices[] = {
        0,1,3,
        1,2,3
    };

    auto vertices_without_indices = {
        0.7f, 0.7f, 0.0f,
        0.7f, -0.7f, 0.0f,
        -0.7f, -0.7f, 0.0f,
        -0.7f, -0.7f, 0.0f,
        -0.7f, 0.7f, 0.0f,
        0.7f, 0.7f, 0.0f,
    };

    auto vertices_color = {
        0.7f, 0.7f, 0.0f,       1.0f, 0.0f, 0.0f,
        0.7f, -0.7f, 0.0f,      0.0f, 1.0f, 0.0f,
        -0.7f, -0.7f, 0.0f,     0.0f, 0.0f, 1.0f,
        -0.7f, -0.7f, 0.0f,     0.0f, 0.0f, 1.0f,
        -0.7f, 0.7f, 0.0f,      0.0f, 1.0f, 1.0f,
        0.7f, 0.7f, 0.0f,       1.0f, 0.0f, 0.0f,
    };
}