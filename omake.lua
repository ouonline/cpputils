project = Project()

dep = project:CreateDependency()
    :AddSourceFiles("*.cpp")
    :AddStaticLibrary("../math", "math_static")
    :AddFlags("-Wall", "-Werror", "-Wextra", "-fPIC")

project:CreateStaticLibrary("cpputils_static"):AddDependencies(dep)
project:CreateSharedLibrary("cpputils_shared"):AddDependencies(dep)

return project
