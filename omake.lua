project = CreateProject()

dep = project:CreateDependency()
dep:AddSourceFiles("*.cpp")
dep:AddFlags("-Wall", "-Werror", "-Wextra", "-fPIC")

a = project:CreateStaticLibrary("text_utils_static")
a:AddDependencies(dep)

so = project:CreateSharedLibrary("text_utils_shared")
so:AddDependencies(dep)

return project
