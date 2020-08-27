project = Project()

flags = {"-Wall", "-Werror", "-Wextra"}

project:CreateBinary("test_text_utils"):AddDependencies(
    project:CreateDependency()
        :AddFlags(flags)
        :AddSourceFiles("test_text_utils.cpp")
        :AddStaticLibraries("..", "cpputils_static"))

project:CreateBinary("test_skiplist"):AddDependencies(
    project:CreateDependency()
        :AddFlags(flags)
        :AddSourceFiles("test_skiplist.cpp")
        :AddStaticLibraries("..", "cpputils_static"))

return project
