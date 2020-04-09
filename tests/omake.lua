project = CreateProject()

target = project:CreateBinary("test_text_utils")
target:AddSourceFiles("*.cpp")
target:AddFlags("-Wall", "-Werror", "-Wextra", "-fPIC")
target:AddStaticLibrary("..", "text_utils_static")

return project
