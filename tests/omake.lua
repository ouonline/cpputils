project = CreateProject()

project:CreateBinary("test_text_utils"):AddDependencies(
    project:CreateDependency()
        :AddSourceFiles("test_text_utils.cpp")
        :AddFlags("-Wall", "-Werror", "-Wextra")
        :AddStaticLibrary("..", "cpputils_static"))

project:CreateBinary("test_skiplist"):AddDependencies(
    project:CreateDependency()
        :AddSourceFiles("test_skiplist.cpp")
        :AddFlags("-Wall", "-Werror", "-Wextra")
        :AddStaticLibrary("..", "cpputils_static"))

return project
