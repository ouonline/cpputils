project = CreateProject()

project:CreateBinary("test_text_utils"):AddDependencies(
    project:CreateDependency()
        :AddSourceFiles("test_text_utils.cpp")
        :AddFlags("-Wall", "-Werror", "-Wextra")
        :AddStaticLibrary("..", "cpputils_static"))

project:CreateBinary("test_skiplist_set"):AddDependencies(
    project:CreateDependency()
        :AddSourceFiles("test_skiplist_set.cpp")
        :AddFlags("-Wall", "-Werror", "-Wextra")
        :AddStaticLibrary("..", "cpputils_static"))

return project
