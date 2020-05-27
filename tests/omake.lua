project = CreateProject()

project:CreateBinary("test_cpp_utils"):AddDependencies(
    project:CreateDependency()
        :AddSourceFiles("*.cpp")
        :AddFlags("-Wall", "-Werror", "-Wextra")
        :AddStaticLibrary("..", "cpputils_static"))

return project
